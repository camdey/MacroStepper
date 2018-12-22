********* Macro Stepper v1.0 *********
*** AUTHOR: Cam Dey
*** DATE: 2018-12-022
**************************************

Arduino .cpp/.ino files for my Macro Stepper project.
The project is for controlling a stepper motor with an Arduino and incrementing it by steps at a specified distance.
This is to be used in-conjunction with a linear axis slide with a camera and macro lens to aid in focus stackking.

The automatic functionality includes being able to set START and END points so that the Arduino will step through the range automatically.
Both manual and automatic modes include functionality for triggering the camera shutter automatically. This can be disabled.
The stepper motor control will rely largely on the AccelStepper library.
