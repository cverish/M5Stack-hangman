/*
  Clarissa Verish
  main.cpp
  2020-08-25

  Contains the game logic for the Hangman game. 
*/

#include <Arduino.h>
#include <M5Stack.h>
#include "hangman.h"
#include "main.h"

// array of correct guessed letters
char guesses[MAX_WORD_LENGTH];
// array containing all guesses, correct and incorrect
char guessedLetters[NUM_LETTERS];
// current letter that has been guessed
char currentLetter;

// number of wrong guesses
int numWrong = 0;
// number of guesses allowed in game
int numGuesses = 10;
// game has been forfeited
bool forfeit = false;

// number of total wins
uint16_t wins = 0;
// number of total losses
uint16_t losses = 0;
// best streak of wins
uint16_t streak = 0;

// data structure for holding the stages of the game
typedef enum
{
  INIT,
  STAGE_NEW_GAME,
  STAGE_INPUT_LETTER,
  STAGE_CHECK_LETTER,
  STAGE_CHECK_WORD,
  STAGE_FINISH,
  STAGE_CREDITS,
} appStages_t;

// initializing the game at stage 0 - INIT
appStages_t stage = INIT;

/*
  setup()

  initializes the M5Stack and the SD card. 
  connects the M5Stack Faces keyboard.
*/
void setup()
{
  M5.begin();
  SD.begin();
  Wire.begin();
  M5.Lcd.setTextDatum(TC_DATUM);
  pinMode(KEYBOARD_INT, INPUT_PULLUP);
}

/*
  loop()

  First checks whether any of the bottom menu keys have been pressed.
  Then checks the stage of the game and executes code according to stage.
*/
void loop()
{
  // check for any button presses that will interrupt game stage
  M5.update();
  checkForButtonPress();
  // switch case containing stages of the game.
  switch (stage)
  {
  case INIT:
    // displays the splash screen and waits for button press. resets all variables and move to STAGE_NEW_GAME
    clearGame();
    M5.update();
    while (!M5.BtnA.wasReleased() && !M5.BtnB.wasReleased() && !M5.BtnC.wasReleased())
    {
      M5.update();
    }
    stage = STAGE_NEW_GAME;
    break;
  case STAGE_NEW_GAME:
    // start a new game and move to STAGE_INPUT_LETTER step
    initiateGame();
    stage = STAGE_INPUT_LETTER;
    break;
  case STAGE_INPUT_LETTER:
    // wait for letter input, then move to STAGE_CHECK_LETTER
    if (getLetter())
    {
      stage = STAGE_CHECK_LETTER;
    }
    break;
  case STAGE_CHECK_LETTER:
    // check if the letter is correct or incorrect, and redraw word. Reset current letter and move to STAGE_LETTER_FEEDBACK
    checkLetter();
    stage = STAGE_CHECK_WORD;
    break;
  case STAGE_CHECK_WORD:
    // check whether the game is complete. If so, move to STAGE_FINISH. If not, move back to STAGE_INPUT_LETTER
    if (checkForCompletion())
    {
      stage = STAGE_FINISH;
    }
    else
    {
      stage = STAGE_INPUT_LETTER;
    }
    break;
  case STAGE_FINISH:
    // wait until user restarts game, then move back to INIT stage
    if (m5.BtnA.wasReleased())
    {
      stage = STAGE_NEW_GAME;
    }
    if (m5.BtnB.wasReleased())
    {
      stage = INIT;
    }
    if (m5.BtnC.wasReleased())
    {
      stage = STAGE_CREDITS;
    }
    break;
  case STAGE_CREDITS:
    credits();
    stage = STAGE_CHECK_WORD;
    break;
  }
}

/*
  checkForButtonPress()

  Handles all checks for button presses within loop()
*/
void checkForButtonPress()
{
  // checks if the Clear button has been pressed. If so, show splash screen and reset all counters.
  if (stage == STAGE_FINISH && M5.BtnB.wasReleased())
  {
    stage = INIT;
    M5.update();
  }
  // checks if the Forfeit button has been pressed. If so, show lose screen and end current game.
  else if (stage != INIT && stage != STAGE_FINISH && m5.BtnB.wasReleased())
  {
    lose();
    forfeit = true;
    stage = STAGE_FINISH;
    M5.update();
  }
  // checks if the New menu button has been pressed. If so, restarts game.
  else if (stage != INIT && stage != STAGE_CREDITS && m5.BtnA.wasReleased())
  {
    stage = STAGE_NEW_GAME;
    M5.update();
  }
  // checks if the Credits button has been pressed. If so, show stage credits screen
  else if (stage != INIT && m5.BtnC.wasReleased())
  {
    stage = STAGE_CREDITS;
    M5.update();
  }
}

