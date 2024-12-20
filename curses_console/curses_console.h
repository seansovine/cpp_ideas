// Provides a wrapper around ncurses functionality.
//
// Created by sean on 12/20/24.
//

#ifndef CURSES_CONSOLE_H
#define CURSES_CONSOLE_H

#include <string>

// Allows moving full ncurses header include to
// the implementation file, so clients don't get
// all of its declarations.
typedef struct _win_st WINDOW;

// ------------------------------
// A class providing an interface
// to ncurses functionality.

class CursesConsole {
public:
  CursesConsole();

  ~CursesConsole();

  // Set current text/background colors.
  void whiteOnBlack();
  void blueOnBlack();

  void writeBuffer();

  void moveCursor( int x, int y );

  void addString( const std::string &str );

  // NOTE: These are not static because they assume
  // the initialization that is done in the constructor.

  int getChar();

  void cursorVisible( bool visible );

private:
  WINDOW *scr;
};

#endif  // CURSES_CONSOLE_H
