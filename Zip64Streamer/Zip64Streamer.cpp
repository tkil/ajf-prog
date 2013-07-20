/**
 * @file Zip64Streamer.cpp
 *
 * @author Anthony Foiani <anthony@foiani.com>
 */

// standard c / posix headers
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

// standard C++ headers
#include <fstream>

// boost headers

// project headers

// local headers
#include "Compat.hpp"

// interface
#include "Zip64Streamer.hpp"

namespace // anonymous
{

using namespace com::foiani;

const uint32_t LOCAL_FILE_HEADER_SIG = 0x04034b50;

// need version 4.5 for zip64 support
const uint16_t VERSION_NEEDED_TO_EXTRACT_4_5 = 45;
const uint16_t VERSION_CREATED_BY_4_5_UNIX   =
  ( 3 << 8 |  // unix
    VERSION_NEEDED_TO_EXTRACT_4_5 );

// GPB == "general purpose bits"
const uint16_t GPB_NO_FLAGS = 0;
const uint16_t GPB_DATA_DESC_FOLLOWS_DATA = 1 << 3;

const uint16_t COMPRESSION_METHOD_DEFLATE = 8; // deflate

const uint32_t DEFER_CRC32 = 0;
const uint32_t DEFER_COMPRESSED_SIZE = 0;
const uint32_t DEFER_UNCOMPRESSED_SIZE = 0;

const uint32_t FORCE_Z64_COMPRESSED_SIZE = 0xffffffff;
const uint32_t FORCE_Z64_UNCOMPRESSED_SIZE = 0xffffffff;
const uint32_t FORCE_Z64_OFFSET = 0xffffffff;
const uint16_t FORCE_Z64_LOCAL_ENTRIES = 0xffff;
const uint16_t FORCE_Z64_TOTAL_ENTRIES = 0xffff;
const uint32_t FORCE_Z64_CDIR_SIZE = 0xffffffff;
const uint32_t FORCE_Z64_CDIR_OFFSET = 0xffffffff;

const uint16_t LENGTH_PLACEHOLDER = 0;

const uint16_t Z64_EXTRA_FIELD_TAG = 0x0001;
const uint16_t Z64_EXTRA_FIELD_HEADER_LENGTH = 4;
const uint16_t Z64_EXTRA_FIELD_UNCOMPRESSED_BYTES = 4;
const uint16_t Z64_EXTRA_FIELD_COMPRESSED_BYTES = 4;

const uint16_t UNIX_EXTRA_FIELD_TAG = 0x000d;
const uint16_t UNIX_EXTRA_FIELD_LENGTH = 16;
const uint16_t UNIX_ZIP_UID = 0;
const uint16_t UNIX_ZIP_GID = 0;

const uint32_t DATA_DESC_SIG = 0x08074b50;

const uint32_t CDIR_FILE_HEADER_SIG = 0x02014b50;

const uint16_t DISK_START_ZERO = 0;
const uint16_t DISK_NUMBER_ZERO = 0;
const uint16_t DISK_TOTAL_ONE = 1;

const uint16_t ZERO_COMMENT_LENGTH = 0;
const uint16_t ZERO_INTERNAL_FILE_ATTR = 0;
const uint32_t UNIX_EXTERNAL_FILE_ATTR = (
    ( 0100666 << 16 ) | // regular file, rw by all
    (       1 << 13 ) | // GMT timestamps
    (       1 << 12 )   // uid/gid present
);

const uint32_t Z64_END_OF_CENTRAL_DIR_REC_SIG = 0x06064b50;
const uint32_t Z64_END_OF_CENTRAL_DIR_LOC_SIG = 0x07064b50;

const uint32_t END_OF_CENTRAL_DIR_SIG = 0x06054b50;

#if EXTRA_DEBUGGING

void
dumpBytes( const string & label,
           const CharBuffer & buf,
           const size_t begin,
           const size_t end )
{
    std::ostringstream oss;
    for ( size_t i = begin; i < end; ++i )
        oss << " " << std::hex << std::setw( 2 ) << std::setfill( '0' ) << +buf.at(i);
    FINE( label << "[" << begin << "-" << end << "]:" << oss.str() );
}

#else

void
dumpBytes( const string &,
           const CharBuffer &,
           const size_t,
           const size_t )
{
}

#endif

void
fixupExtraFieldLength( CharBuffer & cb )
{
    if ( cb.size() >= 4 )
    {
        const uint64_t size( cb.size() - 4 );
        dumpBytes( "fefl: before", cb, 0, 4 );
        cb[2] = static_cast< char >( size      );
        cb[3] = static_cast< char >( size >> 8 );
        dumpBytes( "fefl:  after", cb, 0, 4 );
    }
}

void
fixupRecordLength64( CharBuffer & cb )
{
    if ( cb.size() >= 12 )
    {
        dumpBytes( "frl64: before", cb, 0, 12 );
        const uint64_t size( cb.size() - 12 );
        cb[  4 ] = static_cast< char >( size       );
        cb[  5 ] = static_cast< char >( size >>  8 );
        cb[  6 ] = static_cast< char >( size >> 16 );
        cb[  7 ] = static_cast< char >( size >> 24 );
        cb[  8 ] = static_cast< char >( size >> 32 );
        cb[  9 ] = static_cast< char >( size >> 40 );
        cb[ 10 ] = static_cast< char >( size >> 48 );
        cb[ 11 ] = static_cast< char >( size >> 56 );
        dumpBytes( "frl64:  after", cb, 0, 12 );
    }
}

uint16_t
clampUInt16( const uint64_t val )
{
    if ( val < 0xffff )
        return static_cast< uint16_t >( val );
    else
        return 0xffff;
}

uint32_t
clampUInt32( const uint64_t val )
{
    if ( val < 0xffffffff )
        return static_cast< uint32_t >( val );
    else
        return 0xffffffff;
}

void
write2( CharBuffer & cb, const uint16_t val )
{
    cb.push_back( static_cast< char >( val      ) );
    cb.push_back( static_cast< char >( val >> 8 ) );
}

void
write4( CharBuffer & cb, const uint32_t val )
{
    cb.push_back( static_cast< char >( val       ) );
    cb.push_back( static_cast< char >( val >>  8 ) );
    cb.push_back( static_cast< char >( val >> 16 ) );
    cb.push_back( static_cast< char >( val >> 24 ) );
}

void
write8( CharBuffer & cb, const uint64_t val )
{
    cb.push_back( static_cast< char >( val       ) );
    cb.push_back( static_cast< char >( val >>  8 ) );
    cb.push_back( static_cast< char >( val >> 16 ) );
    cb.push_back( static_cast< char >( val >> 24 ) );
    cb.push_back( static_cast< char >( val >> 32 ) );
    cb.push_back( static_cast< char >( val >> 40 ) );
    cb.push_back( static_cast< char >( val >> 48 ) );
    cb.push_back( static_cast< char >( val >> 56 ) );
}

} // end namespace anonymous

