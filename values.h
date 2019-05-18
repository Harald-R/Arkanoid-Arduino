#ifndef _H_VALUES
#define _H_VALUES

// Constants
const int _pinBtnL = 4;
const int _pinBtnR = 2;

const int _pinDisplayRST = 8;
const int _pinDisplayCD = 9;
const int _pinDisplayCS = 10;

const unsigned long _debounceDelay = 50; // The debounce time for the buttons

const char *_gameTitle       = "ARKANOID";
const char *_welcomeMessage  = "Get ready to play...";
const char *_winnerMessage   = "You won!";
const char *_gameOverMessage = "Game over!";
const char *_scoreMessage    = "Score: ";

const int _ballRad = 6;
const float _ballSpeedUp = 0.09;

const float _initBallX = 20;
const float _initBallY = 250;
const float _initXIncrement = 5;
const float _initYIncrement = 5;
const int _initPaddleX = 64;
const int _paddleY = 310;
const int _paddleWidth = 40;
const int _paddleHeight = 10;
const int _paddleMove = 10;
const int _paddleRoundness = 4;

const int _numBlocks = 12;
const int _blockParams = 4;

const int _blockWidth = 40;
const int _blockHeight = 10;

const short _initBlocks[_numBlocks][_blockParams] = {
//  en,  xloc,   yloc, strength
    {1,   10,     20,     3},
    {1,   70,     20,     3},
    {1,   130,    20,     3},
    {1,   190,    20,     3},
    {1,   10,     40,     2},
    {1,   70,     40,     2},
    {1,   130,    40,     2},
    {1,   190,    40,     2},
    {1,   10,     60,     1},
    {1,   70,     60,     1},
    {1,   130,    60,     1},
    {1,   190,    60,     1}
};

// Variables
float ballX;
float ballY;
float oldBallX;
float oldBallY;
boolean ballInBlock;

float xincrement;
float yincrement;

int paddleX;
int paddleXOld;

int score;
boolean continueGame = true;

unsigned char blockColors[3][3] = {
    {255, 0, 0},   // strength 1
    {255, 255, 0}, // strength 2
    {0, 0, 255}    // strength 3
};

short blocks[_numBlocks][_blockParams] = {
//  en,  xloc,   yloc, strength
    {1,   10,     20,     3},
    {1,   70,     20,     3},
    {1,   130,    20,     3},
    {1,   190,    20,     3},
    {1,   10,     40,     2},
    {1,   70,     40,     2},
    {1,   130,    40,     2},
    {1,   190,    40,     2},
    {1,   10,     60,     1},
    {1,   70,     60,     1},
    {1,   130,    60,     1},
    {1,   190,    60,     1}
};

#endif
