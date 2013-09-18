/*
 * Task:
 *
 *   Given a grid of white (".") or black ("#") cells, find the largest
 *   rook-wise connected set of black cells.
 *
 * Technique:
 *
 *   1. Scan in x-then-y order, find set but not-visited black cells,
 *      then do depth-first search to find extent of connected cells
 *
 *   2. Scan in x-then-y order.  If we encounter a black cell:
 *      a. up and north are visited: use max, add map from min to max.
 *      b. only up, only north: use same value here
 *      c. neither: allocate new group number.
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "test.hpp"

namespace
{

int verbose;

// ---------------------------------------------------------------------

struct coord {
    int x, y;
    coord up    () const { return { x,   y-1 }; }
    coord right () const { return { x+1, y   }; }
    coord down  () const { return { x,   y+1 }; }
    coord left  () const { return { x-1, y   }; }
};

bool
operator < ( const coord & lhs, const coord & rhs )
{
    return ( lhs.x < rhs.x || ( lhs.x == rhs.x &&
                                lhs.y <  rhs.y ) );
}

bool
operator == ( const coord & lhs, const coord & rhs )
{
    return ( lhs.y == rhs.y && lhs.x == rhs.x );
}

std::ostream &
operator << ( std::ostream & os, const coord & c )
{
    return os << "(" << c.x << "," << c.y << ")";
}

typedef std::vector< coord > coord_vec;

std::ostream &
operator << ( std::ostream & os, const coord_vec & cv )
{
    if ( cv.empty() )
        return os << "[]";

    os << "[";
    for ( size_t i = 0; i < cv.size(); ++i )
        os << " " << cv[i];
    os << " ]";

    return os;
}

// ---------------------------------------------------------------------

const char grid_chars[] = ".#abcdefghijklmnopqrstuvwxyz";
const int grid_chars_end = sizeof( grid_chars );

struct grid_from_text
{
    grid_from_text( const char * m );

    int x;
    int y;
    std::vector< int > v;
};

grid_from_text::grid_from_text( const char * m )
    : x( 0 ), y( 0 )
{
    const char * p = m;
    const char * bol = m;
    while ( true )
    {
        if ( *p == '\n' || *p == '\0' )
        {
            const int cols = p - bol;
            if ( x == 0 )
                x = cols;
            else if ( cols != x )
                throw std::invalid_argument( "inconsistent number of columns" );
            ++y;
            bol = p+1;
            if ( *p == '\0' )
                break;
        }
        else
        {
            bool found = false;
            for ( const char * gc = grid_chars; *gc && !found; ++gc )
            {
                if ( *p == *gc )
                {
                    v.push_back( gc - grid_chars );
                    found = true;
                }
            }
            if ( !found )
                v.push_back( grid_chars_end );
        }
        ++p;
    }

    // std::cout << "m=" << m << ", x=" << x << ", y=" << y << std::endl;

}

class grid
{

public:

    grid( const int x, const int y );
    grid( const grid_from_text & gft );

    friend std::ostream & operator << ( std::ostream & os, const grid & g );

    const int x;
    const int y;

    int operator()( const int c, const int r ) const;
    int operator()( const coord & c ) const;

    int & operator()( const int c, const int r );
    int & operator()( const coord & c );

private:

    std::vector< int > v;

};

grid::grid( const int x, const int y )
    : x( x ), y( y ), v( x * y, 0 )
{
}

grid::grid( const grid_from_text & gft )
    : x( gft.x ), y( gft.y ), v( gft.v )
{
}

int
grid::operator()( const int c, const int r ) const
{
    return v[ c + r * x ];
}

int
grid::operator()( const coord & c ) const
{
    return v[ c.x + c.y * x ];
}

int &
grid::operator()( const int c, const int r )
{
    return v[ c + r * x ];
}

int &
grid::operator()( const coord & c )
{
    return v[ c.x + c.y * x ];
}

std::ostream &
operator << ( std::ostream & os, const grid & g )
{
    const char * sep = "";
    for ( int r = 0; r < g.y; ++r )
    {
        os << sep;
        for ( int c = 0; c < g.x; ++c )
        {
            const int val( g( c, r ) );
            os << ( val < grid_chars_end ? grid_chars[val] : '*' );
        }
        sep = "\n";
    }
    return os;
}

// ---------------------------------------------------------------------

int
find_largest_connected( const grid & g, coord_vec & coords )
{
    grid v( g.x, g.y );

    if ( verbose )
        std::clog << "g=\n" << g << std::endl;

    int next_set = 1;

    int largest_size = 0;
    coord_vec largest_coords;

    for ( int row = 0; row < g.y; ++row )
    {
        for ( int col = 0; col < g.x; ++col )
        {
            const coord s{ col, row };
            if ( verbose > 1 )
                std::clog << s << std::endl;

            // empty space, ignore
            if ( g( s ) == 0 )
                continue;

            // not empty, but already visited / accounted for
            if ( v( s ) > 0 )
                continue;

            int curr_size = 0;
            coord_vec curr_coords;
            ++next_set;

            coord_vec curr_queue;
            curr_queue.push_back( s );

            while ( ! curr_queue.empty() )
            {
                coord c = curr_queue.back();
                if ( verbose > 1 )
                    std::clog << "c=" << c << std::endl;
                curr_queue.pop_back();

                curr_coords.push_back( c );

                v( c ) = next_set;
                ++curr_size;

                coord_vec neighbors;
                if ( c.x     > 0   ) neighbors.push_back( c.left  () );
                if ( c.x + 1 < g.x ) neighbors.push_back( c.right () );
                if ( c.y     > 0   ) neighbors.push_back( c.up    () );
                if ( c.y + 1 < g.y ) neighbors.push_back( c.down  () );

                for ( const coord & n : neighbors )
                {
                    if ( g( n ) && ! v( n ) )
                    {
                        v( n ) = next_set;
                        curr_queue.push_back( n );
                    }
                }
            }

            if ( curr_size > largest_size )
            {
                largest_size = curr_size;
                largest_coords.swap( curr_coords );
            }

            if ( verbose )
                std::clog <<
                  "largest=" << largest_size << ", "
                  "v=\n" << v << std::endl;

        }
    }

    coords.swap( largest_coords );
    return largest_size;
}

}

// =====================================================================

#define TEST( GRID, EXP_COORDS )                                        \
    do                                                                  \
    {                                                                   \
        const grid g( GRID );                                           \
        std::ostringstream oss;                                         \
        oss << g;                                                       \
        CHECK_CSTR_EQ( oss.str().c_str(), GRID );                       \
        const coord_vec exp_coords( EXP_COORDS );                       \
        const int exp_size( exp_coords.size() );                        \
        coord_vec got_coords;                                           \
        const int got_size = find_largest_connected( g, got_coords );   \
        std::sort( got_coords.begin(), got_coords.end() );              \
        CHECK_EQ( got_size, exp_size );                                 \
        CHECK_EQ( got_coords, exp_coords );                             \
    }                                                                   \
    while ( 0 )

int
main( int argc, char * argv [] )
{
    verbose = 0;
    for ( int i = 1; i < argc; ++i )
    {
        const std::string arg( argv[i] );
        if ( arg == "-v" )
            ++verbose;
    }

    TEST( "", {} );

    {
        coord_vec expected{ { 0, 0 } };
        TEST( "#", expected );
    }

    TEST( ".", {} );
    TEST( "..", {} );
    TEST( "..\n..", {} );

    {
        coord_vec expected{
            { 0, 0 },
            { 1, 0 }
        };
        TEST( "##", expected );
    }

    {
        coord_vec expected{
            { 0, 0 },
            { 0, 1 },
            { 1, 0 },
            { 1, 1 }
        };
        TEST( "##\n"
              "##", expected );
    }

    {
        coord_vec expected{
            { 0, 1 },
            { 1, 0 },
            { 1, 1 }
        };
        TEST( ".#\n"
              "##", expected );
    }

    {
        coord_vec expected{
            { 3, 0 },
            { 3, 1 },
            { 4, 0 },
            { 4, 1 },
            { 5, 0 },
            { 5, 1 },
        };
        TEST( "##.###\n"
              "##.###", expected );
    }

    {
        coord_vec expected{
            { 0, 0 },
            { 0, 1 },
            { 0, 2 },
            { 1, 0 },
            { 1, 2 },
            { 2, 0 },
            { 2, 1 },
            { 2, 2 },
        };
        TEST( "###\n"
              "#.#\n"
              "###", expected );
    }

    {
        coord_vec expected{
            { 4, 0 },
            { 4, 1 },
            { 4, 2 },
            { 5, 0 },
            { 5, 1 },
            { 5, 2 },
            { 6, 0 },
            { 6, 1 },
            { 6, 2 },
        };
        TEST( "###.###\n"
              "###.###\n"
              "##..###", expected );
    }

    {
        coord_vec expected{
            { 0, 0 },
            { 1, 0 },
            { 2, 0 },
            { 3, 0 },
            { 4, 0 },
            { 5, 0 },
            { 6, 0 },

            { 6, 1 },
            { 6, 2 },
            { 6, 3 },
            { 6, 4 },

            { 5, 4 },
            { 4, 4 },
            { 3, 4 },
            { 2, 4 },
            { 1, 4 },
            { 0, 4 },

            { 0, 3 },
            { 0, 2 },

            { 1, 2 },
            { 2, 2 },
            { 3, 2 },
            { 4, 2 },
        };
        std::sort( expected.begin(), expected.end() );
        TEST( "#######\n"
              "......#\n"
              "#####.#\n"
              "#.....#\n"
              "#######", expected );
    }

    return 0;
}
