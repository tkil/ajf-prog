/*
 * Task:
 *
 *   Given an array of 100 random integers.  Write an algorithm to find
 *   the the closest 2 integers (closest by position) in the array that
 *   add up to 100.
 *
 * Thoughts:
 *
 *   Obvious solution is O(n^2); for 100 values, that's more than good
 *   enough.
 *
 *   Fancier solutions can probably do it in O(n), but that's harder
 *   without specifying certain conditions on the values given.
 *
 * Corner cases:
 *
 *   duplicate values in array?
 *   negative values?
 *   integer addition overflow?
 *   more than one closest pair -- report first or last?
 *
 */

#include <map>
#include <vector>

#include "test.hpp"

namespace
{

typedef std::vector< int > int_vec;
typedef std::pair< size_t, size_t > index_pair;

std::ostream &
operator << ( std::ostream & os, const index_pair & ip )
{
    return os << "[" << ip.first << ", " << ip.second << "]";
}

index_pair
find_closest_sum_100( const int_vec & vec )
{
    if ( vec.size() < 2 )
        return index_pair( 0, 0 );

    size_t best_dist = vec.size();
    index_pair best( 0, 0 );

    for ( size_t i = 0; i + 1 < vec.size(); ++i )
        for ( size_t j = i + 1; j < vec.size(); ++j )
            if ( vec[i] + vec[j] == 100 )
                if ( j - i < best_dist )
                {
                    best_dist = j - i;
                    best.first = i;
                    best.second = j;
                }

    return best;
}

index_pair
find_closest_smarter( const int_vec & vec )
{
    typedef std::multimap< int, size_t > int_index_map;
    int_index_map comp_ints;

    size_t best_dist = vec.size();
    index_pair best( 0, 0 );

    for ( size_t i = 0; i < vec.size(); ++i )
    {
        int_index_map::iterator it( comp_ints.lower_bound( vec[i] ) );
        while ( it->first == vec[i] )
        {
            if ( i - it->second < best_dist )
            {
                best_dist = i - it->second;
                best.first = it->second;
                best.second = i;
            }
            ++it;
        }

        comp_ints.insert( { 100 - vec[i], i } );
    }

    return best;
}

}

int main( int, char * [] )
{
    {
        const int_vec test{ 1, 2, 3, 4, 5, 99, 95 };
        const index_pair result( find_closest_sum_100( test ) );
        CHECK_EQ( result, index_pair( 4, 6 ) );
        const index_pair result2( find_closest_smarter( test ) );
        CHECK_EQ( result2, index_pair( 4, 6 ) );
    }

    {
        const int_vec test{ 1, 2, 3, 4, 5, 99, 95, 1, 99 };
        const index_pair result( find_closest_sum_100( test ) );
        CHECK_EQ( result, index_pair( 7, 8 ) );
        const index_pair result2( find_closest_smarter( test ) );
        CHECK_EQ( result2, index_pair( 7, 8 ) );
    }

    {
        const int_vec test{ 1, 2, 3, 4, 5 };
        const index_pair result( find_closest_sum_100( test ) );
        CHECK_EQ( result, index_pair( 0, 0 ) );
        const index_pair result2( find_closest_smarter( test ) );
        CHECK_EQ( result2, index_pair( 0, 0 ) );
    }

    {
        const int_vec test{ -20, -10, 0, 110, 120 };
        const index_pair result( find_closest_sum_100( test ) );
        CHECK_EQ( result, index_pair( 1, 3 ) );
        const index_pair result2( find_closest_smarter( test ) );
        CHECK_EQ( result2, index_pair( 1, 3 ) );
    }

    return TEST_RESULT;
}
