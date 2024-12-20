// Version of the simple "game" program
// using ncurses for console output.
//
// Created by sean on 12/20/24.
//

#include "curses_console.h"

int main() {
  CursesConsole console;

  // Hide cursor.
  console.cursorVisible( false );
  // Clear screen.
  console.writeBuffer();

  console.moveCursor(1,1);

  console.addString("Press ");
  console.blueOnBlack();
  console.addString("any key");
  console.whiteOnBlack();
  console.addString(" to continue...");

  console.writeBuffer();

  // Should wait for user key input.
  console.getChar();

  // TODO: We will re-implement our simple game
  // using ncurses, following the advice given
  // here:
  //  https://stackoverflow.com/a/78972635/3791169
}
