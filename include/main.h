/*
  Clarissa Verish
  main.h
  2020-08-25

  Header file containing all functions for the hangman game.
*/

#ifndef _MAIN_H_
#define _MAIN_H_

/*
*****************************************************************

                          GAME LOGIC

*****************************************************************
*/
void checkForButtonPress();
void clearGame();
void initiateGame();
bool setupWord();
bool getWord();
bool getLetter();
void checkLetter();
bool checkForCompletion();
void win();
void lose();
void credits();

/*
*****************************************************************

                          GAME GRAPHICS

*****************************************************************
*/
// full-screen graphics
void drawSplashScreen();
void drawInitScreen();
void drawGameScreen();
void drawWinLoseScreen(bool win);
void drawCreditsScreen();
void drawSdErrorScreen();
// menu bar graphics
void drawTopMenu();
void drawBottomMenu();
void drawBottomWinLoseMenu();
void drawBottomCreditsMenu();
// game component graphics
void drawCurrentGuesses();
void drawAlphabet();
void drawHangman();
void drawHangmanOutline();
void drawHangmanSplash();
// generic graphics helpers
void drawScreenTextBox(uint16_t borderColor, uint16_t textColor, String message1, String message2);
void drawLetterBoxWithSize(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor, uint8_t bwidth, uint8_t bheight);
void drawLargeLetterBox(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor);
void drawSmallLetterBox(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor);
void drawMenuButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, String s);

#endif