namespace com
{

namespace /* com:: */ foiani
{

Zip64Streamer::Zip64Streamer( const string & directory,
                              Sender & sender )
    : m_sDir( directory ),
      m_sender( sender ),
      m_offset( 0 ),
      m_bZStreamNeedsReset( false )
{
    DEBUG( "ctor: initializing zlib" );

    zeroStruct( m_zs );

    m_zs.zalloc = 0;
    m_zs.zfree = 0;
    m_zs.opaque = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    int rc = deflateInit2(
        &m_zs,
        Z_DEFAULT_COMPRESSION,
        Z_DEFLATED,
        -15 /* ZLIB_WINDOW_BITS, negative = raw deflate data */,
        8   /* ZLIB_MEMORY_LEVEL */,
        Z_DEFAULT_STRATEGY
    );

#pragma GCC diagnostic pop

    if ( rc != Z_OK )
        throw std::runtime_error( "initializing compression, rc=" + std::to_string( rc ) );

    DEBUG( "ctor: done" );
}

Zip64Streamer::~Zip64Streamer()
{
    DEBUG( "dtor: finishing zip file" );

    // save start of central directory
    const uint64_t centralDirOffset( m_offset );

    // emit a central directory record for each file
    for ( const FileInfo & fi : m_fileInfo )
    {
        FINE( "dtor: adding central dir record for " << QS( fi.name ) );

        CharBuffer z64;
        write2( z64, Z64_EXTRA_FIELD_TAG );
        write2( z64, LENGTH_PLACEHOLDER );
        write8( z64, fi.uncompressed );
        write8( z64, fi.compressed );
        write8( z64, fi.offset );
        fixupExtraFieldLength( z64 );

        CharBuffer unix;
        write2( unix, UNIX_EXTRA_FIELD_TAG );
        write2( unix, LENGTH_PLACEHOLDER );
        write4( unix, fi.stat_atime );
        write4( unix, fi.stat_mtime );
        write2( unix, UNIX_ZIP_UID );
        write2( unix, UNIX_ZIP_GID );
        fixupExtraFieldLength( unix );

        CharBuffer cd;
        write4( cd, CDIR_FILE_HEADER_SIG );
        write2( cd, VERSION_CREATED_BY_4_5_UNIX );
        write2( cd, VERSION_NEEDED_TO_EXTRACT_4_5 );
        write2( cd, GPB_DATA_DESC_FOLLOWS_DATA );
        write2( cd, COMPRESSION_METHOD_DEFLATE );
        write2( cd, fi.msdos_time );
        write2( cd, fi.msdos_date );
        write4( cd, fi.crc32 );
        write4( cd, FORCE_Z64_COMPRESSED_SIZE );
        write4( cd, FORCE_Z64_UNCOMPRESSED_SIZE );
        write2( cd, static_cast< uint16_t >( fi.name.size() ) );
        write2( cd, static_cast< uint16_t >( z64.size() + unix.size() ) );
        write2( cd, ZERO_COMMENT_LENGTH );
        write2( cd, DISK_START_ZERO );
        write2( cd, ZERO_INTERNAL_FILE_ATTR );
        write4( cd, UNIX_EXTERNAL_FILE_ATTR );
        write4( cd, FORCE_Z64_OFFSET );

        emit( cd );
        emitCopy( fi.name );
        emit( z64 );
        emit( unix );
    }

    // how many bytes did that use?
    const uint64_t centralDirBytes( m_offset - centralDirOffset );

    // and where are we now?
    const uint64_t z64EndOfCentralDirLoc( m_offset );

    FINE( "dtor: central dir: "
          "bytes=" << centralDirBytes << ", "
          "offset=" << centralDirOffset );

    DEBUG( "dtor: adding z64 end of central directory record @ " << m_offset );
    CharBuffer z64;
    write4( z64, Z64_END_OF_CENTRAL_DIR_REC_SIG );
    write8( z64, LENGTH_PLACEHOLDER );
    write2( z64, VERSION_CREATED_BY_4_5_UNIX );
    write2( z64, VERSION_NEEDED_TO_EXTRACT_4_5 );
    write4( z64, DISK_NUMBER_ZERO );
    write4( z64, DISK_START_ZERO );
    write8( z64, m_fileInfo.size() ); // # dir entries on this disk
    write8( z64, m_fileInfo.size() ); // # dir entries total
    write8( z64, centralDirBytes );
    write8( z64, centralDirOffset );
    fixupRecordLength64( z64 );
    emit( z64 );

    DEBUG( "dtor: adding z64 end of central directory locator @ " << m_offset );
    CharBuffer loc;
    write4( loc, Z64_END_OF_CENTRAL_DIR_LOC_SIG );
    write4( loc, DISK_NUMBER_ZERO );
    write8( loc, z64EndOfCentralDirLoc );
    write4( loc, DISK_TOTAL_ONE );
    emit( loc );

    DEBUG( "dtor: adding end of central directory record @ " << m_offset);
    CharBuffer end;
    write4( end, END_OF_CENTRAL_DIR_SIG );
    write2( end, DISK_NUMBER_ZERO );
    write2( end, DISK_START_ZERO );
    write2( end, FORCE_Z64_LOCAL_ENTRIES );
    write2( end, FORCE_Z64_TOTAL_ENTRIES );
    write4( end, FORCE_Z64_CDIR_SIZE );
    write4( end, FORCE_Z64_CDIR_OFFSET );
    write2( end, ZERO_COMMENT_LENGTH );
    emit( end );

    DEBUG( "dtor: finalizing zlib" );
    deflateEnd( &m_zs );

    DEBUG( "dtor: done" );
}

bool
Zip64Streamer::addFile( const string & file )
{
    DEBUG( "af: adding file " << QS( file ) );

    FileInfo fi;
    fi.path = m_sDir + "/" + file;
    fi.name = file;
    fi.offset = m_offset;

    fillDateTime( fi );

    CharBuffer z64;
    write2( z64, Z64_EXTRA_FIELD_TAG );
    write2( z64, LENGTH_PLACEHOLDER );
    write8( z64, DEFER_UNCOMPRESSED_SIZE );
    write8( z64, DEFER_COMPRESSED_SIZE );
    fixupExtraFieldLength( z64 );

    CharBuffer unix;
    write2( unix, UNIX_EXTRA_FIELD_TAG );
    write2( unix, LENGTH_PLACEHOLDER );
    write4( unix, fi.stat_atime );
    write4( unix, fi.stat_mtime );
    write2( unix, UNIX_ZIP_UID );
    write2( unix, UNIX_ZIP_GID );
    fixupExtraFieldLength( unix );

    CharBuffer lh; // local header
    write4( lh, LOCAL_FILE_HEADER_SIG );
    write2( lh, VERSION_NEEDED_TO_EXTRACT_4_5 );
    write2( lh, GPB_DATA_DESC_FOLLOWS_DATA );
    write2( lh, COMPRESSION_METHOD_DEFLATE );
    write2( lh, fi.msdos_time );
    write2( lh, fi.msdos_date );
    write4( lh, DEFER_CRC32 );
    write4( lh, FORCE_Z64_COMPRESSED_SIZE );
    write4( lh, FORCE_Z64_UNCOMPRESSED_SIZE );
    write2( lh, static_cast< uint16_t >( fi.name.size() ) );
    write2( lh, static_cast< uint16_t >( z64.size() + unix.size() ) );

    FINE( "af: " << file << ": writing header" );

    emit( lh );
    emitCopy( fi.name );
    emit( z64 );
    emit( unix );

    emitCompressedData( fi );

    FINE( "af: " << file << ": writing descriptor" );
    CharBuffer dd; // data descriptor
    write4( dd, DATA_DESC_SIG );
    write4( dd, fi.crc32 );
    write8( dd, fi.compressed );
    write8( dd, fi.uncompressed );
    emit( dd );

    // save info for eventual use in central directory
    m_fileInfo.push_back( fi );

    return true;
}

size_t
Zip64Streamer::addFileByPattern( const string & pattern )
{
    DEBUG( "afbp: adding pattern " << QS( pattern ) );

    const StringList files( globFiles( m_sDir, pattern ) );
    for ( const string & file : files )
        addFile( file );
    return files.size();
}

void
Zip64Streamer::emit( CharBuffer & cb )
{
    m_offset += cb.size();
    m_sender.send( cb );
}

void
Zip64Streamer::emit( string & s )
{
    m_offset += s.size();
    m_sender.send( s );
}

void
Zip64Streamer::emitCopy( const string & s )
{
    string tmp( s );
    emit( tmp );
}

void
Zip64Streamer::fillDateTime( FileInfo & fi )
{
    struct stat st;
    int rc = stat( fi.path.c_str(), &st );
    if ( rc != 0 )
        throw OSError( "stat" );

    fi.stat_atime = static_cast< uint32_t >( st.st_atime );
    fi.stat_mtime = static_cast< uint32_t >( st.st_mtime );

    // yes, this is a little insane.  these are the bits:
    //   date = YYYYYYYM MMMDDDDD   time = HHHHHMMM MMMSSSSS
    // notes:
    //   year starts at 1980, month is 1-12, day is 1-31
    //   hour is 0-23, minute is 0-59, seconds is 0-29 (times 2)

    struct tm mtm;
    gmtime_r( &st.st_mtime, &mtm );

    fi.msdos_date = static_cast< uint16_t >(
        ( mtm.tm_year - 80 ) << 9 |
        ( mtm.tm_mon  +  1 ) << 5 |
        ( mtm.tm_mday      )
    );
    fi.msdos_time = static_cast< uint16_t >(
        mtm.tm_hour << 11 |
        mtm.tm_min  <<  5 |
        mtm.tm_sec  >>  1
    );

    FINE( "fdt: unix=["
          "y=" << mtm.tm_year << ", "
          "m=" << mtm.tm_mon << ", "
          "d=" << mtm.tm_mday << "; "
          "h=" << mtm.tm_hour << ", "
          "m=" << mtm.tm_min << ", "
          "s=" << mtm.tm_sec << "]" );
    FINE( "fdt: msdos=["
          "y=" << ( ( fi.msdos_date >>  9 ) & 0x007f ) << ", "
          "m=" << ( ( fi.msdos_date >>  5 ) & 0x000f ) << ", "
          "d=" << ( ( fi.msdos_date       ) & 0x001f ) << "; "
          "h=" << ( ( fi.msdos_time >> 11 ) & 0x001f ) << ", "
          "m=" << ( ( fi.msdos_time >>  5 ) & 0x003f ) << ", "
          "s=" << ( ( fi.msdos_time <<  1 ) & 0x003e ) << "]" );
}

void
Zip64Streamer::emitCompressedData( FileInfo & fi )
{
    DEBUG( "ecd: " << fi.name << ": writing compressed data" );

    std::ifstream ifs( fi.path );

    uLong crc = crc32( 0, Z_NULL, 0 );

    if ( m_bZStreamNeedsReset )
        deflateReset( &m_zs );

    while ( true )
    {
        CharBuffer input;
        input.resize( 32 * 1024 );
        ifs.read( &input[0], input.size() );
        const std::streamsize nRead = ifs.gcount();

        m_zs.next_in = reinterpret_cast< unsigned char * >( &input[0] );
        m_zs.avail_in = static_cast< unsigned int >( nRead );
        crc = crc32( crc, m_zs.next_in, m_zs.avail_in );

        CharBuffer output;
        output.resize( 32 * 1024 );
        m_zs.next_out  = reinterpret_cast< unsigned char * >( &output[0] );
        m_zs.avail_out = static_cast< unsigned int >( output.size() );

        const int flag = ( nRead > 0 ? Z_NO_FLUSH : Z_FINISH );
        const int rc = deflate( &m_zs, flag );

        const size_t used( output.size() - m_zs.avail_out );

        FINE( "af: compressing: read " << nRead << ", got " << used );

        if ( ( flag == Z_NO_FLUSH && rc != Z_OK         ) ||
             ( flag == Z_FINISH   && rc != Z_STREAM_END ) )
            throw std::runtime_error( "compressing, rc=" + std::to_string( rc ) );

        if ( used )
        {
            output.resize( used );
            emit( output );
        }

        if ( nRead == 0 )
            break;
    }

    fi.crc32 = static_cast< uint32_t >( crc );
    fi.compressed = m_zs.total_out;
    fi.uncompressed = m_zs.total_in;

    m_bZStreamNeedsReset = true;
}

} // end namespace com::foiani

} // end namespace com
