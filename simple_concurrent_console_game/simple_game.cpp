// A simple console game to experiment with concurrent
// input and display implementations.

// Linux (Unix) specific terminal interaction.
#include <termios.h>
#define STDIN_FILENO 0

#include <array>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

// Dimensions of the game board we are using.
constexpr int WIDTH = 8;
constexpr int HEIGHT = 3;

// Stores current location of the game piece on the game board.
// Assumed to stay within range [0, WIDTH-1] x [0, HEIGHT-1].
static std::array< int, 2 > piecePosition{ 0, 0 };

// Print the board on the next lines of the terminal.
// Called by main thread in render loop.
// TODO: Use something like ncurses to render in place.
void print_board() {
  for ( int i = 0; i < HEIGHT; i++ ) {
    for ( int j = 0; j < WIDTH; j++ ) {
      char thisPositionChar = 'o';
      if ( i == piecePosition[ 1 ] && j == piecePosition[ 0 ] ) {
        thisPositionChar = 'X';
      }
      std::cout << thisPositionChar << " ";
    }
    std::cout << std::endl;
  }
  for ( int j = 0; j < WIDTH; j++ ) {
    std::cout << "==";
  }
  std::cout << "\n";
}

void setupTerminal() {
  struct termios t;

  tcgetattr( STDIN_FILENO, &t );

  // Prevent terminal from buffering input.
  t.c_lflag &= ~ICANON;
  // Prevent terminal from immediately echoing input.
  t.c_lflag &= ~ECHO;

  tcsetattr( STDIN_FILENO, TCSANOW, &t );
}

enum class KeyDirection { UP, DOWN, LEFT, RIGHT, NONE };

KeyDirection getKeypress() {
  char c, d, e;

  // Note that arrow keys are three chars long,
  // and start with characters 27 and 91.
  std::cin >> c;
  if ( c != 27 ) {
    return KeyDirection::NONE;
  }
  std::cin >> d;
  if ( d != 91 ) {
    return KeyDirection::NONE;
  }

  std::cin >> e;
  switch ( e ) {
    case 65:
      return KeyDirection::UP;
    case 66:
      return KeyDirection::DOWN;
    case 67:
      return KeyDirection::RIGHT;
    case 68:
      return KeyDirection::LEFT;
    default:
      return KeyDirection::NONE;
  }
}

// Lifted directly from the code samples from Williams' book.
// See: https://github.com/anthonywilliams/ccia_code_samples
// This is Listing 4.5.

template < typename T >
class threadsafe_queue {
private:
  mutable std::mutex mut;
  std::queue< T > data_queue;
  std::condition_variable data_cond;

public:
  threadsafe_queue() {}
  threadsafe_queue( threadsafe_queue const& other ) {
    std::lock_guard< std::mutex > lk( other.mut );
    data_queue = other.data_queue;
  }

  void push( T new_value ) {
    std::lock_guard< std::mutex > lk( mut );
    data_queue.push( new_value );
    data_cond.notify_one();
  }

  void wait_and_pop( T& value ) {
    std::unique_lock< std::mutex > lk( mut );
    data_cond.wait( lk, [ this ] { return !data_queue.empty(); } );
    value = data_queue.front();
    data_queue.pop();
  }

  std::shared_ptr< T > wait_and_pop() {
    std::unique_lock< std::mutex > lk( mut );
    data_cond.wait( lk, [ this ] { return !data_queue.empty(); } );
    std::shared_ptr< T > res( std::make_shared< T >( data_queue.front() ) );
    data_queue.pop();
    return res;
  }

  bool try_pop( T& value ) {
    std::lock_guard< std::mutex > lk( mut );
    if ( data_queue.empty() ) return false;
    value = data_queue.front();
    data_queue.pop();
    return true;
  }

  std::shared_ptr< T > try_pop() {
    std::lock_guard< std::mutex > lk( mut );
    if ( data_queue.empty() ) return std::shared_ptr< T >();
    std::shared_ptr< T > res( std::make_shared< T >( data_queue.front() ) );
    data_queue.pop();
    return res;
  }

  bool empty() const {
    std::lock_guard< std::mutex > lk( mut );
    return data_queue.empty();
  }
};

// Queue to hold all key press events that haven't been applied to board.
threadsafe_queue< KeyDirection > keypressQueue;

// Function to be executed by terminal input thread. Handles
// getting a keypress event from standard input and stashing
// it in threadsafe queue if it is an UDLR key.
void handleInput() {
  while ( true ) {
    setupTerminal();
    KeyDirection lastPressedDirection = KeyDirection::NONE;

    // Only want to save direction key presses.
    while ( lastPressedDirection == KeyDirection::NONE ) {
      lastPressedDirection = getKeypress();
#if DEBUG
      switch ( lastPressedDirection ) {
        case KeyDirection::NONE:
          std::cout << "Key pressed was not a direction key.\n";
          break;
        case KeyDirection::UP:
          std::cout << "The UP key was pressed.\n";
          break;
        case KeyDirection::DOWN:
          std::cout << "The DOWN key was pressed.\n";
          break;
        case KeyDirection::LEFT:
          std::cout << "The LEFT key was pressed.\n";
          break;
        case KeyDirection::RIGHT:
          std::cout << "The RIGHT key was pressed.\n";
          break;
      }
#endif
    }

    keypressQueue.push( lastPressedDirection );
  }
}

int main() {
  using namespace std::chrono_literals;

  print_board();

  // Launch terminal input thread.
  std::thread inputThread( handleInput );

  // Main render loop.
  while ( true ) {
    while ( !keypressQueue.empty() ) {
      KeyDirection nextPressed = KeyDirection::NONE;
      keypressQueue.try_pop( nextPressed );

      // Now apply keyPress to board.
      switch ( nextPressed ) {
        case KeyDirection::UP:
          piecePosition[ 1 ] = std::max( piecePosition[ 1 ] - 1, 0 );
          break;
        case KeyDirection::DOWN:
          piecePosition[ 1 ] = std::min( piecePosition[ 1 ] + 1, HEIGHT - 1 );
          break;
        case KeyDirection::LEFT:
          piecePosition[ 0 ] = std::max( piecePosition[ 0 ] - 1, 0 );
          break;
        case KeyDirection::RIGHT:
          piecePosition[ 0 ] = std::min( piecePosition[ 0 ] + 1, WIDTH - 1 );
          break;
        case KeyDirection::NONE:
          throw std::runtime_error( "A NONE type direction should not be queued." );
          break;
      }

#if DEBUG
      switch ( nextPressed ) {
        case KeyDirection::UP:
          std::cout << "Applying an UP keypress.\n";
          break;
        case KeyDirection::DOWN:
          std::cout << "Applying a DOWN keypress.\n";
          break;
        case KeyDirection::LEFT:
          std::cout << "Applying a LEFT keypress.\n";
          break;
        case KeyDirection::RIGHT:
          std::cout << "Applying a RIGHT keypress.\n";
          break;
        case KeyDirection::NONE:
          std::cout << "We should never get here.\n";
          break;
      }
      std::cout << "Updated piece position to (" << piecePosition[ 0 ] << ", " << piecePosition[ 1 ]
                << ").\n";
#endif
    }
    print_board();

    // Pause main thread -- experiment with different times (if any).
    std::this_thread::sleep_for( 500ms );
  }
}
