#include "curses_console.h"

#include <ncurses.h>

#include <stdexcept>

CursesConsole::CursesConsole() {
  // This should be stdscr.
  scr = initscr();

  if (scr == nullptr) {
    throw std::runtime_error( "Failed to initialize curses window." );
  }

  // Disable buffering.
  cbreak();
  // Disable echoing.
  noecho();
  // Allow capturing special keys.
  keypad( scr, TRUE );

  // Initialize colors.
  start_color();
  // Create explicit color pairs.
  init_pair( 1, COLOR_WHITE, COLOR_BLACK );
  init_pair( 2, COLOR_BLUE, COLOR_BLACK );
}

CursesConsole::~CursesConsole() {
  // Restore terminal state.
  endwin();
}

void CursesConsole::whiteOnBlack() {
  wattron( scr, COLOR_PAIR( 1 ) );
}

void CursesConsole::blueOnBlack() {
  wattron( scr, COLOR_PAIR( 2 ) );
}

void CursesConsole::writeBuffer() {
  wrefresh( scr );
}

void CursesConsole::moveCursor( int x, int y ) {
  wmove( scr, y, x );
}

void CursesConsole::addString( const std::string &str ) {
  for (const auto &ch : str) {
    waddch( scr, ch );
  }
}

// NOTE: These are not static because they assume
// the initialization that is done in the constructor.

int CursesConsole::getChar() {
  return getch();
}

void CursesConsole::cursorVisible( bool visible ) {
  curs_set( visible ? 1 : 0 );
}
