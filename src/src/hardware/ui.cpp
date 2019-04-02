/***********************************************************************
 * Created by Peter Harrison on 22/12/2017.
 * Copyright (c) 2017 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/


#include "ui.h"
#include "hardware.h"
#include "streaming.h"
#include "mouse.h"
#include "../../maze.h"
#include "../../sensors.h"
#include "../../test.h"
#include "../../parameters.h"

// taken from the CATERINA bootloader - run it at least 10kHz
static volatile unsigned int LLEDPulse;
void breathePin(int pin) {
  LLEDPulse += 1;
  unsigned char p = LLEDPulse >> 8;
  if (p > 127) {
    p = 254 - p;
  }
  if (((unsigned char)LLEDPulse) >= p) {
    digitalWrite(pin, 0);
  } else  {
    digitalWrite(pin, 1);
  }
}

bool occludedLeft() {
  return (sensFL > SENSOR_OCCLUDED_LEVEL) && (sensFR < SENSOR_OCCLUDED_LEVEL);
}
bool occludedRight() {
  return (sensFL < SENSOR_OCCLUDED_LEVEL) && (sensFR > SENSOR_OCCLUDED_LEVEL);
}

int waitForStart() {
  int choice = 0;
  while (choice == 0) {
    int time = 0;
    breathePin(RED_LED);
    if (buttonPressed()) {
      while (buttonPressed()) {
        breathePin(GREEN_LED);
      }
      break;
    }
    while (occludedRight()) {
      digitalWrite(RED_LED, 1);
      delay(10);
      time++;
    }
    if (time > 5) {
      choice = RIGHT;
      break;
    }

    while (occludedLeft()) {
      digitalWrite(GREEN_LED, 1);
      delay(10);
      time++;
    }
    if (time > 5) {
      choice = LEFT;
      break;
    }
  }
  delay(200);
  digitalWrite(GREEN_LED, 0);
  digitalWrite(RED_LED, 0);
  return choice;
}

enum {
  RELEASED,
  PRESSING,
  PRESSED,
  RELEASING
};


/***
 * Debouncing here is a simple state machine for one pin. The debounce time is in
 * system ticks For the PORTED mouse these are at 250Hz. The pin must stay in the
 * same state for two states so, for a 24ms debounce time, the number of debounceTicks
 * will be 3 per state
 *
 * For good quality buttons, small debounce times will probably be reliable and will give
 * good, short response times. Poor quality switches or mechanical sensors like bumpers
 * are likely to require longer debounce times but that will make them less responsive.
 *
 * The pin state can be active high or low and defaults to active low. This would be a pin
 * that is pulled up by a resistor and shorted to ground when the button is pressed.
 *
 * The pressed() and released() functions use read() to get the instantaneous state of
 * the pin.
 *
 * Note that pressed() and released() are not logical opposites since, at the time
 * they get called, the button may be changing state. It is not safe to substitute
 * pressed(BUTTON) with !released(BUTTON).
 *
 * However, for simple use, it is generally acceptable use pressed() and !pressed().
 *
 * To make PRESSED and RELEASED the only two exported states would require persistent
 * storage of state for the button and this is not needed for a simple, one button solution.
 *
 * In normal use the user code might call pressed() to test a pin state and act if it
 * returns true. This is how buttons most commonly work:
 *
 * if (pressed(BUTTON)){
 * 		callFunction();
 * 		while(!released(BUTTON)
 * }
 *
 * However, for something like starting a mouse in the maze or for a test, it is not a good
 * idea to have the action begin until the button is released. That can be achieved like this:
 *
 * if (pressed(BUTTON)){
 * 		while(!released(BUTTON)){};
 * 		callFunction();
 * }
 *
 * Or, if more convenient:
 *
 * waitForClick(BUTTON);
 *
 *
 */

static volatile unsigned char pinState = RELEASED;
static const unsigned char debounceTicks = 3;


// return 1 if the button is in the active state else return 0
inline static int read(byte pin, int activeState = LOW) {
  return digitalRead(pin) == activeState;
}

void debouncePin(unsigned char pin) {
  static unsigned char ticks = 0;
  if (++ticks < debounceTicks) {
    return;
  }
  ticks = 0;
  switch (pinState) {
    case RELEASED:
      if (read(pin)) {
        pinState = PRESSING;
      } else {
        pinState = RELEASED;
      }
      break;
    case PRESSING:
      if (read(pin)) {
        pinState = PRESSED;
      } else {
        pinState = RELEASED;
      }
      break;
    case PRESSED:
      if (read(pin)) {
        pinState = PRESSED;
      } else {
        pinState = RELEASING;
      }
      break;
    case RELEASING:
      if (read(pin)) {
        pinState = PRESSED;
      } else {
        pinState = RELEASED;
      }
      break;
  }
}

