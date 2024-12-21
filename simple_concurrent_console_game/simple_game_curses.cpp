// Version of the simple "game" program
// using ncurses for console output.
//
// Created by sean on 12/20/24.
//

#include <array>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

#include "curses_console.h"

#define KEY_Q 0x71

// Dimensions of the game board we are using.
constexpr int WIDTH = 8;
constexpr int HEIGHT = 3;

// Stores current location of the game piece on the game board.
// Assumed to stay within range [0, WIDTH-1] x [0, HEIGHT-1].
static std::array piecePosition{ 0, 0 };

// Print the board on the terminal using ncurses.
void print_board( CursesConsole& console ) {
  console.moveCursor( 0, 0 );
  console.addString( "Use arrow keys to move piece or q to stop playing:" );

  constexpr int BOARD_INDENT = 1;

  for (int i = 0; i < HEIGHT; i++) {
    console.moveCursor( BOARD_INDENT, i + 2 );

    for (int j = 0; j < WIDTH; j++) {
      if (i == piecePosition[ 1 ] && j == piecePosition[ 0 ]) {
        console.addStringWithColor( "X", CursesConsole::ColorPair::RedOnBlack );
      } else {
        console.addChar( 'o' );
      }

      console.addChar( ' ' );
    }
  }

  console.moveCursor( BOARD_INDENT, HEIGHT + 2 );
  std::string separator;
  for (int j = 0; j < WIDTH - 1; j++) {
    separator += "==";
  }
  separator += "=";
  console.addString( separator );

  console.writeBuffer();
}

void handleKeyPress( int keyCode ) {
  switch (keyCode) {
    case KEY_UP:
      piecePosition[ 1 ] = std::max( piecePosition[ 1 ] - 1, 0 );
      break;
    case KEY_DOWN:
      piecePosition[ 1 ] = std::min( piecePosition[ 1 ] + 1, HEIGHT - 1 );
      break;
    case KEY_RIGHT:
      piecePosition[ 0 ] = std::min( piecePosition[ 0 ] + 1, WIDTH - 1 );
      break;
    case KEY_LEFT:
      piecePosition[ 0 ] = std::max( piecePosition[ 0 ] - 1, 0 );
      break;
    default:  // NOT ARROW KEY
      break;
  }
}

int main() {
  // Ncurses console interface.
  CursesConsole console;

  // Turn off input buffering.
  console.noInputBuffer();

  // Hide cursor.
  console.cursorVisible( false );
  // Clear screen.
  console.writeBuffer();

  // Timeout for getCh: This sets the
  // maximum refresh rate in the main loop.
  //
  // NOTE: Currently all state changes are triggered
  // by user input, but we will add code that runs
  // and makes changes independently. For example,
  // we could have another piece that chases the
  // user piece. So we use the timeout to keep updates
  // happening independently of user input.

  constexpr int TIMEOUT_MS = 50;
  console.blockingGetCh( TIMEOUT_MS );

  unsigned int counter = 0;
  constexpr unsigned int MAX_COUNT = 1 << 12;

  // Main loop.
  while (true) {
    print_board( console );

    // Print refresh count for development use.
    console.moveCursor( 0, HEIGHT + 4 );
    console.addString( "Refresh count: " + std::to_string( counter++ % MAX_COUNT ) + "     " );

    if (int ch = console.getChar(); ch != CursesConsole::NO_KEY) {
      if (ch == KEY_Q) {
        break;
      }
      handleKeyPress( ch );
    }
  }

  console.moveCursor( 0, 0 );

  // Press any key... message w/ color.
  console.addString( "Press " );
  console.addStringWithColor( "any key", CursesConsole::ColorPair::BlueOnBlack );
  console.addString( " to continue..." );

  // Clear rest of old text from line.
  auto lineErase = std::string( 27, ' ' );
  console.addString( lineErase );

  console.writeBuffer();

  // Wait for user input.
  console.blockingGetCh();
  console.getChar();
}
