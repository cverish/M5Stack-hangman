/*
  Clarissa Verish and Matthieu Lalonde
  sd_runner.cpp
  2020-08-25

  Contains the code needed to get a secret word from the SD card.
*/

#include <Arduino.h>
#include <M5Stack.h>
#include "SD.h"
#include "FS.h"
#include "hangman.h"
#include "main.h"

// filename for dictionary included on the M5Stack SD card
String filename = "clean_dict.txt";
File f;
// secret word selected for use in main file
char secretWord[MAX_WORD_LENGTH];

// getWord()
// called within main file when game is restarted
bool getWord()
{
  f = SD.open("/" + filename, FILE_READ);
  // clearing the secretWord array from the last word played
  for (uint8_t i = 0; i < MAX_WORD_LENGTH; i++)
  {
    secretWord[i] = '\0';
  }
  // selecting a random position within the file
  uint16_t randInt = random(0, f.size() - MAX_WORD_LENGTH - 1);
  // initializing character array
  uint8_t c[1] = {0};
  // seek to the random number
  f.seek(randInt);
  // look for the first return character to select the next word
  // incrementing randInt every time we do not find a return character
  while (c[0] != '\n')
  {
    if (!f.seek(randInt))
    {
      f.close();
      return false;
    }
    f.read(c, 1);
    randInt++;
  }
  // setting the first position of the array to the new randInt number
  uint16_t first = randInt;
  uint16_t count = 0;
  // adding to the secretWord array until the next return character is found
  while (count < MAX_WORD_LENGTH)
  {
    if (!f.seek(first))
    {
      f.close();
      return false;
    }
    f.read(c, 1);
    if (c[0] != '\n')
    {
      secretWord[count] = c[0];
    }
    else
    {
      secretWord[count] = '\0';
      f.close();
      return true;
    }
    count++;
    first++;
  }
  f.close();
  return true;
}
