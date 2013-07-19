#ifndef COM_FOIANI_Z64S_ZIP64STREAMER_HPP
#define COM_FOIANI_Z64S_ZIP64STREAMER_HPP 1

/**
 * @file Zip64Streamer.hpp
 *
 * @author Anthony Foiani <anthony@foiani.com>
 */

// standard C / Unix / library headers
#include <zlib.h>

// standard C++ headers
#include <cstdint>
#include <vector>

// boost headers

// project headers

// local headers
#include "Compat.hpp"

namespace com
{

namespace /* com:: */ foiani
{

/** Stream zero or more files as a ZIP64 archive. */
class Zip64Streamer
{

public:

    /** Abstract base for sending data to its destination. */
    struct Sender
    {
        // these are non-constant, to allow consumers to use 'swap' or
        // other constant-time methods to grab the data.

        virtual void send( CharBuffer & b ) = 0;
        virtual void send( string & s ) = 0;
    };

    /** Start the streamer in directory @a dir.  */
    Zip64Streamer( const string & dir, Sender & sender );

    /** Standard destructor. */
    ~Zip64Streamer();

    /** Add a single @a file (relative to dir given in constructor). */
    bool addFile( const string & file );

    /** Add all files that match @a pattern (relative to dir given in constructor). */
    size_t addFileByPattern( const string & pattern );

private:

    const string m_sDir;
    Sender & m_sender;

    uint64_t m_offset;

    struct FileInfo
    {
        string path;
        string name;
        uint64_t offset;
        uint64_t uncompressed;
        uint64_t compressed;
        uint32_t crc32;
        uint16_t msdos_time;
        uint16_t msdos_date;
        uint32_t stat_atime;
        uint32_t stat_mtime;
    };

    typedef std::vector< FileInfo > FileInfoVec;

    FileInfoVec m_fileInfo;

    void emit( CharBuffer & cb );
    void emit( string & s );

    void emitCopy( const string & s );

    void fillDateTime( FileInfo & fi );

    struct z_stream_s m_zs;
    bool m_bZStreamNeedsReset;
    void emitCompressedData( FileInfo & fi );

}; // end class Zip64Streamer

} // end namespace com::foiani

} // end namespace com

#endif // COM_FOIANI_Z64S_ZIP64STREAMER_HPP
