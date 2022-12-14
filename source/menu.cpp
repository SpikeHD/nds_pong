#include <nds.h>
#include <stdio.h>
#include <string>
#include <sstream>

void drawScore(PrintConsole *scoreConsole, int leftScore, int rightScore, int spaces) {
  consoleSelect(scoreConsole);
	consoleClear();

	std::stringstream scorePrint;
	scorePrint << leftScore << std::string(spaces, ' ') << rightScore;

	iprintf(scorePrint.str().c_str());
}

void drawGameMenu(PrintConsole *menuConsole, int ballSpeed, bool ai) {
	consoleSelect(menuConsole);
	consoleClear();
  
  // Menu items
  iprintf("          Speed    AI?\n\n");
  iprintf("          < %d >    %s", ballSpeed, ai ? "on" : "off");
}