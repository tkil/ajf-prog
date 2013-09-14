#include <cstdint>
#include <vector>

namespace
{

inline
int
count_bits_uint16( uint16_t n )
{
    n = ( ( n & 0xaaaa ) >> 1 ) + ( n & 0x5555 );
    n = ( ( n & 0xcccc ) >> 2 ) + ( n & 0x3333 );
    n = ( ( n & 0xf0f0 ) >> 4 ) + ( n & 0x0f0f );
    n = ( ( n & 0xff00 ) >> 8 ) + ( n & 0x00ff );
    return n;
}

inline
int
count_bits_uint8( uint8_t n )
{
    n = ( ( n & 0xaa ) >> 1 ) + ( n & 0x55 );
    n = ( ( n & 0xcc ) >> 2 ) + ( n & 0x33 );
    n = ( ( n & 0xf0 ) >> 4 ) + ( n & 0x0f );
    return n;
}

}

typedef std::vector< uint16_t > uint16_vec;

int eight_bit_lookup( const uint16_vec & vals );
int sixty_four_bit_slice( const uint16_vec & vals );
int thirty_two_bit_slice( const uint16_vec & vals );

