/***********************************************************************
 * Created by Peter Harrison on 30/12/2017.
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

#include "sensors.h"
#include "parameters.h"
#include "hardware/hardware.h"

bool sensorsEnabled;
// sensor values
volatile int sensFR;
volatile int sensFL;
volatile int sensR;
volatile int sensL;

volatile int frontSum;
volatile int frontDiff;

int rawFR;
int rawFL;
int rawR;
int rawL;


volatile int sensorState;


// sensor wall detection
volatile bool wallSensorRight;
volatile bool wallSensorLeft;
volatile bool wallSensorFrontRight;
volatile bool wallSensorFrontLeft;
volatile bool wallSensorFront;

const PROGMEM int frontSumTable[] = {
  311,  311,  309,  309,  307,  306,  305,  304,  302,  300,  299,  297,  295,  293,  290,  287,  280,  277,  268,  259,  254,  246,  241,  231,  226,  220,  214,  206,  203,  197,  193,  189,  183,  181,  175,  173,  168,  165,  162,  158,  154,  151,  148,  147,  145,  140,  139,  136,  133,  132,  129,  128,  125,  124,  121,  120,  117,  116,  115,  113,  112,  111,  110,  107,  107,  105,  103,  103,  102,  100,   99,   99,   96,   97,   95,   94,   94,   92,   92,   91,   90,   90,   88,   88,   87,   86,   86,   86,   84,   84,   84,   83,   82,   81,   82,   81,   81,   80,   79,   79,   78,   78,   77,   77,   77,   76,   75,   75,   75,   74,   75,   73,   73,   73,   73,   72,   72,   71,   71,   71,   70,   70,   69,   69,   69,   69,   69,
};

const PROGMEM int frontDiffTable[] = {
  7,    7,    7,    7,    7,    6,    7,    6,    6,    6,    5,    5,    5,    5,    4,    3,    0,   -3,   -4,   -5,   -4,   -4,   -5,   -3,   -4,   -2,   -4,   -2,   -3,   -1,   -1,   -3,   -1,   -1,   -1,   -1,    0,   -1,    0,    0,    0,    1,    0,    1,    1,    2,    1,    2,    1,    2,    3,    2,    3,    2,    3,    2,    3,    2,    3,    3,    2,    3,    2,    3,    3,    3,    3,    3,    4,    4,    3,    3,    4,    3,    3,    4,    4,    4,    4,    5,    4,    4,    4,    4,    5,    4,    4,    4,    4,    4,    4,    5,    4,    5,    4,    5,    5,    4,    5,    5,    4,    4,    5,    5,    5,    4,    5,    5,    5,    4,    5,    5,    5,    5,    5,    4,    4,    5,    5,    5,    4,    4,    5,    5,    5,    5,    5,
};

void sensorsInit() {
  sensorState = 0;
}

/***
 * Sensor reading takes 8 states.
 * The initial state of zero is only entered once and ensures all emitters off.
 * Time needed per state is between 15 and 30 us and the update is called at
 * a frequency of 4kHz so the load is between about 5% and 10%. Quite high.
 * It looks like some channels are slower to read than others!
 *
 * Note that the ADC hardware is significantly speeded up in hardwareInit();
 *
 * With this scheme, sensors are read only 500 times per second. That is fine for a
 * slow robot but significant improvements will be needed if speed increases.
 *
 * sensorUpdate takes about 600us to run and it could be made more efficient by
 * use of a state machine or by only calling it when the answers are needed
 *
 * All told, reading the sensors takes about 25% of the available processor time.
 *
 * Also, time wasted here robs the processor of valuable power for maze flooding
 * and control tasks.
 */

void sensorUpdate() {
  // gather all the unlit readings
  int darkL = analogRead(LEFT_DIAG);
  int darkR = analogRead(RIGHT_DIAG);
  int darkFL = analogRead(LEFT_FRONT);
  int darkFR = analogRead(RIGHT_FRONT);
  digitalWrite(LED_TX_RF, 1);	// front LEDs on
  digitalWrite(LED_TX_LF, 1);
  delayMicroseconds(50);
  int litFL = analogRead(LEFT_FRONT);
  int litFR = analogRead(RIGHT_FRONT);
  digitalWrite(LED_TX_RF, 0);	// front LEDs off
  digitalWrite(LED_TX_LF, 0);
  digitalWrite(LED_TX_RD, 1);	// side LEDs on
  digitalWrite(LED_TX_LD, 1);
  delayMicroseconds(50);
  int litL = analogRead(LEFT_DIAG);
  int litR = analogRead(RIGHT_DIAG);
  digitalWrite(LED_TX_RD, 0);	// side LEDs off
  digitalWrite(LED_TX_LD, 0);

  // all the dark and lit values have now been collected so
  // see what we got from reflection but never accept negative readings
  rawFL = max(litFL - darkFL, 0);
  rawFR = max(litFR - darkFR, 0);
  rawL = max(litL - darkL, 0);
  rawR = max(litR - darkR, 0);

  // now calculate the normalised readings
  sensFL = (rawFL * LF_NOMINAL) / LF_CAL;
  sensFR = (rawFR * RF_NOMINAL) / RF_CAL;
  sensL = (rawL * LD_NOMINAL) / LD_CAL;
  sensR = (rawR * RD_NOMINAL) / RD_CAL;

  frontSum = sensFL + sensFR;
  frontDiff = sensFL - sensFR;
  // there is some hysteresis built in to the side sensors to ensure
  // cleaner edges
  // decide whether walls are present - use both sensors at the front

  wallSensorFrontRight = sensFR > FRONT_THRESHOLD;
  wallSensorFrontLeft = sensFL > FRONT_THRESHOLD;
  wallSensorFront = wallSensorFrontRight && wallSensorFrontLeft;

  if (sensL < (DIAG_THRESHOLD)) {
    wallSensorLeft = false;
  } else  if (sensL > (DIAG_THRESHOLD + 5)) {
    wallSensorLeft = true;
  }
  if (sensR < (DIAG_THRESHOLD)) {
    wallSensorRight = false;
  } else if (sensR > (DIAG_THRESHOLD + 5)) {
    wallSensorRight = true;
  }

}



void sensorsEnable() {
  sensorsEnabled = true;
}

void sensorsDisable() {
  sensorsEnabled = false;
}

int sensorGetFrontDistance() {
  int i;
  for (i = 0; i < 127; i++) {
    if (frontSum >= (int)pgm_read_word_near(frontSumTable + i)) {
      break;
    }
  }
  return i;
}

int sensorGetFrontSteering(int distance) {
  int i = 0;
  if (distance >= 0 && distance < 127) {
    i = frontDiff - pgm_read_word_near(frontDiffTable + distance);
  }
  return i;
}
