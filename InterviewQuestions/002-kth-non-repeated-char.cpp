/*
 * Task:
 *
 *   Given a stream of characters, find the kth non-repeated character
 *   at any given time.
 *
 * Assumption: characters are single bytes.
 *
 * Technique:
 *
 *   Two 256-element arrays:
 *     occurances[char] = count
 *     order[k] = char
 */

#include <iostream>
#include <string>

class kth_non_repeated_char
{

public:

    kth_non_repeated_char()
        : seq( 0 )
    {
        for ( size_t i = 0; i < 256; ++i )
        {
            occurances[i] = 0;
            order[i] = 0;
        }
    }

    void add_chars( const std::string & s )
    {
        for ( char c : s )
        {
            if ( occurances[c] == 0 )
            {
                order[seq++] = c;
                occurances[c] = 1;
            }
            else if ( occurances[c] == 1 )
            {
                occurances[c] = 2;
            }
        }
    }

    int find_kth_non_repeated_char( const int k )
    {
        int non_repeated = 0;
        for ( size_t i = 0; i < seq; ++i )
            if ( occurances[ order[ i ] ] == 1 )
                if ( ++non_repeated == k )
                    return order[i];
        return -1;
    }

private:

    size_t seq;
    int occurances[ 256 ];
    int order[ 256 ];

};

int main( int argc, char * argv [] )
{
    int n_tests = 0;
    int n_failures = 0;

#define CHECK_EQ( x, y )                                        \
    do                                                          \
    {                                                           \
        ++n_tests;                                              \
        auto x_val = x;                                         \
        auto y_val = y;                                         \
        if ( x_val != y_val )                                   \
        {                                                       \
            std::clog << __LINE__ << ": " <<                    \
              "'" #x "'=" << x_val << " != "                    \
              "'" #y "'=" << y_val << std::endl;                \
            ++n_failures;                                       \
        }                                                       \
    } while ( 0 )

    kth_non_repeated_char knrc;

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), -1 );

    knrc.add_chars( "abcdef" );

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), 'a' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), 'b' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 3 ), 'c' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 4 ), 'd' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 5 ), 'e' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 6 ), 'f' );

    knrc.add_chars( "a" );

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), 'b' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), 'c' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 3 ), 'd' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 4 ), 'e' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 5 ), 'f' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 6 ), -1 );

    knrc.add_chars( "f" );

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), 'b' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), 'c' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 3 ), 'd' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 4 ), 'e' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 5 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 6 ), -1 );

    knrc.add_chars( "a" );

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), 'b' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), 'c' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 3 ), 'd' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 4 ), 'e' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 5 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 6 ), -1 );

    knrc.add_chars( "c" );

    CHECK_EQ( knrc.find_kth_non_repeated_char( 0 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 1 ), 'b' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 2 ), 'd' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 3 ), 'e' );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 4 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 5 ), -1 );
    CHECK_EQ( knrc.find_kth_non_repeated_char( 6 ), -1 );

    std::clog << n_tests << " tests, " << n_failures << " failures" << std::endl;

    return ( n_failures > 0 ? 1 : 0 );
}
