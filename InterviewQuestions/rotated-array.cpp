/*
 * Task:
 *
 *   Given an array, which is sorted but rotated. Find an element
 *   efficiently in this array.
 *
 * Technique:
 *
 *   Use a modified binary search.
 */

#include <algorithm>
#include <iostream>
#include <vector>

namespace
{

typedef std::vector< int > int_vec;
typedef std::vector< int >::iterator int_vec_iter;

template < typename RandomIter >
RandomIter
find_rotated( RandomIter begin,
              RandomIter end,
              const typename std::iterator_traits< RandomIter >::value_type & val )
{
    const RandomIter orig_begin( begin );
    const RandomIter orig_end( end );
    while ( end - begin > 10 )
    {
        if ( *begin == val )
            return begin;

        const RandomIter mid = begin + ( end - begin ) / 2;

        if ( *begin < *mid ) 
        {
            // no rotation within this range
            if ( *begin <= val && val < *mid ) end   = mid;
            else                               begin = mid;
        }
        else 
        {
            // rotation in front half
            if ( *begin <= val || val < *mid ) end   = mid;
            else                               begin = mid;
        }
    }

    while ( begin != end )
    {
        if ( *begin == val )
            return begin;
        ++begin;
    }

    return orig_end;
}

}

int main( int argc, char * argv [] )
{
    const size_t num_ints = 100;

    // fill vector with lots of even numbers
    int_vec vec;
    vec.reserve( num_ints );
    for ( int i = 0; i < num_ints; ++i )
        vec.push_back( i * 2 );

    int n_tests = 0;
    int n_failures = 0;

#define CHECK_ITER_EQ( x, y )                                   \
    do                                                          \
    {                                                           \
        ++n_tests;                                              \
        auto x_val = ( x ) - vec.begin();                       \
        auto y_val = ( y ) - vec.begin();                       \
        if ( x_val != y_val )                                   \
        {                                                       \
            std::clog << __LINE__ << ": " <<                    \
              "'" #x "'=" << x_val << " != "                    \
              "'" #y "'=" << y_val << std::endl;                \
            ++n_failures;                                       \
        }                                                       \
    } while ( 0 )
    
    // do simple tests on straightforward array

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 0 ),
                   vec.begin() );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), ( num_ints - 1 ) * 2 ),
                   vec.end() - 1 );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), num_ints ),
                   vec.begin() + num_ints / 2 );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(),   -1 ), vec.end() );
    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 2001 ), vec.end() );
    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 4001 ), vec.end() );

    // now rotate the vector
    const size_t third = vec.size() / 3;
    std::rotate( vec.begin(), vec.begin() + third, vec.end() );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 0 ),
                   vec.end() - third );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), ( num_ints - 1 ) * 2 ),
                   vec.end() - third - 1 );

    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), num_ints ),
                   vec.begin() + num_ints / 2 - third );
    
    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(),   -1 ), vec.end() );
    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 2001 ), vec.end() );
    CHECK_ITER_EQ( find_rotated( vec.begin(), vec.end(), 4001 ), vec.end() );

    std::clog << n_tests << " tests, " << n_failures << " failures" << std::endl;

    return ( n_failures > 0 ? 1 : 0 );
}
