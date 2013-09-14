#include "bit_counters.hpp"

int eight_bit_lookup( const uint16_vec & vals )
{
    int lookup[256];
    for ( uint16_t i = 0; i < 256; ++i )
        lookup[i] = count_bits_uint16( i );

    int rv = 0;
    for ( std::size_t i = 0; i < vals.size(); ++i )
        rv += ( lookup[ ( vals[i] >> 8 ) & 0xff ] +
                lookup[   vals[i]        & 0xff ] );
    return rv;
}
