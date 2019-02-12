# macroStack v1.0
- Author: Cam Dey
- Created: 2018-12-18
- Last Revised: 2019-02-12

The macroStack is a fully automated macrophotography rig designed to enable a camera to take photos at precisely incremented steps. The macroStack can work with or without an automated shutter and also in manual mode if the user prefers to control the stepping themselves.

# Functionality
**Home Screen**
 - House icon: run homing sequence for the rail, finds min and max positions of the rail
 - X icon: clears the settings from the auto mode
 
**Manual Screen**
 - Step Dist: press to set distance per movement of the motor
	 - value is the distance per movement covered by the rail in millimetres
 - Step Nr: number of movements taken in manual mode
 - Rail Pos: current position of the camera on the rail
 - Aperture icon: press to enable/disable automated shutter
 - X icon: press to clear step count
 - Arrows: for increment/decrementing the Step Dist. if enabled or moving the rail forward/back otherwise

**Auto Screen**
 - Step Dist: press to set distance per movement of the motor
	 - value is the distance per movement covered by the rail in millimetres
 - Est. Time: estimated time remaining in autoStack procedure. Takes into account shutter delay and the recycle time of the flash unit
 - Progress: number of movements completed out of total number of movements in procedure
 - Aperture icon: press to enable/disable automated shutter
 - Cog icon: opens the config menu for the auto mode
 - Play symbol: used to increment Step Dist. if enabled, otherwise used for starting the autoStack procedure
 - Pause symbol: used to decrement Step Dist. if enabled, otherwise used for pausing the autoStack procedure

**Config Screen**

 - START: press to set the start position for the autoStack procedure. Will automatically change to current position when enabled, use joystick or arrows to move rail into correct position
 - END: press to set the end position for the autoStack procedure. Will automatically change to current position when enabled, use joystick or arrows to move rail into correct position
 - RUN: will move the camera rail to the START position, slowly increment through the autoStack range until it reaches the END, and then move back to the START position
 - Clock symbol: press to change the shutter delay between taking photos after each movement of the stepper motor
 - Arrows: used to increment/decrement shutter delay or move the position of the rail

# Macrophotography

## What is focus stacking?

Focus stacking is a method popular within the field of macrophotography where multiple photos of the same subject are taken at different focus depths and then merged together with software. This enables a macro lens with a shallow depth of field to produce an image of a subject that is fully in focus, even if the subject is physically longer than the depth of field of the lens.

## What do you need for Macrophotography?

While you can create a focus stack just with handheld photos taken at different distances from the subject, typically a **macro rail** is used to help with this process. A macro rail allows you to move a camera attached to a platform by very specific distances along a fixed rail or slide. By turning a handle or knob of the macro rail, the camera can be incremented along the rail by distances as small as 25μm or as large as the rail is long.

**Macro Rail**
For this project, you will need a macro rail that can be attached to a stepper motor. Many commercially available macro rails can be retrofitted with a pinion gear to accept a belt, which is in turn attached to a pinion gear on a stepper motor. Other macro rails are designed to interface directly with a stepper motor, especially those designed to be used for CNC machines or 3D printing.

**Stepper Motor**
A stepper motor is required to move the macro rail automatically. You can do this manually, and many do, but after awhile you get sick of the repetitive process of "move camera rail slightly, take photo, move camera rail, take photo".

There are many types of stepper motors available but for this project I would recommend one in the NEMA 17 form factor with either 200 (1.8°) or 400 (0.9°) steps per revolution and between 1A and 2A current rating. The current rating is important for the load the stepper motor can move and hold but it is not the only determiner of this. A motor with a holding torque of at least 3kg-cm should be adequate.

The voltage of the motor is less important as you can supply more voltage than the rated voltage of the motor. More supply voltage typically equals a faster motor.

**Camera and Lens**
Any DSLR or mirrorless camera will be suitable. A macro lens of your choosing is also sufficient.

**Lighting**
I would recommend buying a flash for your camera, preferably wireless. Macrophotography often deals with very low amounts of light and high apertures so a flash is particularly useful. This project is designed to work with the shutter of a Sony multi-terminal port for triggering the shutter and a PC sync cable for detecting the output signal of the flash being triggered but either of these can be changed to the communication methods required for your camera and flash.
