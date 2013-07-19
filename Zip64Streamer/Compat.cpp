/**
 * @file Compat.cpp
 *
 * These are spread out through my production database; for compiling
 * just the Zip64Streamer class and test, I've put them all in one
 * place.
 *
 * @author Anthony Foiani <anthony@foiani.com>
 */

// standard C / Unix headers
#include <glob.h>
#include <errno.h>
#include <string.h>

// standard C++ headers
#include <sstream>

// interface
#include "Compat.hpp"

namespace // anonymous
{

using std::string;

string
genOSErrorString( const string & text )
{
    std::ostringstream s;

    s << text << ": error " << errno;

    const int maxBufLen = 256;
    char buf[ maxBufLen ];
    s << " (" << strerror_r( errno, buf, maxBufLen ) << ")";

    return s.str();
}

const char *
quoteChar( const char c )
{
    static const char * quoted[256] = {
        "\\x00", "\\x01", "\\x02", "\\x03", "\\x04", "\\x05", "\\x06", "\\a",
        "\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "\\x0e", "\\x0f",
        "\\x10", "\\x11", "\\x12", "\\x13", "\\x14", "\\x15", "\\x16", "\\x17",
        "\\x18", "\\x19", "\\x1a", "\\x1b", "\\x1c", "\\x1d", "\\x1e", "\\x1f",
        " ",     "!",     "\\\"",  "#",     "$",     "%",     "&",     "\\'",
        "(",     ")",     "*",     "+",     ",",     "-",     ".",     "/",
        "0",     "1",     "2",     "3",     "4",     "5",     "6",     "7",
        "8",     "9",     ":",     ";",     "<",     "=",     ">",     "?",
        "@",     "A",     "B",     "C",     "D",     "E",     "F",     "G",
        "H",     "I",     "J",     "K",     "L",     "M",     "N",     "O",
        "P",     "Q",     "R",     "S",     "T",     "U",     "V",     "W",
        "X",     "Y",     "Z",     "[",     "\\\\",  "]",     "^",     "_",
        "`",     "a",     "b",     "c",     "d",     "e",     "f",     "g",
        "h",     "i",     "j",     "k",     "l",     "m",     "n",     "o",
        "p",     "q",     "r",     "s",     "t",     "u",     "v",     "w",
        "x",     "y",     "z",     "{",     "|",     "}",     "~",     "\\x7f",
        "\\x80", "\\x81", "\\x82", "\\x83", "\\x84", "\\x85", "\\x86", "\\x87",
        "\\x88", "\\x89", "\\x8a", "\\x8b", "\\x8c", "\\x8d", "\\x8e", "\\x8f",
        "\\x90", "\\x91", "\\x92", "\\x93", "\\x94", "\\x95", "\\x96", "\\x97",
        "\\x98", "\\x99", "\\x9a", "\\x9b", "\\x9c", "\\x9d", "\\x9e", "\\x9f",
        "\\xa0", "\\xa1", "\\xa2", "\\xa3", "\\xa4", "\\xa5", "\\xa6", "\\xa7",
        "\\xa8", "\\xa9", "\\xaa", "\\xab", "\\xac", "\\xad", "\\xae", "\\xaf",
        "\\xb0", "\\xb1", "\\xb2", "\\xb3", "\\xb4", "\\xb5", "\\xb6", "\\xb7",
        "\\xb8", "\\xb9", "\\xba", "\\xbb", "\\xbc", "\\xbd", "\\xbe", "\\xbf",
        "\\xc0", "\\xc1", "\\xc2", "\\xc3", "\\xc4", "\\xc5", "\\xc6", "\\xc7",
        "\\xc8", "\\xc9", "\\xca", "\\xcb", "\\xcc", "\\xcd", "\\xce", "\\xcf",
        "\\xd0", "\\xd1", "\\xd2", "\\xd3", "\\xd4", "\\xd5", "\\xd6", "\\xd7",
        "\\xd8", "\\xd9", "\\xda", "\\xdb", "\\xdc", "\\xdd", "\\xde", "\\xdf",
        "\\xe0", "\\xe1", "\\xe2", "\\xe3", "\\xe4", "\\xe5", "\\xe6", "\\xe7",
        "\\xe8", "\\xe9", "\\xea", "\\xeb", "\\xec", "\\xed", "\\xee", "\\xef",
        "\\xf0", "\\xf1", "\\xf2", "\\xf3", "\\xf4", "\\xf5", "\\xf6", "\\xf7",
        "\\xf8", "\\xf9", "\\xfa", "\\xfb", "\\xfc", "\\xfd", "\\xfe", "\\xff"
    };

    return quoted[ static_cast< size_t >( c ) ];
}

bool startsWith( const string & str,
                 const string & prefix )
{
    if ( prefix.length() > str.length() )
        return false;
    if ( prefix.empty() )
        return true;

    typedef std::pair< string::const_iterator,
                       string::const_iterator > match_pair;

    // this forces a search for equality at position 0, and should
    // be faster than either an unconstrained "find" or a
    // substring-then-compare.
    match_pair mp( std::mismatch( prefix.begin(), prefix.end(),
                                  str.begin() ) );

    return ( mp.first == prefix.end() );
}

bool endsWith( const string & str,
               const string & suffix )
{
    if ( suffix.length() > str.length() )
        return false;
    if ( suffix.empty() )
        return true;

    const size_t start( str.length() - suffix.length() );
    size_t pos = str.find( suffix, start );

    return pos == start;
}

} // end namespace [anonymous]

namespace com
{

namespace /* com:: */ foiani
{

OSError::OSError( const string & text )
    : std::runtime_error( genOSErrorString( text ) )
{
}

string
QS( const string & str )
{
    string rv;
    rv.reserve( str.length() );

    for ( const char & c : str )
        rv.append( quoteChar( c ) );

    return rv;
}

StringList
globFiles( const string & dir, const string & pattern )
{
    DEBUG( "gf: dir=" << QS( dir ) << ", pattern=" << QS( pattern ) );

    const string fullPath( dir + "/" + pattern );

    glob_t globBuf;
    const int rc = glob( fullPath.c_str(),
                         /* flags = */ GLOB_MARK,
                         /* errfunc = */ 0,
                         &globBuf );
    FINE( "gf: rc=" << rc );

    StringList rv;

    string err;
    switch ( rc )
    {
    case 0:            break;
    case GLOB_NOSPACE: err = "out of memory"; break;
    case GLOB_ABORTED: err = "read error"; break;
    case GLOB_NOMATCH: FINE( "gf: no matches" ); return rv;
    default:           err = "unknown error " + std::to_string( rc ); break;
    }

    if ( ! err.empty() )
    {
        ERROR( "gf: error=" << QS( err ) );
        throw std::runtime_error( "glob: " + err );
    }

    for ( size_t i = 0; i < globBuf.gl_pathc; ++i )
    {
        const string s( globBuf.gl_pathv[i] );
        if ( endsWith( s, "/" ) ) // ignore directories
            continue;
        else if ( ! startsWith( s, dir ) ) // ignore outside parent dir
            continue;
        else
        {
            const string basename( s.substr( dir.size() + 1 ) );
            FINE( "gf:   match: " << QS( basename ) );
            rv.push_back( basename );
        }
    }

    return rv;
}

} // end namespace com::foiani

} // end namespace com
