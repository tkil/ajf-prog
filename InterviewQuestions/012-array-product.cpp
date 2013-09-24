/*
 * Task:
 *
 *   There is an array A[N] of N numbers. You have to compose an array
 *   Output[N] such that Output[i] will be equal to multiplication of
 *   all the elements of A[N] except A[i]. For example Output[0] will
 *   be multiplication of A[1] to A[N-1] and Output[1] will be
 *   multiplication of A[0] and from A[2] to A[N-1]. Solve it without
 *   division operator and in O(n).
 *
 * Technique:
 *
 *   O(n^2) is easy.
 *
 *   O(n) forces some dynamic programming / memoization.  But 2n is
 *   still O(n), so maybe going forward in one pass, then backwards in
 *   another...
 */

#include <iosfwd>
#include <vector>

#include "test.hpp"

namespace
{

typedef std::vector< int > int_vec;

std::ostream &
operator << ( std::ostream & os, const int_vec & iv )
{
    if ( iv.empty() )
        return os << "[]";

    os << "[";
    for ( const int i : iv )
        os << " " << i;
    return os << " ]";
}

int_vec
get_excl_product( const int_vec & input )
{
    if ( input.empty() )
        return input;

    int_vec output( input.size(), 1 );

    int prod;

    // set output[n] to product of input 0 .. n-1
    prod = 1;
    for ( size_t i = 0; i < input.size(); ++i )
    {
        output[i] = prod;
        prod *= input[i];
    }

    // std::clog << "after fwd: output=" << output << std::endl;

    // multiply output[n] by product of input n+1 .. end
    prod = 1;
    for ( size_t i = input.size(); i > 0; --i )
    {
        output[i-1] *= prod;
        prod *= input[i-1];
    }

    // std::clog << "after rev: output=" << output << std::endl;

    return output;
}

}

int
main( int argc, char * argv [] )
{
    {
        const int_vec input{ 1, 2, 3, 4 };
        const int_vec received( get_excl_product( input ) );
        const int_vec expected{ 24, 12, 8, 6 };
        
        CHECK_EQ( received, expected );
    }

    return TEST_RESULT;
}