bool buttonPressed() {
  return (pinState == PRESSED);
}

bool buttonReleased() {
  return (pinState == RELEASED);
}

void waitForClick() {
  while (buttonReleased()) {};
  while (buttonPressed()) {};
}

void waitForKeyboardEnter() {
  console << F("Press Enter to continue...");
  console << endl;
  int data = 0;
  while(data != 13) { // Enter
    data = console.read();
  }
}


/***
 * just sit in a loop, flashing lights waiting for the button to be pressed
 */
void panic() {
  while (!buttonPressed()) {
    digitalWrite(GREEN_LED, 1);
    digitalWrite(RED_LED, 0);
    delay(50);
    digitalWrite(GREEN_LED, 0);
    digitalWrite(RED_LED, 1);
    delay(50);
  }
  digitalWrite(GREEN_LED, 1);
  digitalWrite(RED_LED, 1);
  while (buttonPressed()) {
    ; // do nothing
  }
  digitalWrite(GREEN_LED, 0);
  digitalWrite(RED_LED, 0);
}



void doButton() {
  digitalWrite(GREEN_LED, 1);
  while (buttonPressed()) {
  };
  delay(100);
  digitalWrite(GREEN_LED, 0);
  if (digitalRead(SEL1) == HIGH) {
    console.print(F("\nRunning...\n"));
    mouseRunMaze();
  } else {
    console.print(F("\nSearching...\n"));
    testSearcher(GOAL);
  }
  delay(200);
  console.write(':');
}


void doCLI() {
  console << F("---------------------") << endl;
  int data = console.read();
  delay(100); // enough time for a reasonably long input line to fill buffer
  switch (data) {
    case 'g':
      console << F("Searching...") << endl;
      testSearcher(GOAL);
      break;
    case 'G':
      console << F("Running...") << endl;
      mouseRunMaze();
      break;
    case 'c':
      console << F("Calibrate Sensors...") << endl;
      testCalibrateSensors();
      break;
    case 'C':
      console << F("Calibrate Front Sensors...") << endl;
      testCalibrateFrontSensors();
      break;
    case 'W':
    case 'w':
      printMazeWallData();
      break;
    case 'm':
      printMazePlain();
      break;
    case 'M':
      mazeFlood(GOAL);
      printMazeDirs();
      printMazeCosts();
      break;
    case 'r':
    case 'R':
      mazeFlood(GOAL);
      if (pathGenerate(0)) {
        console.println(F("\nSolution found"));
      }
      printMazeDirs();
      console.println((char*)(path));
      pathExpand(path);
      console.println((char*)(commands));
      break;
    case 's':
      printSensors();
      break;
    case 'S':
      printCurrentWalls(); 
      break;
    case 'p':
    case 'P':
      printMouseParameters();
      break;
    case 'x':
      console << F("Reset the maze to default empty state") << endl;
      mazeInit(emptyMaze);
      printMazePlain();
      mouseState = SEARCHING;
      break;
    case 'X':
      console << F("Reset the maze to Japan 2007 Finals") << endl;
      mazeInit(japan2007);
      printMazePlain();
      mouseState = SEARCHING;
      break;
    case 'i':
    case 'I':
      console << F("Mouse location: 0x") << _HEX(mouse.location) << endl;
      console << F("Mouse heading:    ") << _HEX(mouse.heading) << endl;
      break;
    case 'h':
    case 'H':
      printHelp();
      break;
    default:
      console.print(F("I received: "));
      console.print(data, DEC);
      console.print(' ');
      while (console.available()) {
        data = console.read();
        console.print(data, DEC);
        console.print(' ');
      }
      console.println();
  }

  console << endl << F(":");
  // in case console input has multiple characters, ditch the extras
  while (console.available()) {
    console.read();
  }
}


void printMouseParameters() {
  console << F("Ported Mouse parameters:") << endl;
  console << F("  Counts per 180mm:   ") << MM(180) << endl;
  console << F("  Counts per 360 deg: ") << DEG(360) << endl;
  console << F("  ######################################") << endl;
  console << F("  Left sensor calibration: ") << LD_CAL << endl;
  console << F("  Right sensor calibration: ") << RD_CAL << endl;
  console << F("  Front Left sensor calibration: ") << LF_CAL << endl;
  console << F("  Front Right sensor calibration: ") << RF_CAL << endl;
  console << F("  Front wall interference threshold: ") << FRONT_WALL_INTERFERENCE_THRESHOLD << endl;
  console << F("  Diagonal threshold: ") << DIAG_THRESHOLD << endl;
  console << F("  Front threshold: ") << FRONT_THRESHOLD << endl;
  console << F("  ######################################") << endl;
  console << F("  Goal: ") << GOAL << F(" (0x");
  console.print(GOAL, HEX);
  console << F(")") << endl;
}

