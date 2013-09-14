#include "bit_counters.hpp"

int
thirty_two_bit_slice( const uint16_vec & vals )
{
    int rv = 0;
    std::size_t i = 0;

    const uint16_t * base = &vals[0];

    // do them individually until we're at a 64-bit boundary
    while ( i < vals.size() && ( reinterpret_cast< const intptr_t >( base + i ) & 0x03 ) )
        rv += count_bits_uint16( vals[i++] );

    // do the bulk as 32-bit values
    while ( i + 2 < vals.size() )
    {
        uint32_t n = * reinterpret_cast< const uint32_t * >( base + i );
        n = ( ( n & 0xaaaaaaaa ) >>  1 ) + ( n & 0x55555555 );
        n = ( ( n & 0xcccccccc ) >>  2 ) + ( n & 0x33333333 );
        n = ( ( n & 0xf0f0f0f0 ) >>  4 ) + ( n & 0x0f0f0f0f );
        n = ( ( n & 0xff00ff00 ) >>  8 ) + ( n & 0x00ff00ff );
        n = ( ( n & 0xffff0000 ) >> 16 ) + ( n & 0x0000ffff );
        rv += n;
        i += 2;
    }

    // and then any at the end
    while ( i < vals.size()  )
        rv += count_bits_uint16( vals[i++] );

    return rv;
}
