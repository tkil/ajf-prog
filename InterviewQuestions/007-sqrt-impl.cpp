/*
 * Task:
 *   Implement a square root function.
 *
 * Notes:
 *   Use Newton's method.
 *
 */

#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace
{

double
my_sqrt( const double x )
{
    if ( x < 0 )
        throw std::domain_error( "real-valued double can't represent sqrt of negative value" );

    // and our tolerance
    const double epsilon = 0.00001;

    // newton's method:
    // f(r) =  x - r * r;
    // f'(r) = - 2 * r;
    // r_i = r_{i-1} - f( r_{i-1} ) / f'( r_{i-1} );

    // our initial guesss
    double r = x/2;
    double diff =  x - r * r;
    do
    {
        double r1 = r;
        // r = r1 - ( x - r1 * r1 ) / ( -2 * r1 );
        r = r1 - diff / ( -2 * r1 );
        std::clog << "r=" << r << ", r1=" << r1 << std::endl;
        diff = x - r * r;
    }
    while ( diff < -epsilon || epsilon < diff );

    return r;
}

}

int
main( int argc, char * argv [] )
{
    double x;
    if ( argc > 1 )
    {
        std::istringstream iss( argv[1] );
        iss >> x;
        if ( ! iss && ! iss.eof() )
        {
            std::clog << argv[0] << ": could not parse '" << argv[1] << "' as float" << std::endl;
            return 1;
        }
    }
    else
    {
        x = drand48();
        if ( x < 0 )
            x = -x;
    }

    const double s = my_sqrt( x );

    std::cout << "my_sqrt( " << x << " ) = " << s << std::endl;

    return 0;
}
