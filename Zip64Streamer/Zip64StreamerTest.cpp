/**
 * @file Zip64StreamerTest.cpp
 *
 * @author Anthony Foiani <anthony@foiani.com>
 */

// standard C++ headers
#include <fstream>

// local headers
#include "Compat.hpp"

// header under test
#include "Zip64Streamer.hpp"

namespace // anonymous
{

using namespace com::foiani;

class FileSender
    : public Zip64Streamer::Sender
{

public:
    FileSender( const string & filename );
    ~FileSender();

    virtual void send( CharBuffer & b );
    virtual void send( string & s );

private:

    std::ofstream m_ofs;

};

FileSender::FileSender( const string & filename )
    : m_ofs( filename )
{
    DEBUG( "fs: ctor: done" );
}

FileSender::~FileSender()
{
    DEBUG( "fs: dtor: done" );
}

/* virtual */ void
FileSender::send( CharBuffer & b )
{
    CharBuffer tmp;
    tmp.swap( b );
    m_ofs.write( &tmp[0], tmp.size() );
}

/* virtual */ void
FileSender::send( string & s )
{
    string tmp;
    tmp.swap( s );
    m_ofs.write( tmp.data(), tmp.size() );
}

} // end namespace [anonymous]

int
main( int argc, char * argv [] )
{
    if ( argc < 3 )
    {
        ERROR( "usage: " << argv[0] << " ZIPFILE [FILE/PATTERN...]" );
        return 1;
    }

    const string zipFile( argv[1] );
    DEBUG( "creating file sender for " << QS( zipFile ) );
    FileSender sender( argv[1] );

    const string dir( "." );
    DEBUG( "creating zip streamer in dir " << QS( dir ) );
    Zip64Streamer z64s( dir, sender );

    for ( int i = 2; i < argc; ++i )
    {
        const string pat( argv[i] );
        FINE( "adding pattern " << QS( pat ) );
        z64s.addFileByPattern( pat );
    }
    DEBUG( "done adding patterns" );

    return 0;
}

