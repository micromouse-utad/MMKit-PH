# MMKit-PH

This repository is a fork of [Peter's Harrison code](https://github.com/micromouseonline/ported-micromouse) for the [MMKit](https://www.micromouse.utad.pt/?page_id=998&lang=en). The repository was created to make it easier to run Peter's code using the Arduino IDE, as well as to help in the calibration processes needed to get the micromouse runing correctly.

## The Kit



<table>
  <tr>
    <th>Label</th>
    <th>Component</th>
    <th rowspan="17"></th>
    <th>Image</th>
  <tr>
    <td>1</td>
    <td>Left IR Sensor</td>
    <td rowspan="16"><img src="https://www.micromouse.utad.pt/wp-content/uploads/2016/05/MicromouseKit-3.png" width=450</img></td>

  </tr>
  <tr>
    <td>2</td>
    <td>Forward Left IR Sensor</td>
  </tr>
  <tr>
    <td>3</td>
    <td>AA Batteries Support (4 – AA)</td>
  </tr>
  <tr>
    <td>4</td>
    <td>ON/OFF AA Batteries</td>
  </tr>
  <tr>
    <td>5</td>
    <td>Left Stepper Motor</td>
  </tr>
  <tr>
    <td>6</td>
    <td>Configurable Switches for Control Software</td>
  </tr>
  <tr>
    <td>7</td>
    <td>Connector for Right Motor</td>
  </tr>
  <tr>
    <td>8</td>
    <td>Right IR Sensor</td>
  </tr>
  <tr>
    <td>9</td>
    <td>Forward Right IR Sensor</td>
  </tr>
  <tr>
    <td>10</td>
    <td>Bluetootd Connector (module not included)</td>
  </tr>
  <tr>
    <td>11</td>
    <td>LED pin 13 / Buzzer (Buzzer not included)</td>
  </tr>
  <tr>
    <td>12</td>
    <td>LED ON/OFF</td>
  </tr>
  <tr>
    <td>13</td>
    <td>Jumper for Selection AA Batteries/LiPo Battery</td>
  </tr>
  <tr>
    <td>14</td>
    <td>Right Stepper Motor</td>
  </tr>
  <tr>
    <td>15</td>
    <td>ON/OFF LiPo Battery (Battery not included)    </td>
  </tr>
  <tr>
    <td>16</td>
    <td>Connector for Left Motor</td>
  </tr>
</table>

## Instalation

There's no instalation required, to run the code first download its [latest release](https://github.com/micromouse-utad/MMKit-PH/releases/latest). 
After, uncompressed the code in your a location of your choosing in your machine and just open the project throught the Arduino IDE.

## Calibration

The calibration is extremely important in this code, seeing that with a bad calibrated robot...

The calibration consists of 4 parts:
1. Side sensors raw calibration
2. Front sensors raw calibration
3. Front sensors sum/diff calibration
4. 

To help with this process a routine was created to help you calibrate the robot, you cal call it if connected through the UART by pressing the key 'c'.

#### Side sensors raw calibration

#### Front sensors raw calibration

#### Front sensors sum/diff calibration
