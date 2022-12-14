#include <nds.h>
#include <stdio.h>
#include <string>
#include <sstream>

// Custom stuff
#include "number_util.h"
#include "consts.h"

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

bool menuActions(touchPosition touchXY, int &coreVel, bool &ai) {
  // Speed decrease button bounds
  int dcrX = 80, dcrY = SCR_MAX_Y / 2, dcrS = 10;

  // Speed increase button bounds
  int incrX = 110, incrY = SCR_MAX_Y / 2, incrS = 10;

  // AI toggle bounds
  int aiX = 150, aiY = SCR_MAX_Y / 2, aiS = 30;

  // Decrease button check
  if (
      between(touchXY.px, dcrX, dcrX + dcrS) &&
      between(touchXY.py, dcrY, dcrY + dcrS))
  {
    coreVel--;
    return true;
  }

  // Increase button check
  if (
      between(touchXY.px, incrX, incrX + incrS) &&
      between(touchXY.py, incrY, incrY + incrS))
  {
    coreVel++;
    return true;
  }

  // AI toggle check
  if (
      between(touchXY.px, aiX, aiX + aiS) &&
      between(touchXY.py, aiY, aiY + aiS))
  {
    ai = !ai;
    return true;
  }

  return false;
}