#include <SPI.h>
#include "Ucglib.h"
#include "values.h"

// States of the buttons
int readBtnL = 0;
int readBtnR = 0;
int stateBtnL = 0;
int stateBtnR = 0;
int lastStateBtnL = LOW;
int lastStateBtnR = LOW;

// Debounce time for buttons
unsigned long lastDebounceTimeBtnL = 0;
unsigned long lastDebounceTimeBtnR = 0;

Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ _pinDisplayCD, /*cs=*/ _pinDisplayCS, /*reset=*/ _pinDisplayRST);

void setup() {
    delay(100);
    Serial.begin(9600);

    // Setup the LCD
    ucg.begin(UCG_FONT_MODE_TRANSPARENT);
    ucg.setFont(ucg_font_ncenR14_hr);
    ucg.clearScreen();

    // Display the welcome message
    reset();
}

// Resets all variables, blocks, ball and paddle to restart game play 
void reset() {
    ballX = _initBallX;
    ballY = _initBallY;
    oldBallX = _initBallX;
    oldBallY = _initBallY;
    xincrement = _initXIncrement;
    yincrement = _initXIncrement;
    ballInBlock = false;

    paddleX = _initPaddleX;
    paddleXOld = _initPaddleX;

    score = 0;
    continueGame = true;

    resetBlocks();
    welcome();
}

// Resets all the blocks 
void resetBlocks() {
    for(byte i = 0; i < _numBlocks; i++) {
        for(byte param = 0; param < _blockParams; param++) {
            blocks[i][param] = _initBlocks[i][param];
        }
    }
}

// Game play loop 
void  loop() {
    readBtnL = digitalRead(_pinBtnL);
    readBtnR = digitalRead(_pinBtnR);
  
    if (continueGame) {                                   // If the game is still in play
        drawFrame();                                      // Draw the frame
        movePaddle();                                     // Update the location of the paddle
        boolean paddleCollision = checkPaddleCollision(); // Determine if the ball has hit the paddle or block
        ballInBlock = checkBlockCollision();

        if (score == _numBlocks)                          // If the score is equivalent to the number of blocks, game is over
            winner();                                     // Display message to user
        else {                                            // The game is still in play
            if (paddleCollision || ballInBlock)           // Redraw screen to draw over any collisions
                drawFrame();
            delay(50);                                    // Slight delay
            continueGame = updatePos();                   // Update the position of the ball
        }
    }
    else {                                                // The game is over, the ball fell off the screen. Display message to user.
        gameOver();
    }
}

// Draw the frame with the ball, paddle and blocks
void drawFrame() {
    ucg.setColor(0, 0, 0);
    ucg.drawDisc(oldBallX, oldBallY, _ballRad, UCG_DRAW_ALL);                         // Draw over the old ball
    ucg.setColor(230, 0, 126);
    ucg.drawDisc(ballX, ballY, _ballRad, UCG_DRAW_ALL);                               // Draw the new ball
  
    ucg.setColor(0, 0, 0);
    ucg.drawRBox(paddleXOld, _paddleY, _paddleWidth, _paddleHeight, _paddleRoundness); // Draw over old paddle
  
    ucg.setColor(0xff, 0xff, 0xff);
    ucg.drawRBox(paddleX, _paddleY, _paddleWidth, _paddleHeight, _paddleRoundness);    // Draw new paddle
  
    for(int i = 0; i < _numBlocks; i++) {                                             // Draw the blocks
        if(blocks[i][0] == 1)                                                         // If still in play
            drawBlock(i);                                                             // Draw the block
    }
}

// Move the ball 
boolean updatePos() {
    if (ballX > 240 - _ballRad*2 || ballX < 0 + _ballRad*2) // If the ball hits the right or left of the screen
        xincrement = -xincrement;
  
    if(ballY > 320)                                         // Ball has hit the bottom. GAME OVER
        return false;
    else if (ballY < 0 + _ballRad*2)                        // If the ball is at top of the screen
        yincrement = -yincrement;
  
    oldBallX = ballX;                                       // Save the ball's current position
    oldBallY = ballY;
  
    ballX += xincrement;                                    // Update the ball's location
    ballY += yincrement;
   
    return true;
}

