
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
    X() = delete;
};

int main()
{
    {
        result<int> r;

        BOOST_TEST( !r.has_value() );
        BOOST_TEST( r.has_error() );

        BOOST_TEST_EQ( r.error(), make_error_code( result_errc::not_initialized ) );
    }

    {
        result<X> r;

        BOOST_TEST( !r.has_value() );
        BOOST_TEST( r.has_error() );

        BOOST_TEST_EQ( r.error(), make_error_code( result_errc::not_initialized ) );
    }

    return boost::report_errors();
}