/*
  clearGame()

  Starts the game from a clean slate - clears all variables and displays splash screen
*/
void clearGame()
{
  drawSplashScreen();
  wins = 0;
  losses = 0;
  streak = 0;
  currentLetter = 0;
  forfeit = false;
}

/*
  initiateGame()

  Initiates a new game.

*/
void initiateGame()
{
  // reset the currentLetter
  currentLetter = 0;
  // draw the init screen
  drawInitScreen();
  // if the word does not set up properly, display the SD Card error. Loop forever to keep status up - game will not continue.
  // Add SD card and reset M5Stack.
  if (!setupWord())
  {
    drawSdErrorScreen();
    while (true)
    {
    }
  }
  // reset the array of guessed letters to null characters.
  for (int i = 0; i < NUM_LETTERS; i++)
  {
    guessedLetters[i] = 0;
  }
  // reset the wrong guess counter
  numWrong = 0;
  // draw the game screen
  drawGameScreen();
}

/*
  setupWord()

  Resets the secretWord and correct guesses to an empty array. Returns True if word fetched correctly, false if not.
*/
bool setupWord()
{
  // resetting the secret word and the correct guesses
  for (int i = 0; i < MAX_WORD_LENGTH; i++)
  {
    guesses[i] = 0;
    secretWord[i] = 0;
  }
  // returns the new word using getWord() within the sd_runner file
  return getWord();
}

/*
  getLetter()

  Checks whether a letter has been pressed by the user. If so, returns true. If not, returns false.
*/
bool getLetter()
{
  M5.Lcd.setCursor(150, 1);
  // reset currentLetter
  currentLetter = 0;
  // checks whether a button is being pressed
  if (digitalRead(KEYBOARD_INT) == LOW)
  {
    // request 1 byte from keyboard
    Wire.requestFrom(KEYBOARD_I2C_ADDR, 1);
    while (Wire.available())
    {
      // receive a byte as character
      uint8_t key_val = Wire.read();
      // checks whether a byte exists
      if (key_val != 0)
      {
        // checks if the key being pressed is on the keyboard
        if (key_val >= 32 && key_val < 127)
        { // ASCII String
          Serial.print((char)key_val);
          // checks whether the key pressed is a letter A-Z and whether it is the currently selected letter
          if ((key_val >= 97 && key_val <= 122) && key_val != currentLetter)
          {
            // sets the current letter to the pressed key and returns true.
            currentLetter = (char)key_val;
            return true;
          }
        }
      }
    }
  }
  return false;
}

/*
  checkLetter()

  Checks whether the selected letter is in the secret word or not.

*/
void checkLetter()
{
  // has letter beeng guessed yet?
  if (currentLetter != guessedLetters[currentLetter - 'a'] && currentLetter != 0)
  {
    // add to guessed array
    guessedLetters[currentLetter - 'a'] = currentLetter;
    // redraw current guesses and alphabet
    drawCurrentGuesses();
    drawAlphabet();
    // check whether current character is in secretWord - if so, return.
    for (int i = 0; i < MAX_WORD_LENGTH; i++)
    {
      char c = secretWord[i];
      if (c == currentLetter && c != 0)
      {
        return;
      }
    }
    // current character is not in secretWord
    numWrong++;
    drawHangman();
  }
}

/*
  checkForCompletion()

  Checks whether the game has been completed. 

  Creates a counter for correct letters and checks whether the letter in secretWord matches the letter in guesses. 
  If it does, increment correctCount.

  If the number of guesses allowed matches the number of incorrect guesses, the player loses.
  If the correctCount matches the length of the secret word, the player wins.

  Returns true if the game is complete, false if the game is not complete.
*/
bool checkForCompletion()
{
  for (uint16_t i = 0; i < MAX_WORD_LENGTH; i++)
  {
    if (secretWord[i] != guesses[i] && numGuesses != numWrong)
    {
      // guesses do not equal secret word and player still has guesses left
      return false;
    }
  }
  if (numGuesses == numWrong || forfeit)
  {
    lose();
  }
  else
  {
    win();
  }
  // redraw top menu with new wins/losses/streak
  drawTopMenu();
  return true;
}

/*
  Shows the win screen and increases the wins counter. Also increments the streak counter.
*/
void win()
{
  drawWinLoseScreen(true);
  wins++;
  streak++;
}

/*
  lose()

  Shows the lose screen and increases the losses counter. Sets the current streak to zero.
*/
void lose()
{
  drawWinLoseScreen(false);
  losses++;
  streak = 0;
}

/*
  Shows the credits. Waits for button B to be pressed, then redraws the game.
*/
void credits()
{
  drawCreditsScreen();
  while (!m5.BtnB.wasReleased())
  {
    M5.update();
  }
  drawBottomMenu();
  drawGameScreen();
}