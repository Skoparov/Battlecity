#ifndef MAKE_UNIQUE_H
#define MAKE_UNIQUE_H

#include <memory>

namespace std
{

template< class type, typename... ctor_args >
std::unique_ptr< type > make_unique( ctor_args&&... args )
{
    return std::unique_ptr< type >( new type( std::forward< ctor_args >( args )... ) );
}

}// std

#endif
