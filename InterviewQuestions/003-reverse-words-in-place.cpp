// sample input:  "Today is a beautiful day"
// sample output: "day beautiful a is Today"

#include <string.h>

#include <iostream>
#include <utility>

#include "test.hpp"

namespace
{

void reverse_chars( char * begin, char * end )
{
    while ( end - begin > 1 )
        std::swap( *begin++, *--end );
}

}

void reverse_words(char* sentence)
{
    char * sentence_begin = sentence;
    char * sentence_end   = sentence;
    
    // find end of string
    while ( *sentence_end != '\0' )
        ++sentence_end;
    
    char * begin = sentence_begin;
    char * end = sentence_end;
    // reverse all characters
    reverse_chars( begin, end );

    // now reverse each word
    begin = sentence_begin;
    end = begin;
    while ( end < sentence_end )
    {
        end = begin;
        while ( end < sentence_end && *end != ' ' )
            ++end;
        reverse_chars( begin, end );
        while ( end < sentence_end && *end == ' ' )
            ++end;
        begin = end;
    }
}

int main( int argc, char * argv [] )
{

    // test the requested input/output
    char test1[] = "Today is a beautiful day";
    reverse_words( test1 );
    CHECK_CSTR_EQ( test1, "day beautiful a is Today" );
    
    // check empty string    
    char test2[] = "";
    reverse_words( test2 );
    CHECK_CSTR_EQ( test2, "" );
    
    // check single word    
    char test3[] = "word";
    reverse_words( test3 );
    CHECK_CSTR_EQ( test3, "word" );

    return TEST_RESULT;
}
