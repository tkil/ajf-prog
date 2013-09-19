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
        // std::clog << "  scan: lag=" << lag->val << ", p=" << p->val << std::endl;
    }

    node * tail[5];
    int i = 0;

    for ( node * p = lag->next; p; p = p->next )
    {
        // std::clog << "  tail: [" << i << "]=" << p->val << std::endl;
        tail[i++] = p;
    }

    while ( i > 0 )
    {
        lag->next = tail[--i];
        lag = lag->next;
        // std::clog << "  flip: lag=" << lag->val << std::endl;
    }
    lag->next = 0;

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
