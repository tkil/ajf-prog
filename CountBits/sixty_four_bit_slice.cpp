#include "bit_counters.hpp"

int
sixty_four_bit_slice( const uint16_vec & vals )
{
    int rv = 0;
    std::size_t i = 0;

    const uint16_t * base = &vals[0];

    // do them individually until we're at a 64-bit boundary
    while ( i < vals.size() && ( reinterpret_cast< const intptr_t >( base + i ) & 0x07 ) )
        rv += count_bits_uint16( vals[i++] );

    // do the bulk as 64-bit values
    while ( i + 4 < vals.size() )
    {
        uint64_t n = * reinterpret_cast< const uint64_t * >( base + i );
        n = ( ( n & 0xaaaaaaaaaaaaaaaa ) >>  1 ) + ( n & 0x5555555555555555 );
        n = ( ( n & 0xcccccccccccccccc ) >>  2 ) + ( n & 0x3333333333333333 );
        n = ( ( n & 0xf0f0f0f0f0f0f0f0 ) >>  4 ) + ( n & 0x0f0f0f0f0f0f0f0f );
        n = ( ( n & 0xff00ff00ff00ff00 ) >>  8 ) + ( n & 0x00ff00ff00ff00ff );
        n = ( ( n & 0xffff0000ffff0000 ) >> 16 ) + ( n & 0x0000ffff0000ffff );
        n = ( ( n & 0xffffffff00000000 ) >> 32 ) + ( n & 0x00000000ffffffff );
        rv += n;
        i += 4;
    }

    // and then any at the end
    while ( i < vals.size()  )
        rv += count_bits_uint16( vals[i++] );

    return rv;
}
