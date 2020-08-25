/*
  Clarissa Verish
  draw.cpp
  2020-08-25

  Contains the graphics functions for the Hangman game. 
*/

#include <Arduino.h>
#include <M5Stack.h>
#include "hangman.h"
#include "main.h"

// fonts used
const GFXfont *NORMAL_FONT = &FreeMono12pt7b;
const GFXfont *LARGE_FONT = &FreeMonoBold12pt7b;
const GFXfont *SMALL_FONT = &FreeMono9pt7b;

/*
*****************************************************************

                    FULL-SCREEN GRAPHICS

*****************************************************************
*/

/*
  drawSplashScreen()

  displays the game's splash screen when the device is turned on or reset button pressed.
*/
void drawSplashScreen()
{
    M5.Lcd.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setFont(LARGE_FONT);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.drawString("h a n g m a n", 160, 40);
    M5.Lcd.setFont(SMALL_FONT);
    drawHangmanSplash();
    M5.Lcd.drawString("press any button", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40);
}

/*
  drawInitScreen()

  sets the font to normal, clears the screen, and redraws the top and bottom menus.
  also resets the hangman outline to dark grey
*/
void drawInitScreen()
{
    M5.Lcd.setFont(NORMAL_FONT);
    M5.Lcd.fillScreen(BLACK);
    drawBottomMenu();
    drawTopMenu();
    drawHangmanOutline();
}

/*
  drawGameScreen()

  redraws the current state of the game.
*/
void drawGameScreen()
{
    M5.Lcd.setFont(NORMAL_FONT);
    M5.Lcd.fillScreen(BLACK);
    drawBottomMenu();
    drawTopMenu();
    drawHangmanOutline();
    drawHangman();
    drawCurrentGuesses();
    drawAlphabet();
}

/*
  drawWinLoseScreen(bool win)

  draws a win or lose screen, depending on whether win is true or false
*/
void drawWinLoseScreen(bool win)
{
    M5.Lcd.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    drawTopMenu();
    drawBottomWinLoseMenu();
    uint16_t color = GREEN;
    String message = "You win!";
    if (!win)
    {
        color = RED;
        message = "You lose.";
    }
    drawScreenTextBox(color, BLACK, message, "\n     Word:");
    M5.Lcd.print("     ");
    M5.Lcd.print(secretWord);
}

/*
  drawCreditsScreen()

  displays a text box that shows the credits for the game
*/
void drawCreditsScreen()
{
    M5.Lcd.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    drawTopMenu();
    drawBottomCreditsMenu();
    drawScreenTextBox(TFT_DARKGREY, BLACK, "Credits", "      creator:\n      Clarissa Verish\n      contributor:\n      Matthieu Lalonde");
}

/*
  drawSdErrorScreen()

  displays a text box to notify the user that the dictionary cannot be read from the SD Card.
*/
void drawSdErrorScreen()
{
    drawScreenTextBox(RED, RED, "SD error", "      Cannot read dictionary\n      from SD card.");
}

/*
*****************************************************************

                    MENU BAR GRAPHICS

*****************************************************************
*/

/*
  drawTopMenu()

  Draws the top menu showing the total number of wins and losses, as well as the current streak.
*/
void drawTopMenu()
{
    uint16_t w = SCREEN_WIDTH;
    uint16_t buttonWidth = 88;
    uint16_t buttonHeight = 22;
    uint16_t leftMargin = 6;
    uint16_t spacing = 20;
    M5.Lcd.fillRect(0, 0, w, buttonHeight + 2, TFT_MAROON);
    drawMenuButton(leftMargin + ((spacing + buttonWidth) * 0), 0, buttonWidth, buttonHeight, TFT_MAROON, "wins:" + String(wins));
    drawMenuButton(leftMargin + ((spacing + buttonWidth) * 1), 0, buttonWidth, buttonHeight, TFT_MAROON, "lose:" + String(losses));
    drawMenuButton(leftMargin + ((spacing + buttonWidth) * 2), 0, buttonWidth, buttonHeight, TFT_MAROON, "strk:" + String(streak));
}

