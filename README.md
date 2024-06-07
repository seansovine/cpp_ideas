# C++ Examples

This repo will contain small programs to try out different ideas in C++.

*Note:* The class below is probably YAGNI; there are probably simpler ways to
solve the same problem whenever it comes up. Or there are complexities of
actual use cases that this is ignoring. I just made it for fun and for
learning purposes. And hopefully I'll add more ideas and more discussion
later.

## `maybe_owning`

This has a class `PointerHolder` that acts as a wrapper for either a `std::unique_ptr` or
a raw pointer. If it holds a raw pointer, then when the wrapper object is
destroyed it deletes the pointer it holds. For a `unique_ptr` the wrapper
just holds the underlying pointer and assumes the `unique_ptr` will handle
deallocation whenever it is destroyed. So the name of the project is "maybe
owning", since the wrapper class might or might not own the pointer it holds.

The motivation for this is a situation where you have a `unique_ptr` variable
that may need replaced in some local scope. Something like:

```cpp
auto ptr = std::make_unique< Thing >( args... );

{
	PointerHolder ptrTwo;
	if ( condition ) {
		ptrTwo = PointerHolder( ptr );
	} else {
		ptrTwo = PointerHolder( new Thing( otherArgs... ) );
	}

	func( *ptrTwo );
	ptrTwo->memberFunc();
}

otherFunc( *ptr );
ptr->otherMemberFunc();
```
