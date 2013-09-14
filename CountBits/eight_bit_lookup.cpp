#include "bit_counters.hpp"

int eight_bit_lookup( const uint16_vec & vals )
{
    int lookup[256];
    for ( uint16_t i = 0; i < 256; ++i )
    {
        uint16_t n = i;
        n = ( ( n & 0xaaaa ) >> 1 ) + ( n & 0x5555 );
        n = ( ( n & 0xcccc ) >> 2 ) + ( n & 0x3333 );
        n = ( ( n & 0xf0f0 ) >> 4 ) + ( n & 0x0f0f );
        n = ( ( n & 0xff00 ) >> 8 ) + ( n & 0x00ff );
        lookup[i] = n;
    }

    int rv = 0;
    for ( std::size_t i = 0; i < vals.size(); ++i )
        rv += ( lookup[ ( vals[i] >> 8 ) & 0xff ] +
                lookup[   vals[i]        & 0xff ] );
    return rv;
}