// Checks if the ball has hit the paddle 
boolean checkPaddleCollision() {
    int ballTop    = ballY - _ballRad;                            // Define some values for easy reference
    int ballBottom = ballY + _ballRad;
    int ballLeft   = ballX - _ballRad;
    int ballRight  = ballX + _ballRad;
    int paddleX1   = paddleX + _paddleWidth - 1;
    int middleOffset = 5;
  
    if (ballBottom >= _paddleY) {                                 // If the ball is hitting the TOP of the paddle
        if (ballX >= paddleX && ballX <= paddleX1) {              // If the ball has hit in between the left and right edge of paddle
            float paddleMidPt = (paddleX+_paddleWidth-1)/2;       // Determine paddle midpt to determine whether to flip x direction
            yincrement = -yincrement;                             // Flip y increment
            if (ballX <= paddleMidPt-middleOffset)                // If the ball's xloc is less than the paddle's mid point, flip the x direction
                xincrement = -xincrement; 
            return true;
        }
        if (ballRight >= paddleX && ballLeft < paddleX) {         // If the ball is hitting the LEFT of the paddle
            if (xincrement > 0)                                   // If it's coming from the left, flip x
                xincrement = -xincrement;
            yincrement = -yincrement;
            return true;
        }
        else if (ballLeft <= paddleX1 && ballRight > paddleX1) {  // If the ball is hitting the RIGHT of the paddle
            if (xincrement < 0)                                   // If it's coming from the right, flip x
                xincrement = -xincrement;
            yincrement = -yincrement;
            return true;
        }
    }
    return false;
}
// Checks if the ball has collided with a block 
boolean checkBlockCollision() {
    int ballTop = ballY-_ballRad;                                            // Values for easy reference
    int ballBottom = ballY+_ballRad;
    int ballLeft = ballX-_ballRad;
    int ballRight = ballX+_ballRad;
    
    for(int i=0; i<_numBlocks; i++) {                                        // Loop through the blocks
        if (blocks[i][0] == 1) {                                              // If the block hasn't been eliminated
            int blockX = blocks[i][1];                                       // Grab x and y location
            int blockY = blocks[i][2];
         
            if (ballBottom >= blockY && ballTop <= blockY+_blockHeight) {    // If hitting BLOCK
                if (ballRight >= blockX && ballLeft <= blockX+_blockWidth) {
                    // Only change y direction if horizontal collision
                    if (ballBottom >= blockY && ballTop < blockY)
                        yincrement = -yincrement;
                    else if (ballTop <= blockY+_blockHeight && ballBottom > blockY+_blockHeight)
                        yincrement = -yincrement;

                    // Only change x direction if lateral collision 
                    if (ballRight >= blockX && ballLeft < blockX && ballX > oldBallX)
                        xincrement = -xincrement;
                    else if (ballLeft <= blockX+_blockWidth && ballRight > blockX+_blockWidth && ballX < oldBallX)
                        xincrement = -xincrement;

                    if (ballInBlock) return true;   

                    --blocks[i][3];
                    drawBlock(i);
                    return true;
                }
            }
        }
    }
    return false;                                                           // No collision detected
}

// Draw blocks with the color corresponding to their strength
void drawBlock(int i) {
    switch (blocks[i][3]) {
        case 0:
            removeBlock(i);
            break;
        case 1:
            ucg.setColor(blockColors[0][0], blockColors[0][1], blockColors[0][2]);
            ucg.drawBox(blocks[i][1], blocks[i][2], _blockWidth, _blockHeight);
            break;
        case 2:
            ucg.setColor(blockColors[1][0], blockColors[1][1], blockColors[1][2]);
            ucg.drawBox(blocks[i][1], blocks[i][2], _blockWidth, _blockHeight);
            break;
        case 3:
            ucg.setColor(blockColors[2][0], blockColors[2][1], blockColors[2][2]);
            ucg.drawBox(blocks[i][1], blocks[i][2], _blockWidth, _blockHeight);
            break;
    }
}

// Removes a block from game play 
void removeBlock(int index) {
    ucg.setColor(0, 0, 0);
    ucg.drawBox(blocks[index][1], blocks[index][2], _blockWidth, _blockHeight); // Draw the block
    blocks[index][0] = 0;                                                       // Mark it as out of play
    score++;                                                                    // Increment score
    adjustSpeed();                                                              // Increment the speed of the ball
}

// Move the paddle 
void movePaddle() {
    paddleXOld = paddleX;                                           // Save the paddle's old location
    
    if (readBtnL != lastStateBtnL)                                  // Reset the debouncing timers
        lastDebounceTimeBtnL = millis();
    if (readBtnR != lastStateBtnR)
        lastDebounceTimeBtnR = millis();
  
    if ((millis() - lastDebounceTimeBtnL) > _debounceDelay) {
        if (readBtnL != stateBtnL)                                  // Check if the left button's state has changed
            stateBtnL = readBtnL;
        if (stateBtnL == HIGH)                                      // Move the paddle if the new button state is HIGH
            paddleX = max(0, paddleX - _paddleMove);
    }
  
    if ((millis() - lastDebounceTimeBtnR) > _debounceDelay) {
        if (readBtnR != stateBtnR)                                  // Check if the left button's state has changed
            stateBtnR = readBtnR;
        if (stateBtnR == HIGH)                                      // Move the paddle if the new button state is HIGH
            paddleX = min(240 - _paddleWidth, paddleX + _paddleMove);
    }
  
    lastStateBtnL = readBtnL;
    lastStateBtnR = readBtnR;
}

// Increments the speed of the ball
void adjustSpeed() {
    if (yincrement < 0)
        yincrement -= _ballSpeedUp;
    else
        yincrement += _ballSpeedUp;
      
    if (xincrement < 0)
        xincrement -= _ballSpeedUp;
    else
        xincrement += _ballSpeedUp;
}

// Draws a welcome message before game play begins
void welcome() {
    ucg.setFont(ucg_font_courB14_tf);
    ucg.setColor(255, 255, 255);
    ucg.setPrintPos(15,30);
    ucg.setPrintDir(0);
    ucg.print(_welcomeMessage);
    delay(2000);
  
    ucg.clearScreen();
    ucg.setFont(ucg_font_helvB18_tr);
    ucg.setColor(230, 0, 126);
    ucg.setPrintPos(55,160);
    ucg.setPrintDir(0);
    ucg.print(_gameTitle);
    delay(1500);
    ucg.clearScreen();
}

// Tell the player they won
void winner() {
    ucg.clearScreen();
    ucg.setFont(ucg_font_courB14_tf);
    ucg.setColor(255, 255, 255);
    ucg.setPrintPos(80,160);
    ucg.setPrintDir(0);
    ucg.print(_winnerMessage);
    delay(3000);

    ucg.clearScreen();
    reset();
}

// Start a new game 
void gameOver() {
    ucg.clearScreen();
    ucg.setFont(ucg_font_courB14_tf);
    ucg.setColor(255, 255, 255);
    ucg.setPrintPos(70,160);
    ucg.setPrintDir(0);
    ucg.print(_gameOverMessage);
    delay(3000);

    ucg.clearScreen();
    reset();
}

