#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "bit_counters.hpp"

namespace // anonymous
{

// quick and dirty version of boost::lexical_cast
template < typename T >
T
lexical_cast( const char * str )
{
    std::istringstream iss( str );
    T rv;
    iss >> rv;
    if ( ! iss && ! iss.eof() )
        throw std::invalid_argument( str );
    return rv;
}

typedef std::function< int ( const uint16_vec & ) > bit_counter_func;

typedef std::chrono::high_resolution_clock clock;
typedef clock::time_point instant;
typedef clock::duration duration;

int
time_loops( bit_counter_func func,
            const char * func_label,
            const uint16_vec & vec,
            const int sec )
{
    int reps = 0;
    int total = 0;
    int count = 0;

    instant start( clock::now() );
    duration elapsed;
    do
    {
        count = func( vec );
        total += count;
        ++reps;
        elapsed = clock::now() - start;
    }
    while ( elapsed < std::chrono::seconds( sec ) );

    auto us = std::chrono::duration_cast< std::chrono::microseconds >( elapsed ).count();
    std::cout << func_label
              << static_cast< double >( vec.size() ) * reps / us
              << " vals/µs" << std::endl;

    return count;
}

} // end namespace [anonymous]

int main( int argc, char * argv [] )
{
    size_t num_vals = 10000;
    int sec = 1;

    if ( argc > 1 )
        num_vals = lexical_cast< size_t >( argv[1] );

    if ( argc > 2 )
        sec = lexical_cast< int >( argv[2] );

    std::cout << "timing loops over " << num_vals << " values for " << sec << " seconds" << std::endl;

    // generate random values
    std::random_device rand_dev;
    std::mt19937 rand_gen( rand_dev() );
    std::uniform_int_distribution< uint16_t > rand_dist;

    std::vector< uint16_t > vec;
    vec.reserve( num_vals );
    for ( size_t i = 0; i < num_vals; ++i )
        vec.push_back( rand_dist( rand_gen ) );

    const int eight_bit_lookup_count     = time_loops( eight_bit_lookup,     "eight_bit_lookup:     ", vec, sec );
    const int thirty_two_bit_slice_count = time_loops( thirty_two_bit_slice, "thirty_two_bit_slice: ", vec, sec );
    const int sixty_four_bit_slice_count = time_loops( sixty_four_bit_slice, "sixty_four_bit_slice: ", vec, sec );

    if ( eight_bit_lookup_count != sixty_four_bit_slice_count )
        std::clog <<
          "8blc=" << eight_bit_lookup_count << " != "
          "64bsc=" << sixty_four_bit_slice_count << std::endl;

    if ( eight_bit_lookup_count != thirty_two_bit_slice_count )
        std::clog <<
          "8blc=" << eight_bit_lookup_count << " != "
          "32bsc=" << thirty_two_bit_slice_count << std::endl;

    return 0;
}