/*
  drawBottomMenu()

  Draws the bottom menu showing the menu buttons - New, forfeit, and menu - at the specified x,y coordinates
*/
void drawBottomMenu()
{
    int x = BOTTOM_XCOOR;
    int y = BOTTOM_YCOOR;
    uint16_t buttonWidth = 88;
    uint16_t buttonHeight = 22;
    uint16_t leftMargin = 16;
    uint16_t spacing = 10;
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 0), y, buttonWidth, buttonHeight, TFT_MAROON, "NEW");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 1), y, buttonWidth, buttonHeight, TFT_MAROON, "FORFEIT");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 2), y, buttonWidth, buttonHeight, TFT_MAROON, "CREDITS");
}

/*
  drawBottomFinishMenu()

  Draws the bottom menu when the player wins or loses - shows different options than normal menu
*/
void drawBottomWinLoseMenu()
{
    int x = BOTTOM_XCOOR;
    int y = BOTTOM_YCOOR;
    uint16_t buttonWidth = 88;
    uint16_t buttonHeight = 22;
    uint16_t leftMargin = 16;
    uint16_t spacing = 10;
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 0), y, buttonWidth, buttonHeight, TFT_MAROON, "NEW");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 1), y, buttonWidth, buttonHeight, TFT_MAROON, "CLEAR");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 2), y, buttonWidth, buttonHeight, TFT_MAROON, "CREDITS");
}

/*
  drawBottomCreditsMenu()
  
  Draws the bottom menu when the credits menu is showing
*/
void drawBottomCreditsMenu()
{
    int x = BOTTOM_XCOOR;
    int y = BOTTOM_YCOOR;
    uint16_t buttonWidth = 88;
    uint16_t buttonHeight = 22;
    uint16_t leftMargin = 16;
    uint16_t spacing = 10;
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 0), y, buttonWidth, buttonHeight, BLACK, "");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 1), y, buttonWidth, buttonHeight, TFT_MAROON, "CLOSE");
    drawMenuButton(x + leftMargin + ((spacing + buttonWidth) * 2), y, buttonWidth, buttonHeight, BLACK, "");
}

/*
*****************************************************************

                    GAME COMPONENT GRAPHICS

*****************************************************************
*/

/*
  drawCurrentGuesses()

  Draws the letter boxes at the bottom showing the characters in the secret word.

  If the letter has not been guessed, shows an empty box with a light grey outline.
  If it has been guessed, shows the letter with a green outline.

*/
void drawCurrentGuesses()
{
    // initializing the character as empty
    char c = ' ';
    // setting the inital color for the outline as light grey
    uint16_t color = TFT_LIGHTGREY;
    // calculating the initial x-coordinate for the first box - responsive based on length of word
    int x = SCREEN_WIDTH / 2 - 0.5 * (strlen(secretWord) * 30) + SIDE_MARGIN;
    // looping through the secretWord and guesses array
    for (int i = 0; i < MAX_WORD_LENGTH; i++)
    {
        // if the currentLetter is in the secretWord, set that slot in guesses to the current letter
        if (secretWord[i] == currentLetter)
        {
            guesses[i] = currentLetter;
        }
        // if the current character in secretWord has been guessed, display the letter in the box and set the outline to green.
        if (secretWord[i] == guesses[i])
        {
            c = secretWord[i];
            color = TFT_GREEN;
        }
        // if the current character has not been guessed, do not display the letter and set the outline to light grey.
        else
        {
            color = TFT_LIGHTGREY;
        }
        // if the character in secretWord at this index exists, draw a new letter box
        if (secretWord[i] != 0)
        {
            // letter box changes x coordinate based on the index of the letter
            drawLargeLetterBox(c, x + 30 * i, WORDBOX_YCOOR, BLACK, WHITE, color);
        }
        // resets the character to empty before the next loop
        c = ' ';
    }
}

