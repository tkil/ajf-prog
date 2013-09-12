#ifndef TEST_H
#define TEST_H 1

#include <iostream>

#include <string.h>

namespace
{

struct TestCounter
{
    TestCounter() : count( 0 ), failures( 0 ) {}
    int count;
    int failures;

    void pass() { ++count; }
    void fail() { ++count; ++failures; }
};

TestCounter testCounter;

}

#define CHECK_CSTR_EQ( result, expected )                       \
    do                                                          \
    {                                                           \
        if ( strcmp( result, expected ) != 0 )                  \
        {                                                       \
            std::clog << __FILE__ << ":" << __LINE__ << ": "    \
              "expected='" << expected << "' "                  \
              "result='" << result << "'" << std::endl;;        \
            testCounter.pass();                                 \
        }                                                       \
        else                                                    \
        {                                                       \
            testCounter.fail();                                 \
        }                                                       \
    }                                                           \
    while ( 0 )

#define TEST_RESULT ( testCounter.failures == 0 ? 0 : 1 )

#endif
