#include <string>
#include <map>
#include <sstream>
#include <vector>

#include "test.hpp"

namespace
{

const std::string input =
  "Alpha"   "\t" "Bravo"   "\t" "DOB-A" "\n"
  "Bravo"   "\t" "Charlie" "\t" "DOB-B" "\n"
  "Charlie" "\t" "Charlie" "\t" "DOB-C" "\n"
  "Delta"   "\t" "Charlie" "\t" "DOB-D" "\n";

const std::string output =
  "Charlie                 DOB-C\n"
  "  Bravo                 DOB-B\n"
  "    Alpha               DOB-A\n"
  "  Delta                 DOB-D\n";

struct emp;

typedef std::vector< emp * > emp_ptr_vec;

struct emp
{
    emp( const std::string & name )
        : name( name ) {}

    const std::string name;
    std::string dob;
    emp_ptr_vec subs;
};

emp_ptr_vec
find_trees( std::istream & is )
{
    emp_ptr_vec rv;

    typedef std::map< std::string, emp * > name_emp_ptr_map;
    name_emp_ptr_map by_name;

    while ( true )
    {
        std::string name, mgr_name, dob;
        is >> name >> mgr_name >> dob;
        if ( ! is )
            break;

        emp * curr;
        name_emp_ptr_map::iterator it = by_name.find( name );
        if ( it != by_name.end() )
            curr = it->second;
        else
            curr = new emp( name );
        curr->dob = dob;

        if ( name == mgr_name )
        {
            rv.push_back( curr );
            continue;
        }

        emp * mgr;
        it = by_name.find( mgr_name );
        if ( it != by_name.end() )
        {
            mgr = it->second;
        }
        else
        {
            mgr = new emp( mgr_name );
            by_name.insert( { mgr_name, mgr } );
        }
        mgr->subs.push_back( curr );
    }

    return rv;
}

void
pad( std::ostream & os, const int num_spaces )
{
    for ( int i = 0; i < num_spaces; ++i )
        os << ' ';
}

void
emit_tree( std::ostream & os, const emp * curr, int level = 0 )
{
    pad( os, 2 * level );
    os << curr->name;
    pad( os,  24 - ( curr->name.length() + 2 * level ) );
    os << curr->dob << "\n";
    for ( emp * sub : curr->subs )
        emit_tree( os, sub, level+1 );
}

void
emit_trees( std::ostream & os, const emp_ptr_vec & trees )
{
    for ( emp * curr : trees )
        emit_tree( os, curr, 0 );
}

}

int
main( int /* argc */ , char * /* argv */ [] )
{
    std::istringstream iss( input );
    std::ostringstream oss;

    emp_ptr_vec trees = find_trees( iss );
    emit_trees( oss, trees );

    CHECK_EQ( oss.str(), output );

    std::cout <<
      "----- input -----\n" <<
      input <<
      "----- expected -----\n" <<
      output <<
      "----- generated -----\n" <<
      oss.str();

    return 0;
}