/*
  drawAlphabet()

  Draws the alphabet showing guessed and unguessed letters.

  Cases:
  Letter is unguessed: letter and outline are white.
  Current letter is correct: letter and outline are green.
  Current letter is incorrect: letter and outline are green.
  Letter has been guessed: letter and outline are dark grey.
*/
void drawAlphabet()
{
    int x = KEYBOARD_XCOOR;
    int y = KEYBOARD_YCOOR;
    // setting the width of the alphabet
    int alphabet_width = 7;
    // initializing text and border colors to white
    uint16_t textColor = WHITE;
    uint16_t borderColor = WHITE;
    // tracks the current line for the letters
    uint8_t line = 0;
    // looping through letters a-z
    for (char c = 'a'; c <= 'z'; c++)
    {
        // creating an index for the letter to find it in the guessedLetters array
        int i = c - 'a';
        // if the letter has been guessed, set the text and background to dark grey
        if (guessedLetters[i] == c)
        {
            textColor = TFT_DARKGREY;
            borderColor = TFT_DARKGREY;
        }
        else
        {
            textColor = WHITE;
            borderColor = WHITE;
        }
        // if the letter in the loop is the currently selected letter
        if (c == currentLetter)
        {
            // creating a boolean to track whether the letter has been found or not
            bool found = false;
            // looping through the secret word array
            for (int j = 0; j < MAX_WORD_LENGTH; j++)
            {
                // if the secret word letter is \0, end of secret word has been reached and loop broken.
                if (secretWord[j] == '\0')
                {
                    break;
                }
                // if the current character is in the secret word, found set to true and loop broken.
                if (c == secretWord[j])
                {
                    found = true;
                    break;
                }
            }
            // if the letter has been found, the letter is correct. set the text and border color to green.
            if (found)
            {
                textColor = GREEN;
                borderColor = GREEN;
            }
            // if it was not found, the letter is incorrect. set the text and border color to red.
            else
            {
                textColor = RED;
                borderColor = RED;
            }
        }
        // draw a small letter box for the current character in the correct x,y location with the specified colors.
        drawSmallLetterBox(c, x + (SMALL_BOX_WIDTH + 6) * (i % alphabet_width), y + (SMALL_BOX_HEIGHT + 6) * line, textColor, BLACK, borderColor);
        // once the end of the line has been reached, increment the line number to move the next letter to the next line.
        if ((i + 1) % alphabet_width == 0)
        {
            line++;
        }
    }
}

/*
  drawHangman()

  draws the current state of the hangman based on the number of incorrect guesses
*/
void drawHangman()
{
    int x = HANGMAN_XCOOR;
    int y = HANGMAN_YCOOR;
    uint16_t color = HANGMAN_FILL_COLOR;
    switch (numWrong)
    {
    case 1: // base
        M5.Lcd.fillRect(x, y + 94, 96, 12, color);
        break;
    case 2: // post
        M5.Lcd.fillRect(x + 76, y + 11, 8, 80, color);
        break;
    case 3: // support
        M5.Lcd.fillRect(x + 20, y, 76, 8, color);
        break;
    case 4: // rope
        M5.Lcd.fillRect(x + 41, y + 11, 4, 11, color);
        break;
    case 5: // head
        M5.Lcd.fillEllipse(x + 43, y + 32, 8, 8, color);
        break;
    case 6: // body
        M5.Lcd.fillRect(x + 38, y + 42, 10, 22, color);
        break;
    case 7: // left leg
        M5.Lcd.fillRect(x + 38, y + 66, 4, 18, color);
        break;
    case 8: // right leg
        M5.Lcd.fillRect(x + 44, y + 66, 4, 18, color);
        break;
    case 9: // left arm
        M5.Lcd.fillRect(x + 28, y + 42, 8, 4, color);
        break;
    case 10: // right arm
        M5.Lcd.fillRect(x + 50, y + 42, 8, 4, color);
        break;
    }
}

/*
  drawhangmanOutline()

  draws a new, empty hangman outline 
*/
void drawHangmanOutline()
{
    int x = HANGMAN_XCOOR;
    int y = HANGMAN_YCOOR;
    uint16_t color = HANGMAN_OUTLINE_COLOR;
    M5.Lcd.fillRect(x, y + 94, 96, 12, color);
    M5.Lcd.fillRect(x + 76, y + 11, 8, 80, color);
    M5.Lcd.fillRect(x + 20, y, 76, 8, color);
    M5.Lcd.fillRect(x + 41, y + 11, 4, 11, color);
    M5.Lcd.fillEllipse(x + 43, y + 32, 8, 8, color);
    M5.Lcd.fillRect(x + 38, y + 42, 10, 22, color);
    M5.Lcd.fillRect(x + 38, y + 66, 4, 18, color);
    M5.Lcd.fillRect(x + 44, y + 66, 4, 18, color);
    M5.Lcd.fillRect(x + 28, y + 42, 8, 4, color);
    M5.Lcd.fillRect(x + 50, y + 42, 8, 4, color);
}

