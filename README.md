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

# `simple_concurrent_console_game`

This is a very simple console "game", for testing out basic concurrency ideas.
It has one thread that reads key input from a UNIX standard input (which the program
is expected to be associated with) and another thread that has a render loop that
prints the current state of a very simple game board. There is a thread-safe queue
which has been lifted directly from the example code repo for Anthony Williams' book
_C++ Concurrency in Action_,
[here](https://github.com/anthonywilliams/ccia_code_samples/blob/main/listings/listing_4.5.cpp).
(Reading through that book motivated this example.)
The key input thread populates this queue, and before each draw the render loop
applies all the arrow key presses stored in the queue to update the current position
of the "game piece". The render thread sleeps half a second at the end of each iteration.
