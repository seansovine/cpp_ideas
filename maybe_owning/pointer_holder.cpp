#include <iostream>
#include <memory>

// Type trait idea from: https://stackoverflow.com/questions/65752626.

template < typename T >
struct is_unique_ptr : std::false_type {};
template < typename T >
struct is_unique_ptr< std::unique_ptr< T > > : std::true_type {};

// Holds a raw pointer or unique pointer; if a raw pointer, deletes it on destruction.

template < typename T, typename WrappedType >
class PointerHolder {
  T* ptr;

public:
  template < typename U >
  PointerHolder( const std::unique_ptr< U >& u ) : ptr{ u.get() } {};

  template < typename U >
  PointerHolder( U* u ) : ptr{ u } {};

  // Treat r-value smart pointer like raw pointer to prevent use after free.
  template < typename U >
  PointerHolder( std::unique_ptr< U >&& u ) : ptr{ u.release() } {};

  template < typename U >
  PointerHolder( U ) = delete;

  ~PointerHolder() {
    if constexpr ( !is_unique_ptr< WrappedType >::value ) {
      delete ptr;
    }
  };

  T& operator*() {
    return *ptr;
  }

  T* operator->() {
    return ptr;
  }
};

// Guides for CTAD.

template < typename U >
PointerHolder( const std::unique_ptr< U >& ) -> PointerHolder< U, std::unique_ptr< U > >;

template < typename U >
PointerHolder( std::unique_ptr< U >&& ) -> PointerHolder< U, U* >;

template < typename U >
PointerHolder( U* ) -> PointerHolder< U, U* >;

/* Test it. */

int main() {
  auto stringPtr = std::make_unique< std::string >( "Hello!" );

  PointerHolder holderOne( stringPtr );
  PointerHolder holderTwo( new std::string( "Hello again!" ) );
  PointerHolder holderTre( std::make_unique< std::string >( "And again!" ) );

  std::cout << *holderOne << std::endl;
  std::cout << *holderTwo << std::endl;
  std::cout << *holderTre << std::endl;

  std::cout << "Size of " << *holderOne << " is " << holderOne->size() << "." << std::endl;

  return 0;
};