/*
  drawhangmanSplash()

  draws a new, empty hangman outline at the specified x,y coordinates
*/
void drawHangmanSplash()
{
    int x = (SCREEN_WIDTH - 100) / 2;
    int y = 70;
    uint16_t color = RED;
    M5.Lcd.fillRect(x, y + 94, 96, 12, color);
    M5.Lcd.fillRect(x + 76, y + 11, 8, 80, color);
    M5.Lcd.fillRect(x + 20, y, 76, 8, color);
    M5.Lcd.fillRect(x + 41, y + 11, 4, 11, color);
    M5.Lcd.fillEllipse(x + 43, y + 32, 8, 8, color);
    M5.Lcd.fillRect(x + 38, y + 42, 10, 22, color);
    M5.Lcd.fillRect(x + 38, y + 66, 4, 18, color);
    M5.Lcd.fillRect(x + 44, y + 66, 4, 18, color);
    M5.Lcd.fillRect(x + 28, y + 42, 8, 4, color);
    M5.Lcd.fillRect(x + 50, y + 42, 8, 4, color);
}

/*
*****************************************************************

                    GENERIC GRAPHIC HELPERS

*****************************************************************
*/

/*
  drawScreenTextBox(borderColor, textColor, message1, message2)

  Creates a generic full-screen text box with the indicated border color and text color.
  Adds the message to the text box.
*/
void drawScreenTextBox(uint16_t borderColor, uint16_t textColor, String message1, String message2)
{
    M5.Lcd.fillRect(20, 30, 280, 180, borderColor);
    M5.Lcd.fillRect(40, 50, 240, 140, WHITE);
    M5.Lcd.setCursor(56, 80);
    M5.Lcd.setFont(LARGE_FONT);
    M5.Lcd.setTextColor(textColor);
    M5.Lcd.println(message1);
    M5.Lcd.setFont(SMALL_FONT);
    M5.Lcd.println(message2);
}

/*
  drawLetterBoxWithSize(c, x, y, textColor, backgroundColor, borderColor, bwidth, bheight)

  Creates a generic letter box with width bwidth and height bheight.
  Letter box has indicated backgroundColor, textColor, and borderColor.
  Letter box is placed at coordinates x and y.
  Letter box displays indicated character (if letter box is to display no character, c should be set equal to ' ')
*/
void drawLetterBoxWithSize(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor, uint8_t bwidth, uint8_t bheight)
{
    M5.Lcd.fillRect(x, y, bwidth, bheight, backgroundColor);
    M5.Lcd.drawRect(x, y, bwidth, bheight, borderColor);
    M5.Lcd.setCursor(x + (bwidth / 5), y + 3.75 * (bheight / 5));
    M5.Lcd.setTextColor(textColor, backgroundColor);
    M5.Lcd.print(c);
}

/*
  drawLetterBox(c, x, y, textColor, backgroundColor, borderColor)

  creates a large letter box used at the bottom for the secret word letters.
*/
void drawLargeLetterBox(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor)
{
    M5.Lcd.setFont(LARGE_FONT);
    drawLetterBoxWithSize(c, x, y, textColor, backgroundColor, borderColor, LARGE_BOX_WIDTH, LARGE_BOX_HEIGHT);
    M5.Lcd.setFont(NORMAL_FONT);
}

/*
  drawSmallLetterBox(c, x, y, textColor, backgroundColor, borderColor)

  creates a small letter box used in the guessed and unguessed letter alphabet.
*/
void drawSmallLetterBox(char c, uint16_t x, uint16_t y, uint16_t textColor, uint16_t backgroundColor, uint16_t borderColor)
{
    M5.Lcd.setFont(SMALL_FONT);
    drawLetterBoxWithSize(c, x, y, textColor, backgroundColor, borderColor, SMALL_BOX_WIDTH, SMALL_BOX_HEIGHT);
    M5.Lcd.setFont(NORMAL_FONT);
}

/*
  drawMenuButton(x, y, w, h, color, s)

  draws a menu button at the specified x,y coordinates.
  Box width w and height h, with background color, and displays a string in light grey text. 
*/
void drawMenuButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, String s)
{
    M5.Lcd.fillRoundRect(x, y, w, h + 2, 4, color);
    M5.Lcd.setCursor(x + 3 + (88 - s.length() * 12) / 2, y + 16);
    M5.Lcd.setFont(SMALL_FONT);
    M5.Lcd.setTextColor(TFT_LIGHTGREY);
    M5.Lcd.print(s);
    M5.Lcd.setFont(NORMAL_FONT);
}
