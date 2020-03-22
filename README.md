# MacroStepper v2.0
Author: Cam Dey

Created: 2018-12-18
v2.0 added: 2020-01-01
Last Revised: 2020-03-22

Note: v1.0 documentation hasn't been updated to incorporate the TMC2130 driver or the other hardware changes.

The MacroStepper is a fully automated macrophotography rig designed to enable a camera to take photos at precisely incremented steps. The macroStack can work with or without an automated shutter and also in manual mode if the user prefers to control the stepping themselves.

# Schematic
## Fritzing Mockup
![PCB_top](https://github.com/camdey/MacroStepper/blob/new_pcb_layout/schematics/MacroStepper_v2.0_top.png)
![PCB_bottom](https://github.com/camdey/MacroStepper/blob/new_pcb_layout/schematics/MacroStepper_v2.0_bottom.png)

## Pinouts
**2.8" TFT Touch Screen Display**

 - Connected via custom PCB shield
 
| TMC2130     | PCB 		|
| ----------- | --------------- |
| VMOT        | 12V Power Supply|
| VIO         | Arduino 3.3V	|
| GND         | GND		|
| M1A         | Black wire	|
| M1B         | Green wire	|
| M2A         | Red wire        |
| M2B         | Blue wire	|
| DIR         | D28		|
| STEP        | D30		|
| CS          | D32		|
| MISO        | ICSP1		|
| SCK         | ICSP3		|
| MOSI        | ICSP4		|
| EN          | D34		|

| Joystick    | PCB	|
| ----------- | ------- |
| X Axis      | A6	|
| Z Axis      | A7	|

| Camera      | PCB 	|
| ----------- | ------- |
| GODOX       | A8	|
| SHT         | D36	|

| Misc        | PCB 	|
| ----------- | ------- |
| PIEZO       | D22	|


# Interface
**Home Screen**
 - House icon: _run homing sequence for the rail, finds min and max positions of the rail_
 - X icon: _clears the settings from the auto mode_
 
**Manual Screen**
 - Step Dist: _press to set distance per movement of the motor_
	 - _value is the distance per movement covered by the rail in millimetres_
 - Step Nr: _number of movements taken in manual mode_
 - Rail Pos: _current position of the camera on the rail_
 - Aperture icon: _press to enable/disable automated shutter_
 - X icon: _press to clear step count_
 - Arrows: _for increment/decrementing the Step Dist. if enabled or moving the rail forward/back otherwise_

**Auto Screen**
 - Step Dist: _press to set distance per movement of the motor_
	 - _value is the distance per movement covered by the rail in millimetres_
 - Est. Time: _estimated time remaining in autoStack procedure. Takes into account shutter delay and the recycle time of the flash unit_
 - Progress: _number of movements completed out of total number of movements in procedure_
 - Aperture icon: _press to enable/disable automated shutter_
 - Cog icon: _opens the config menu for the auto mode_
 - Play symbol: _used to increment Step Dist. if enabled, otherwise used for starting the autoStack procedure_
 - Pause symbol: _used to decrement Step Dist. if enabled, otherwise used for pausing the autoStack procedure_

**Config Screen**

 - START: _press to set the start position for the autoStack procedure. Will automatically change to current position when enabled, use joystick or arrows to move rail into correct position_
 - END: _press to set the end position for the autoStack procedure. Will automatically change to current position when enabled, use joystick or arrows to move rail into correct position_
 - RUN: _will move the camera rail to the START position, slowly increment through the autoStack range until it reaches the END, and then move back to the START position_
 - Clock symbol: _press to change the shutter delay between taking photos after each movement of the stepper motor_
 - Arrows: _used to increment/decrement shutter delay or move the position of the rail_


# Functionality
**Flash/Shutter Control**
The camera shutter is controlled by the Arduino via cable where a HIGH signal is sent to a transistor to short the Shutter and Focus pins of the Sony A6000 to ground. The duration of this signal is determined by an output signal from a PC sync cable. This cable is connected to the output of a Godox X1T-S transmitter. This transmitter sends a LOW signal for several milliseconds when the flash is ready to trigger. When the low signal is received by the Arduino, the shutter line goes LOW and the Shutter and Focus pins are returned to nominal state.

**Distance settings**
The step distance setting refers to the distance travelled by the rail per movement. A movement is the sequence of steps required by the stepper motor to travel the set distance. The default distance per step in my case was 25μm, calculated by the following: screw pitch / steps per revolution / gear reduction. My rail had a 1mm pitch and 1:2 gear reduction, while the motor takes 200 steps per revolution.

If the step dist. is set to 100μm, this equates to four steps at 25μm each. The sequence of these four steps is termed a movement in this project.

**AutoStack Procedure**
The autoStack procedure will increment a range by a given number of movements, determined by the step distance and the start and end positions determined by the user. With a 100μm step distance and 180 steps required to go from the start position to the end position, 45 (180/4) movements are required and thus 45 photos will be taken. The procedure can be paused or reset at any stage and after completion the procedure can be restarted. Both step distance and the end position can be changed during the procedure once paused.

# Macrophotography

## What is focus stacking?

Focus stacking is a method popular within the field of macrophotography where multiple photos of the same subject are taken at different focus depths and then merged together with software. This enables a macro lens with a shallow depth of field to produce an image of a subject that is fully in focus, even if the subject is physically longer than the depth of field of the lens.

## What do you need for Macrophotography?

While you can create a focus stack just with handheld photos taken at different distances from the subject, typically a **macro rail** is used to help with this process. A macro rail allows you to move a camera attached to a platform by very specific distances along a fixed rail or slide. By turning a handle or knob of the macro rail, the camera can be incremented along the rail by distances as small as 25μm or as large as the rail is long.

**Macro Rail**
For this project, you will need a macro rail that can be attached to a stepper motor. Many commercially available macro rails can be retrofitted with a pinion gear to accept a belt, which is in turn attached to a pinion gear on a stepper motor. Other macro rails are designed to interface directly with a stepper motor, especially those designed to be used for CNC machines or 3D printing.

**Stepper Motor**
A stepper motor is required to move the macro rail automatically. You can do this manually, and many do, but after awhile you get sick of the repetitive process of "move camera rail, take photo, move camera rail, take photo".

There are many types of stepper motors available but for this project I would recommend one in the NEMA 17 form factor with either 200 (1.8°) or 400 (0.9°) steps per revolution and between 1A and 2A current rating. The current rating is important for the load the stepper motor can move and hold but it is not the only determiner of this. A motor with a holding torque of at least 3kg-cm should be adequate.

The voltage of the motor is less important as you can supply more voltage than the rated voltage of the motor. More supply voltage typically equals a faster motor.

**Camera and Lens**
Any DSLR or mirrorless camera will be suitable. A macro lens of your choosing is also sufficient.

**Lighting**
I would recommend buying a flash for your camera, preferably wireless. Macrophotography often deals with very low amounts of light and high apertures so a flash is particularly useful. This project is designed to work with the shutter of a Sony multi-terminal port for triggering the shutter and a PC sync cable for detecting the output signal of the flash being triggered but either of these can be changed to the communication methods required for your camera and flash.
