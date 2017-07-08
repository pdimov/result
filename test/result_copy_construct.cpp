
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
    static int instances;

    int v_;

    X(): v_() { ++instances; }

    explicit X( int v ): v_( v ) { ++instances; }

    X( int v1, int v2 ): v_( v1+v2 ) { ++instances; }
    X( int v1, int v2, int v3 ): v_( v1+v2+v3 ) { ++instances; }

    X( X const& r ): v_( r.v_ ) { ++instances; }

    X& operator=( X const& ) = delete;

    ~X() { --instances; }
};

int X::instances = 0;

int main()
{
    {
        result<int> r;
        result<int> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error(), make_error_code( result_errc::not_initialized ) );
    }

    {
        result<int> const r;
        result<int> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error(), make_error_code( result_errc::not_initialized ) );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<X> r;
        result<X> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error(), make_error_code( result_errc::not_initialized ) );

        BOOST_TEST_EQ( X::instances, 0 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<int> r( 0 );
        result<int> r2( r );

        BOOST_TEST( r2.has_value() );
        BOOST_TEST( !r2.has_error() );

        BOOST_TEST_EQ( r2.value(), 0 );
    }

    {
        result<int> const r( 0 );
        result<int> r2( r );

        BOOST_TEST( r2.has_value() );
        BOOST_TEST( !r2.has_error() );

        BOOST_TEST_EQ( r2.value(), 0 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<X> r( 1 );
        result<X> r2( r );

        BOOST_TEST( r2.has_value() );
        BOOST_TEST( !r2.has_error() );

        BOOST_TEST_EQ( r2.value().v_, 1 );

        BOOST_TEST_EQ( X::instances, 2 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<X> const r( 1 );
        result<X> r2( r );

        BOOST_TEST( r2.has_value() );
        BOOST_TEST( !r2.has_error() );

        BOOST_TEST_EQ( r2.value().v_, 1 );

        BOOST_TEST_EQ( X::instances, 2 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        auto ec = make_error_code( std::errc::invalid_argument );

        result<int> r( ec );
        result<int> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error(), ec );
    }

    {
        auto ec = make_error_code( std::errc::invalid_argument );

        result<int> const r( ec );
        result<int> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error(), ec );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<std::string, X> r( 1 );
        result<std::string, X> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error().v_, 1 );

        BOOST_TEST_EQ( X::instances, 2 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    {
        result<std::string, X> const r( 1 );
        result<std::string, X> r2( r );

        BOOST_TEST( !r2.has_value() );
        BOOST_TEST( r2.has_error() );

        BOOST_TEST_EQ( r2.error().v_, 1 );

        BOOST_TEST_EQ( X::instances, 2 );
    }

    BOOST_TEST_EQ( X::instances, 0 );

    return boost::report_errors();
}
