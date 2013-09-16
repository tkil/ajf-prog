#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

namespace
{

typedef std::vector< int > int_vec;

struct sub_array
{
    sub_array() : begin( 0 ), end( 0 ), sum( 0 ) {}

    size_t begin;
    size_t end;
    int sum;
};

std::ostream &
operator << ( std::ostream & os, const sub_array & sa )
{
    return os << sa.begin << "..." << sa.end << "=" << sa.sum;
}

sub_array
whole_array( const int_vec & v )
{
    sub_array rv;
    rv.begin = 0;
    rv.end = v.size();
    for ( const int i : v )
        rv.sum += i;

    return rv;
}

sub_array
simple_cubic( const int_vec & v )
{
    sub_array rv;

    for ( size_t begin = 0; begin+1 < v.size(); ++begin )
    {
        for ( size_t end = begin+1; end < v.size()+1; ++end )
        {
            int sum = 0;
            for ( size_t i = begin; i < end; ++i )
                sum += v[i];
            if ( sum > rv.sum )
            {
                rv.begin = begin;
                rv.end = end;
                rv.sum = sum;
            }
        }
    }

    return rv;
}

sub_array
simple_quadratic( const int_vec & v )
{
    int_vec t;
    t.resize( v.size() + 1 );
    t[0] = 0;
    for ( size_t i = 0; i < v.size(); ++i )
        t[i+1] = t[i] + v[i];

    sub_array rv;

    for ( size_t begin = 0; begin+1 < v.size(); ++begin )
    {
        for ( size_t end = begin+1; end < v.size()+1; ++end )
        {
            const int sum = t[end] - t[begin];
            if ( sum > rv.sum )
            {
                rv.begin = begin;
                rv.end = end;
                rv.sum = sum;
            }
        }
    }

    return rv;
}

sub_array
smart_linear( const int_vec & v )
{
    sub_array best;
    sub_array latest;

    for ( size_t i = 0; i < v.size(); ++i )
    {
        const int new_sum = latest.sum + v[i];
        if ( new_sum >= 0 )
        {
            latest.sum = new_sum;
        }
        else if ( v[i] > 0 )
        {
            latest.begin = i;
            latest.sum = v[i];
        }
        else
        {
            latest.begin = i+1;
            latest.sum = 0;
        }

        latest.end = i+1;

        if ( latest.sum > best.sum )
            best = latest;

        // std::cout << "  latest=" << latest << std::endl;
    }

    if ( latest.sum > best.sum )
        return latest;
    else
        return best;
}

}

int main( int argc, char * argv [] )
{
    size_t n_elts = 100;
    if ( argc > 1 )
    {
        std::istringstream iss( argv[1] );
        iss >> n_elts;
    }

    std::cout << "n_elts=" << n_elts << std::endl;

    boost::mt19937 rng;
    struct timeval tv;
    gettimeofday( &tv, 0 );
    rng.seed( static_cast< unsigned int >( tv.tv_sec  ) ^
              static_cast< unsigned int >( tv.tv_usec ) );

    boost::uniform_int<> int_dist( -1000, 1000 );
    boost::variate_generator< boost::mt19937 &, boost::uniform_int<> >
      int_gen( rng, int_dist );

    int_vec v;
    v.reserve( n_elts );
    for ( size_t i = 0; i < n_elts; ++i )
        v.push_back( int_gen() );

    // std::cout << "v=[";
    // for ( const int i : v )
    //     std::cout << " " << i;
    // std::cout << " ]" << std::endl;

    {
        const sub_array result = whole_array( v );
        std::cout << "whole_array:      " << result << std::endl;
    }

    {
        const sub_array result = simple_cubic( v );
        std::cout << "simple_cubic:     " << result << std::endl;
    }

    {
        const sub_array result = simple_quadratic( v );
        std::cout << "simple_quadratic: " << result << std::endl;
    }

    {
        const sub_array result = smart_linear( v );
        std::cout << "smart_linear:     " << result << std::endl;
    }

    return 0;
}
