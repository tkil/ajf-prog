/*
 * Task:
 *
 *   You have a ladder n-steps in height.  You can either take one
 *   step or two steps up the ladder at a time.  How can you find out
 *   all the different combinations up the ladder?  Then figure out an
 *   algorithm that will actually print out all the different ways up
 *   the ladder.  ie: 1,1,2,1,2,2... etc...
 *
 * Notes:
 *
 *   Formally, this is finding all partitions of 'n' such that the
 *   partition sizes are 1 or 2.
 *
 *   A recursive solution should be straightforward.  Let N(n) be
 *   the number of paths possible on a ladder with 'n' rungs.
 *
 *     N(0) = 0
 *     N(1) = 1   1
 *     N(2) = 2   11 2
 *     N(3) = 3   111 12 21
 *     N(4) = 5   1111 112 121 211 22
 *     N(5) = 8   11111 1112 1121 1211 2111 122 212 221
 *
 *   So it's actually the fibonacci series (modify N(0) to be 1,
 *   meaning the empty string), which makes sense:
 *
 *     N(n) = N(n-1) // start with step of 1
 *          + N(n-2) // start with step of 2
 *
 *   We can generate the set of paths recursively as well.  Let L(n)
 *   be the set of all routes expressed as strings of "1" or "2".
 *
 *     L(0) = { "" };
 *     L(1) = { "1" };
 *     
 *   So for n>1, we have:
 *
 *     L(n) = { "1" + L(n-1), "2" + L(n-2) }
 *
 *   Where "+" is really something like a "map" operation.
 *
 *   To generate them iteratively... 
 *
 *   start with all 1s: 11111
 *
 *   what transformation will take such a string, and convert it to
 *   the lexicographically next value?
 *
 *   look at which rungs are skipped?
 *
 *           01234
 *     11111  0000
 *     1112   0001
 *     1121   0010
 *     1211   0100
 *     122    0101
 *     2111   1000
 *     212    1001
 *     221    1010
 *
 *   Uhg.  it's almost like counting with carry, but not quite.
 *
 *
 */

#include <iostream>
#include <set>
#include <string>

namespace
{

typedef std::set< std::string > string_set;

void iter( const int n, string_set & paths )
{
    paths.clear();
    if ( n == 0 )
        return;

    paths.insert( "1" );
    if ( n == 1 )
        return;

    string_set prev;
    prev.swap( paths );
    paths.insert( "11" );
    paths.insert( "2" );
    if ( n == 2 )
        return;

    for ( int i = 2; i < n; ++i )
    {
        string_set tmp;
        for ( const std::string & s : prev )
            tmp.insert( s + "2" );
        for ( const std::string & s : paths )
            tmp.insert( s + "1" );
        prev.swap( paths );
        paths.swap( tmp );
    }

    return;
}

void dump( const std::string & label,
           const string_set & paths )
{
    std::cout << label;
    for ( const std::string & s : paths )
        std::cout << " " << s;
    std::cout << std::endl;
}

}

int main( int argc, char * argv [] )
{
    int n = 5;
    if ( argc == 2 )
        n = std::stoi( argv[1] );

    string_set paths;

    iter( n, paths );
    dump( "iter:", paths );

    string_set bin_paths;
    for ( const std::string & path : paths )
    {
        std::string bin_path;
        for ( const char c : path )
            if ( c == '1' ) bin_path.append(  "0" );
            else            bin_path.append( "10" );
        bin_paths.insert( bin_path );
    }
    dump( "bin:  ", bin_paths );

    return 0;
}
