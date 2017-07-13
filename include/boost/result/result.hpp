#ifndef BOOST_RESULT_RESULT_HPP_INCLUDED
#define BOOST_RESULT_RESULT_HPP_INCLUDED

// Copyright 2017 Peter Dimov.
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <system_error>
#include <type_traits>
#include <utility>
#include <string>
#include <iosfwd>
#include <cassert>

//

namespace boost
{
namespace result
{

// result_errc

enum class result_errc
{
    not_initialized = 1,
};

} // namespace result
} // namespace boost

namespace std
{

template<> struct is_error_code_enum< boost::result::result_errc >: std::true_type {};

} // namespace std

namespace boost
{
namespace result
{

class result_category_impl: public std::error_category
{
public:

    virtual const char * name() const noexcept
    {
        return "boost::result";
    }

    virtual std::string message( int e ) const
    {
        switch( e )
        {
        case static_cast<int>( result_errc::not_initialized ):

            return "result<> not initialized";

        default:

            return "unknown result<> error";
        }
    }
};

std::error_category const& result_category()
{
    static result_category_impl cat;
    return cat;
}

std::error_code make_error_code( result_errc e )
{
    return std::error_code( static_cast<int>( e ), result_category() );
}

// error_code_to_exception

std::system_error error_code_to_exception( std::error_code const & e )
{
    return std::system_error( e );
}

// in_place_*

struct in_place_value_t
{
    constexpr in_place_value_t() noexcept {}
};

constexpr in_place_value_t in_place_value;

struct in_place_error_t
{
    constexpr in_place_error_t() noexcept {}
};

constexpr in_place_error_t in_place_error;

// result

template<class T, class E = std::error_code> class result
{
private:

    struct in_place_empty_t {};

    union storage
    {
        T t_;
        E e_;

        // construct value
        template<class... A> explicit storage( in_place_value_t, A&&... a ): t_( std::forward<A>(a)... )
        {
        }

        // construct error
        template<class... A> explicit storage( in_place_error_t, A&&... a ): e_( std::forward<A>(a)... )
        {
        }

        // construct empty
        storage( in_place_empty_t )
        {
        }

        ~storage()
        {
        }
    };

    enum class which
    {
        value,
        error
    };

    which i_;
    storage v_;

public:

    // constructors

    // default
    template<class En2 = void, class En = typename std::enable_if<
        std::is_void<En2>::value &&
        std::is_nothrow_constructible<E, result_errc>::value
        >::type>
    constexpr result() noexcept: i_( which::error ), v_( in_place_error_t(), result_errc::not_initialized )
    {
    }

    // explicit, value
    template<class A, class En = typename std::enable_if<
        std::is_constructible<T, A>::value &&
        !std::is_convertible<A, T>::value &&
        !std::is_constructible<E, A>::value
        >::type>
    explicit constexpr result( A&& a )
        noexcept( std::is_nothrow_constructible<T, A>::value )
        : i_( which::value ), v_( in_place_value_t(), std::forward<A>(a) )
    {
    }

    // explicit, error
    template<class A, class En2 = void, class En = typename std::enable_if<
        std::is_constructible<E, A>::value &&
        !std::is_convertible<A, E>::value &&
        !std::is_constructible<T, A>::value
        >::type>
    explicit constexpr result( A&& a )
        noexcept( std::is_nothrow_constructible<E, A>::value )
        : i_( which::error ), v_( in_place_error_t(), std::forward<A>(a) )
    {
    }

    // implicit, value
    template<class A, class En2 = void, class En3 = void, class En = typename std::enable_if<
        std::is_convertible<A, T>::value &&
        !std::is_constructible<E, A>::value
        >::type>
    constexpr result( A&& a )
        noexcept( std::is_nothrow_constructible<T, A>::value )
        : i_( which::value ), v_( in_place_value_t(), std::forward<A>(a) )
    {
    }

    // implicit, error
    template<class A, class En2 = void, class En3 = void, class En4 = void, class En = typename std::enable_if<
        std::is_convertible<A, E>::value &&
        !std::is_constructible<T, A>::value
        >::type>
    constexpr result( A&& a )
        noexcept( std::is_nothrow_constructible<E, A>::value )
        : i_( which::error ), v_( in_place_error_t(), std::forward<A>(a) )
    {
    }

    // more than one arg, value
    template<class... A, class En = typename std::enable_if<
        std::is_constructible<T, A...>::value &&
        !std::is_constructible<E, A...>::value &&
        sizeof...(A) >= 2
        >::type>
    constexpr result( A&&... a )
        noexcept( std::is_nothrow_constructible<T, A...>::value )
        : i_( which::value ), v_( in_place_value_t(), std::forward<A>(a)... )
    {
    }

    // more than one arg, error
    template<class... A, class En2 = void, class En = typename std::enable_if<
        !std::is_constructible<T, A...>::value &&
        std::is_constructible<E, A...>::value &&
        sizeof...(A) >= 2
        >::type>
    constexpr result( A&&... a )
        noexcept( std::is_nothrow_constructible<E, A...>::value )
        : i_( which::error ), v_( in_place_error_t(), std::forward<A>(a)... )
    {
    }

    // tagged, value
    template<class... A, class En = typename std::enable_if<
        std::is_constructible<T, A...>::value
        >::type>
    constexpr result( in_place_value_t, A&&... a )
        noexcept( std::is_nothrow_constructible<T, A...>::value )
        : i_( which::value ), v_( in_place_value_t(), std::forward<A>(a)... )
    {
    }

    // tagged, error
    template<class... A, class En = typename std::enable_if<
        std::is_constructible<E, A...>::value
        >::type>
    constexpr result( in_place_error_t, A&&... a )
        noexcept( std::is_nothrow_constructible<E, A...>::value )
        : i_( which::error ), v_( in_place_error_t(), std::forward<A>(a)... )
    {
    }

private:

    static constexpr bool has_nothrow_copy =
        std::is_nothrow_copy_constructible<T>::value &&
        std::is_nothrow_copy_constructible<E>::value;

public:

    // copy
    result( result const& r )
        noexcept( has_nothrow_copy )
        : i_( r.i_ ), v_( in_place_empty_t() )
    {
        switch( i_ )
        {
        case which::value:

            ::new( static_cast<void*>( &v_.t_ ) ) T( r.v_.t_ );
            break;

        case which::error:

            ::new( static_cast<void*>( &v_.e_ ) ) E( r.v_.e_ );
            break;
        }
    }

private:

    static constexpr bool has_nothrow_move =
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_move_constructible<E>::value;

public:

    // move
    result( result&& r )
        noexcept( has_nothrow_move )
        : i_( r.i_ ), v_( in_place_empty_t() )
    {
        switch( i_ )
        {
        case which::value:

            ::new( static_cast<void*>( &v_.t_ ) ) T( std::move( r.v_.t_ ) );
            break;

        case which::error:

            ::new( static_cast<void*>( &v_.e_ ) ) E( std::move( r.v_.e_ ) );
            break;
        }
    }

    // destructor
    ~result()
    {
        switch( i_ )
        {
        case which::value:

            v_.t_.~T();
            break;

        case which::error:

            v_.e_.~E();
            break;
        }
    }

    // copy assignment

    result& operator=( result const volatile& ) = delete;

    template<class En2 = void, class En = typename std::enable_if<
        std::is_void<En2>::value && has_nothrow_copy
        >::type>
    result& operator=( result const& r )
        noexcept( std::is_nothrow_copy_assignable<T>::value && std::is_nothrow_copy_assignable<E>::value )
    {
        if( i_ == r.i_ )
        {
            switch( i_ )
            {
            case which::value:

                v_.t_ = r.v_.t_;
                break;

            case which::error:

                v_.e_ = r.v_.e_;
                break;
            }
        }
        else
        {
            this->~result();
            ::new( static_cast<void*>( this ) ) result( r );
        }

        return *this;
    }

    template<class En2 = void, class En3 = void, class En = typename std::enable_if<
        std::is_void<En2>::value && !has_nothrow_copy && has_nothrow_move
        >::type>
    result& operator=( result const& r )
        noexcept( std::is_nothrow_move_assignable<result>::value )
    {
        operator=( result( r ) );
        return *this;
    }

    template<class En2 = void, class En3 = void, class En4 = void, class En = typename std::enable_if<
        std::is_void<En2>::value && !has_nothrow_copy && !has_nothrow_move
        >::type>
    result& operator=( result const& r )
    {
        if( i_ == r.i_ )
        {
            switch( i_ )
            {
            case which::value:

                v_.t_ = r.v_.t_;
                break;

            case which::error:

                v_.e_ = r.v_.e_;
                break;
            }
        }
        else
        {
            this->~result();

            try
            {
                ::new( static_cast<void*>( this ) ) result( r );
            }
            catch( ... )
            {
                ::new( static_cast<void*>( this ) ) result();
                throw;
            }
        }

        return *this;
    }

    // move assignment

    template<class En2 = void, class En = typename std::enable_if<
        std::is_void<En2>::value && !has_nothrow_move
        >::type>
    result& operator=( result&& r )
    {
        if( i_ == r.i_ )
        {
            switch( i_ )
            {
            case which::value:

                v_.t_ = std::move( r.v_.t_ );
                break;

            case which::error:

                v_.e_ = std::move( r.v_.e_ );
                break;
            }
        }
        else
        {
            this->~result();

            try
            {
                ::new( static_cast<void*>( this ) ) result( std::move( r ) );
            }
            catch( ... )
            {
                ::new( static_cast<void*>( this ) ) result();
                throw;
            }
        }

        return *this;
    }

    template<class En2 = void, class En3 = void, class En = typename std::enable_if<
        std::is_void<En2>::value && has_nothrow_move
        >::type>
    result& operator=( result&& r )
        noexcept( std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_assignable<E>::value )
    {
        if( i_ == r.i_ )
        {
            switch( i_ )
            {
            case which::value:

                v_.t_ = std::move( r.v_.t_ );
                break;

            case which::error:

                v_.e_ = std::move( r.v_.e_ );
                break;
            }
        }
        else
        {
            this->~result();
            ::new( static_cast<void*>( this ) ) result( std::move( r ) );
        }

        return *this;
    }

    // queries

    constexpr bool has_value() const noexcept
    {
        return i_ == which::value;
    }

    constexpr bool has_error() const noexcept
    {
        return i_ == which::error;
    }

    constexpr explicit operator bool() const noexcept
    {
        return i_ == which::value;
    }

    // checked value access
#if defined( BOOST_NO_CXX11_REF_QUALIFIERS )

    BOOST_CXX14_CONSTEXPR T value() const
    {
        if( has_value() )
        {
            return v_.t_;
        }
        else
        {
            throw error_code_to_exception( v_.e_ );
        }
    }

#else

    BOOST_CXX14_CONSTEXPR T& value() &
    {
        if( has_value() )
        {
            return v_.t_;
        }
        else
        {
            throw error_code_to_exception( v_.e_ );
        }
    }

    BOOST_CXX14_CONSTEXPR T const& value() const&
    {
        if( has_value() )
        {
            return v_.t_;
        }
        else
        {
            throw error_code_to_exception( v_.e_ );
        }
    }

    BOOST_CXX14_CONSTEXPR T&& value() &&
    {
        return std::move( value() );
    }

    BOOST_CXX14_CONSTEXPR T const&& value() const&&
    {
        return std::move( value() );
    }

#endif

    // unchecked value access

    BOOST_CXX14_CONSTEXPR T* operator->() noexcept
    {
        return has_value()? &v_.t_: 0;
    }

    BOOST_CXX14_CONSTEXPR T const* operator->() const noexcept
    {
        return has_value()? &v_.t_: 0;
    }

#if defined( BOOST_NO_CXX11_REF_QUALIFIERS )

    BOOST_CXX14_CONSTEXPR T& operator*() noexcept
    {
        T* p = operator->();

        assert( p != 0 );

        return *p;
    }

    BOOST_CXX14_CONSTEXPR T const& operator*() const noexcept
    {
        T const* p = operator->();

        assert( p != 0 );

        return *p;
    }

#else

    BOOST_CXX14_CONSTEXPR T& operator*() & noexcept
    {
        T* p = operator->();

        assert( p != 0 );

        return *p;
    }

    BOOST_CXX14_CONSTEXPR T const& operator*() const & noexcept
    {
        T const* p = operator->();

        assert( p != 0 );

        return *p;
    }

    BOOST_CXX14_CONSTEXPR T&& operator*() && noexcept
    {
        return std::move(**this);
    }

    BOOST_CXX14_CONSTEXPR T const&& operator*() const && noexcept
    {
        return std::move(**this);
    }

#endif

    // error access

    constexpr E error() const noexcept
    {
        return has_error()? v_.e_: E();
    }

    // swap

    BOOST_CXX14_CONSTEXPR void swap( result& r ) /*noexcept(...)*/
    {
        if( i_ == r.i_ )
        {
            using std::swap;

            switch( i_ )
            {
            case which::value:

                swap( v_.t_, r.v_.t_ );
                break;

            case which::error:

                swap( v_.e_, r.v_.e_ );
                break;
            }
        }
        else
        {
            std::swap( *this, r );
        }
    }

    template<class T2, class E2> BOOST_CXX14_CONSTEXPR bool operator==( result<T2, E2> const & r ) const
    {
        if( i_ != r.i_ ) return false;

        switch( i_ )
        {
        case which::value:

            return v_.t_ == r.v_.t_;

        case which::error:

            return v_.e_ == r.v_.e_;

        default:

            return false;
        }
    }

    template<class T2, class E2> BOOST_CXX14_CONSTEXPR bool operator!=( result<T2, E2> const & r ) const
    {
        if( i_ != r.i_ ) return true;

        switch( i_ )
        {
        case which::value:

            return v_.t_ != r.v_.t_;

        case which::error:

            return v_.e_ != r.v_.e_;

        default:

            return false;
        }
    }
};

template<class Ch, class Tr, class T, class E> std::basic_ostream<Ch, Tr>& operator<<( std::basic_ostream<Ch, Tr>& os, result<T, E> const & r )
{
    if( r.has_value() )
    {
        os << "value:" << *r;
    }
    else
    {
        os << "error:" << r.error();
    }

    return os;
}

} // namespace result
} // namespace boost

#endif // #ifndef BOOST_RESULT_RESULT_HPP_INCLUDED
