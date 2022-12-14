#include <nds.h>
#include <gl2d.h>

#include <stdio.h>
#include <string>
#include <sstream>

const int SCR_MAX_X = 256;
const int SCR_MAX_Y = 192;
const int PAD_SIZE_X = 5;
const int PAD_SIZE_Y = 60;
const int BALL_SIZE = 10;
const int CONSOLE_WIDTH = SCR_MAX_X / 2;
const int CONSOLE_T_WIDTH = 32;

volatile int frame = 0;

void Vblank() {
	frame++;
}

bool between(int num, int min, int max) {
	return min <= num && num <= max;
}

u32 kdr() {
	return keysHeld();
}

/* Controls section */
bool leftUp() {
	return kdr() & KEY_UP;
}

bool leftDown() {
	return kdr() & KEY_DOWN;
}

bool rightUp() {
	return kdr() & KEY_X;
}

bool rightDown() {
	return kdr() & KEY_Y;
}

// https://www.tutorialspoint.com/determining-how-many-digits-there-are-in-an-integer-in-cplusplus
int digits(int i) {
	int count = 1;
   while(i > 9) {
      i = i / 10;
      count++;
   }
   return count;
}

// Game
int main(void) {
	int playPosX = 10, playPosY = 0;
	int opPosX = SCR_MAX_X - 10, opPosY = 0;
	int ballX = 0, ballY = 0;
	int leftScore = 0;
	int rightScore = 0;

	// Spacing between score prints
	int scrSpc = CONSOLE_WIDTH - 2;

	// Paddle movement speed
	int paddleVel = 2;

	// Core velocity is just the speed
	int coreVel = 2;

	// Velocity starts as down and towards the player
	int ballVelX = -coreVel, ballVelY = coreVel;

	// Score and menu change flags, used to reprint console
	bool scrChange = false, menuChange = false;

	// Store touch position
	// touchPosition touchXY;
	irqSet(IRQ_VBLANK, Vblank);

	videoSetMode(MODE_5_3D);
	videoSetModeSub(MODE_0_2D);

	// Init consoles
	PrintConsole scoreConsole;
	consoleInit(&scoreConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

	PrintConsole menuConsole;
	consoleInit(&menuConsole, 1, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
	
	// Draws right at the top of the bottom screen
	consoleSetWindow(&scoreConsole, SCR_MAX_X / 4, 0, CONSOLE_WIDTH, 16);

	// Draws in the middle of the bottom screen
	consoleSetWindow(&menuConsole, SCR_MAX_X / 4, SCR_MAX_Y / 2, CONSOLE_WIDTH, 16);

	// initialize gl2d
	glScreen2D();

	// Start the ball at the middle of the screen
	ballX = SCR_MAX_X / 2;
	ballY = SCR_MAX_Y / 2;

	// Game loop
	while(true) {
		int dig = digits(leftScore) + digits(rightScore);
		scrSpc = CONSOLE_T_WIDTH - dig;

		glBegin2D();

		// If the score has changed, trigger a referesh
		if (scrChange) {
			consoleSelect(&scoreConsole);
			consoleClear();

			std::stringstream scorePrint;
			scorePrint << leftScore << std::string(scrSpc, ' ') << rightScore;

			iprintf(scorePrint.str().c_str());

			scrChange = false;
		}

		if (menuChange) {
			consoleSelect(&menuConsole);
			consoleClear();
			iprintf("This is the menu console");
		}


		// touchRead(&touchXY);

		// Get inputs
		scanKeys();

		// Button inputs
		if (leftUp()) {
			playPosY -= paddleVel;
		}

		if (leftDown()) {
			playPosY += paddleVel;
		}

		if (rightUp()) {
			opPosY -= paddleVel;
		}

		if (rightDown()) {
			opPosY += paddleVel;
		}

		// // Prevent paddle resetting 
		// if (touchXY.py != 0)
		// 	playPosY = touchXY.py;

		// // Prevent paddle going offscreen
		// if (playPosY > SCR_MAX_Y - PAD_SIZE_Y)
		// 	playPosY = SCR_MAX_Y - PAD_SIZE_Y;

		// Draw player
		glBoxFilled(playPosX, 0 + playPosY, playPosX + PAD_SIZE_X, PAD_SIZE_Y + playPosY, 999999);

		// Draw opponent
		glBoxFilled(opPosX, 0 + opPosY, opPosX + PAD_SIZE_X, PAD_SIZE_Y + opPosY, 999999);

		// Draw ball
		glBoxFilled(ballX, ballY, ballX + BALL_SIZE, ballY + BALL_SIZE, 999999);

		// Update ball position
		ballX += ballVelX;
		ballY += ballVelY;

		// Win conditions
		if (ballX <= 0) {
			rightScore++;

			// Reset ball
			ballX = SCR_MAX_X / 2;
			ballY = SCR_MAX_Y / 2;

			scrChange = true;
		} else if (ballX + BALL_SIZE >= SCR_MAX_X) {
			leftScore++;
			
			// Reset ball
			ballX = SCR_MAX_X / 2;
			ballY = SCR_MAX_Y / 2;

			scrChange = true;
		}

		// Ball bouncing
		if (ballY <= 0) {
			// Positive coreVel, since we bounce off the top
			ballVelY = coreVel;
		}

		if (ballY + BALL_SIZE >= SCR_MAX_Y) {
			// Positive coreVel, since we bounce off the bottom
			ballVelY = -coreVel;
		}

		// We technically don't need screen bound bouncing on the X axis, since the paddles will be doing that, but it's good for testing
		if (ballX <= 0 || (
			between(ballX, playPosX, playPosX + PAD_SIZE_X) &&
			between(ballY + BALL_SIZE, playPosY, playPosY + PAD_SIZE_Y)
		)) {
			ballVelX = coreVel;
		}

		if (ballX + BALL_SIZE >= SCR_MAX_X || (
			between(ballX + BALL_SIZE, opPosX, opPosX + PAD_SIZE_X) &&
			between(ballY + BALL_SIZE, opPosY, opPosY + PAD_SIZE_Y)
		)) {
			ballVelX = -coreVel;
		}

		glEnd2D();
		glFlush(0);

		swiWaitForVBlank();
	}
}
