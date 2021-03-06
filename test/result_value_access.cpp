
// Copyright 2017 Peter Dimov.
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/result/result.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

using namespace boost::result;

struct X
{
    int v_;

    explicit X( int v ): v_( v ) {}

    X( X const& ) = delete;
    X& operator=( X const& ) = delete;
};

int main()
{
    {
        result<int> r;

        BOOST_TEST( !r.has_value() );
        BOOST_TEST( r.has_error() );

        BOOST_TEST_NOT( r );
        BOOST_TEST( !r );

        BOOST_TEST_THROWS( r.value(), std::system_error );

        BOOST_TEST_EQ( r.operator->(), static_cast<int*>(0) );
    }

    {
        result<int> const r;

        BOOST_TEST( !r.has_value() );
        BOOST_TEST( r.has_error() );

        BOOST_TEST_NOT( r );
        BOOST_TEST( !r );

        BOOST_TEST_THROWS( r.value(), std::system_error );

        BOOST_TEST_EQ( r.operator->(), static_cast<int const*>(0) );
    }

    {
        BOOST_TEST( !result<int>().has_value() );
        BOOST_TEST( result<int>().has_error() );

        BOOST_TEST_NOT( result<int>() );
        BOOST_TEST( !result<int>() );

        BOOST_TEST_THROWS( result<int>().value(), std::system_error );

        BOOST_TEST_EQ( result<int>().operator->(), static_cast<int*>(0) );
    }

    {
        result<int> r( 1 );

        BOOST_TEST( r.has_value() );
        BOOST_TEST( !r.has_error() );

        BOOST_TEST( r );
        BOOST_TEST_NOT( !r );

        BOOST_TEST_EQ( r.value(), 1 );
        BOOST_TEST_EQ( *r, 1 );

        BOOST_TEST_EQ( r.operator->(), &*r );
    }

    {
        result<int> const r( 1 );

        BOOST_TEST( r.has_value() );
        BOOST_TEST( !r.has_error() );

        BOOST_TEST( r );
        BOOST_TEST_NOT( !r );

        BOOST_TEST_EQ( r.value(), 1 );
        BOOST_TEST_EQ( *r, 1 );

        BOOST_TEST_EQ( r.operator->(), &*r );
    }

    {
        BOOST_TEST( result<int>( 1 ).has_value() );
        BOOST_TEST( !result<int>( 1 ).has_error() );

        BOOST_TEST( result<int>( 1 ) );
        BOOST_TEST_NOT( !result<int>( 1 ) );

        BOOST_TEST_EQ( result<int>( 1 ).value(), 1 );
        BOOST_TEST_EQ( *result<int>( 1 ), 1 );

        BOOST_TEST( result<int>( 1 ).operator->() != 0 );
    }

    {
        result<X> r( 1 );

        BOOST_TEST( r.has_value() );
        BOOST_TEST( !r.has_error() );

        BOOST_TEST( r );
        BOOST_TEST_NOT( !r );

        BOOST_TEST_EQ( r.value().v_, 1 );
        BOOST_TEST_EQ( (*r).v_, 1 );
        BOOST_TEST_EQ( r->v_, 1 );

        BOOST_TEST_EQ( r.operator->(), &*r );
    }

    {
        result<X> const r( 1 );

        BOOST_TEST( r.has_value() );
        BOOST_TEST( !r.has_error() );

        BOOST_TEST( r );
        BOOST_TEST_NOT( !r );

        BOOST_TEST_EQ( r.value().v_, 1 );
        BOOST_TEST_EQ( (*r).v_, 1 );
        BOOST_TEST_EQ( r->v_, 1 );

        BOOST_TEST_EQ( r.operator->(), &*r );
    }

    {
        BOOST_TEST( result<X>( 1 ).has_value() );
        BOOST_TEST( !result<X>( 1 ).has_error() );

        BOOST_TEST( result<X>( 1 ) );
        BOOST_TEST_NOT( !result<X>( 1 ) );

        BOOST_TEST_EQ( result<X>( 1 ).value().v_, 1 );
        BOOST_TEST_EQ( (*result<X>( 1 )).v_, 1 );
        BOOST_TEST_EQ( result<X>( 1 )->v_, 1 );
    }

    return boost::report_errors();
}