void printSensors() {
  console << F("\nRaw Values:        ");
  console << _JUSTIFY(rawFL, 5);
  console << _JUSTIFY(rawL, 5);
  console << _JUSTIFY(rawR, 5);
  console << _JUSTIFY(rawFR, 5);
  console << endl;
  console << F("Normalized Values: ");
  console << _JUSTIFY(sensFL, 5);
  console << _JUSTIFY(sensL, 5);
  console << _JUSTIFY(sensR, 5);
  console << _JUSTIFY(sensFR, 5);
  console << endl;
}

// prints currently detected walls
void printCurrentWalls() {
  boolean walls_detected = false;
  if((sensFR > FRONT_THRESHOLD) && (sensFL > FRONT_THRESHOLD)) {
    console << sensFL << F(" __ ") << sensFR << endl;
    walls_detected = true;
  }
  else{
    console << F("     ") << endl;
  }

  if (sensL < (DIAG_THRESHOLD)) {
    console << F("     ");
  } else  if (sensL > (DIAG_THRESHOLD + 5)) {
    console << sensL << F("|  ");
    walls_detected = true;
  }


  if (sensR < (DIAG_THRESHOLD)) {
    console << F(" ") << endl;
  } else if (sensR > (DIAG_THRESHOLD + 5)) {
    console << F("|") << sensR << endl;
    walls_detected = true;
  }

  if(walls == false) console << F("No walls detected") << endl;
}


// simple formatting functions for printing maze costs
void printHex(unsigned char value) {
  if (value < 16) {
    console.print('0');
  }
  console.print(value, HEX);
}



/***
 * printing functions.
 * Code space can be saved here by not usingserial.print
 */

void printNorthWalls(int row) {
  for (int col = 0; col < 16;  col++) {
    unsigned char cell = row + 16 * col;
    console << 'o';
    if (hasWall(cell, NORTH)) {
      console << F("---");
    } else {
      console << F("   ");
    }
  }
  console << 'o' << endl;
}

void printSouthWalls(int row) {
  for (int col = 0; col < 16;  col++) {
    unsigned char cell = row + 16 * col;
    console << 'o';
    if (hasWall(cell, SOUTH)) {
      console << F("---");
    } else {
      console << F("   ");
    }
  }
  console << 'o' << endl;
}

void printMazePlain() {
  console.println();
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = static_cast<unsigned char>(row + 16 * col);
      if (hasExit(cell, WEST)) {
        console << F("    ");
      } else {
        console << F("|   ");
      }
    }
    console << '|' << endl;
  }
  printSouthWalls(0);
  console << endl;
}

void printMazeCosts() {
  console << endl;
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = static_cast<unsigned char>(row + 16 * col);
      if (hasExit(cell, WEST)) {
        console << ' ';
      } else {
        console << '|';
      }
      console << _JUSTIFY(cost[cell], 3);
    }
    console << '|' << endl;
  }
  printSouthWalls(0);
  console << endl;
}

static char dirChars[] = "^>v<*";
char dirLetters[] = "NESW";

void printMazeDirs() {
  console << endl;
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = row + 16 * col;
      
      if (hasWall(cell, WEST)) {
        console << '|';
      } else {
        console << ' ';
      }
      unsigned char direction = directionToSmallest(cell, NORTH);
      if (cell == GOAL) {
        direction = 4;
      }
      console << ' ' << dirChars[direction];
      //console << ' ';
    }
    console << '|' << endl;
  }
  printSouthWalls(0);
  console << endl;
}

void printMazeWallData() {
  console << endl;
  for (int row = 15; row >= 0; row--) {
    for (int col = 0; col < 16; col++) {
      int cell = row + 16 * col;
      printHex(walls[cell]);
      console << ' ';
    }
    console << endl;
  }
  console << endl;
}


void printHelp() {
  console << F("\nHelp Page for the console commands") << endl;
  console << F("\tg   - Start Test Search") << endl;
  console << F("\tG   - Start Run") << endl;
  console << F("\tc   - Start Sensors Calibration") << endl;
  console << F("\tC   - Start Front Sensors Calibration") << endl;
  console << F("\tw,W - Print Maze Walls") << endl;
  console << F("\tm   - Print Maze Walls Simple") << endl;
  console << F("\tM   - Print Maze Directions and Costs") << endl;
  console << F("\tr,R - Test Solution") << endl;
  console << F("\ts   - Print Sensors") << endl;
  console << F("\tS   - Print Current Walls") << endl;
  console << F("\tp,P - Print Mouse Parameters") << endl;
  console << F("\tx   - Reset Maze") << endl;
  console << F("\tX   - Reset Maze to Japan 2007 Finals") << endl;
  console << F("\ti,I - Print Mouse Location/Direction") << endl;
  console << F("\th,H - Print Help Page") << endl;
}
