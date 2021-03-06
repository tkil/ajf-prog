/*
 * Task:
 *
 *   Given the root node to a singly linked list, reverse the last 5
 *   nodes in the list.  For a list with 5 or less nodes, reverse the
 *   whole list.  (Task 2 in tasks.txt)
 *
 * Notes:
 *
 *   Use a "lagged pointer" to keep track of the last non-reversed
 *   node, then an array to temporarily store pointers to the nodes to
 *   be reversed.
 *
 *   Main simplifying trick is to make the "head" of the list the
 *   "next" element on a temporary node; that allows us to remove any
 *   special-casing for "head" itself.
 */

#include <iostream>

namespace
{

class list
{

public:

    struct node
    {
        int val;
        node * next;
    };

    list( const int n );
    ~list();

    void reverse_last_five();

    void emit( const char * ) const;

private:

    node * head;

};

list::list( const int n )
    : head( 0 )
{
    if ( n >= 1 )
    {
        head = new node{ 0, 0 };
        node * p = head;
        for ( int i = 1; i < n; ++i )
        {
            p->next = new node{ i, 0 };
            p = p->next;
        }
    }
}

list::~list()
{
    while ( head )
    {
        node * tmp = head;
        head = head->next;
        delete tmp;
    }
}

void
list::reverse_last_five()
{
    // zero- or one-length lists are already reversed
    if ( ! head || ! head->next )
        return;

    // create a fake node to hang the list off; this makes it much
    // easier to splice the current 'head' itself.
    node tmp{ -1, head };
    tmp.next = head;

    // now scan forward to the end of the list, keeping a pointer that
    // lags 6 nodes behind.
    node * lag = &tmp;
    int seen = 0;
    for ( node * p = head; p; p = p->next )
    {
        if ( seen > 4 )
            lag = lag->next;
        else
            ++seen;
        // std::clog << "  scan: lag=" << lag->val << ", "
        //   "p=" << p->val << std::endl;
    }

    node * prev = 0;
    node * curr = lag->next;
    while ( curr )
    {
        node * next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    lag->next = prev;

    head = tmp.next;
}

void
list::emit( const char * label ) const
{
    std::cout << label;
    node * p = head;
    while ( p )
    {
        std::cout << ' ' << p->val;
        p = p->next;
    }
    std::cout << std::endl;
}

}

int
main( int argc, char * argv[] )
{

#define TEST( n )               \
    do                          \
    {                           \
        list l( n );            \
        l.emit( "pre:  " );     \
        l.reverse_last_five();  \
        l.emit( "post: " );     \
    }                           \
    while ( 0 )

    TEST( 0 );
    TEST( 1 );
    TEST( 4 );
    TEST( 5 );
    TEST( 6 );
    TEST( 10 );

    return 0;
}
