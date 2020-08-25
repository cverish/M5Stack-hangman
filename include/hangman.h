/*
  Clarissa Verish
  hangman.h
  2020-08-25

  Header file containing the variables for the Hangman game.
*/

#ifndef _HANGMAN_H_
#define _HANGMAN_H_

#define KEYBOARD_I2C_ADDR 0X08
#define KEYBOARD_INT 5
#define MAX_WORD_LENGTH 11
#define NUM_LETTERS 26

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BOTTOM_MARGIN 6
#define TOP_MARGIN 6
#define SIDE_MARGIN 3
#define LARGE_BOX_WIDTH 24
#define LARGE_BOX_HEIGHT 28
#define SMALL_BOX_WIDTH 18
#define SMALL_BOX_HEIGHT 22

#define TOP_XCOOR 0
#define TOP_YCOOR 0
#define HANGMAN_XCOOR 20
#define HANGMAN_YCOOR 48
#define KEYBOARD_XCOOR 132
#define KEYBOARD_YCOOR 48
#define WORDBOX_XCOOR 0
#define WORDBOX_YCOOR 172
#define BOTTOM_XCOOR 0
#define BOTTOM_YCOOR 216

#define HANGMAN_OUTLINE_COLOR TFT_DARKGREY
#define HANGMAN_FILL_COLOR TFT_WHITE

extern char secretWord[MAX_WORD_LENGTH];
extern char guesses[MAX_WORD_LENGTH];
extern char guessedLetters[NUM_LETTERS];

extern char currentLetter;

// number of wrong guesses
extern int numWrong;
// number of guesses allowed in game
extern int numGuesses;
// game has been forfeited
extern bool forfeit;

// number of total wins
extern uint16_t wins;
// number of total losses
extern uint16_t losses;
// best streak of wins
extern uint16_t streak;

#endif
