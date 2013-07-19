#ifndef COM_FOIANI_Z64S_COMPAT_HPP
#define COM_FOIANI_Z64S_COMPAT_HPP 1

/**
 * @file Compat.hpp
 *
 * These are spread out through my production database; for compiling
 * just the Zip64Streamer class and test, I've put them all in one
 * place.
 *
 * @author Anthony Foiani <anthony@foiani.com>
 */

// standard C / Unix headers
#include <string.h>

// standard C++ headers
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// note, these do more in the production code, this just lets it compile.
#define ERROR( x ) std::clog << "ERROR: "   << x << std::endl
#define WARN( x )  std::clog << "WARN:  "   << x << std::endl
#define DEBUG( x ) std::clog << "       "   << x << std::endl
#define FINE( x )  std::clog << "       > " << x << std::endl

#define EXTRA_DEBUGGING 0

namespace com
{

namespace /* com:: */ foiani
{

using std::string;

using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

typedef std::vector< char > CharBuffer;

typedef std::vector< string > StringList;

/** Zero out the memory contained in @a obj. */
template < typename T >
void
zeroStruct( T & obj )
{
    memset( &obj, 0, sizeof( T ) );
}

/** Generate an exception with @a label based on errno. */
struct OSError
    : public std::runtime_error
{
    OSError( const string & label );
};

/** Quote @a str for printing. */
string
QS( const string & str );

/** Return a list of all basenames in @a dir that match @a pattern. */
StringList globFiles( const string & dir, const string & pattern );

} // end namespace com::foiani

} // end namespace com

#endif // COM_FOIANI_Z64S_COMPAT_HPP
