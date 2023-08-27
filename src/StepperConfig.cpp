#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"

TMC5160Stepper_Ext::TMC5160Stepper_Ext(uint16_t pinCS, float RS, int8_t link) : TMC5160Stepper(pinCS, RS, link) {};

void initDriver(TMC5160Stepper_Ext &stepper, uint16_t rmsCurrent, uint16_t nrMicrosteps, bool shaftDirection, int pinEN, int pinCS) {
	stepper.begin();
  // 23HM22-2804S: max 1900rms, stepper rated for 2.8A
  // SY42STH47-1206A: max 850rms, stepper rated for 1.2A
	stepper.rms_current(rmsCurrent); 
	stepper.microsteps(nrMicrosteps);
  stepper.shaft(shaftDirection); // inverse shaft, large target moves away from rear, small target moves towards rear

  stepper.enablePin(pinEN);
  stepper.chipSelectPin(pinCS);
}


void configStealthChop(TMC5160Stepper_Ext &stepper) {
  stepper.GCONF();
  stepper.en_pwm_mode(true);
  stepper.en_softstop(true);
  stepper.PWMCONF();
  stepper.pwm_autoscale(true);             // Needed for stealthChop
  stepper.pwm_autograd(true);
  stepper.TZEROWAIT(5000);                 // wait time before next movement, approx 300ms, range 0 - 65,536
  stepper.TPOWERDOWN(10);
  stepper.TPWMTHRS(0);                     // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
  stepper.TCOOLTHRS(0);                    // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                          // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
  stepper.THIGH(100);                      // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
  stepper.VDCMIN(0);                       // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
  stepper.CHOPCONF();
  stepper.toff(5);                         // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
  stepper.hstrt(0);
  stepper.hend(0);
  stepper.tbl(2);                          // comparator blank time
  stepper.chm(0);                          // 0 = spreadCycle chopper mode (1 = const. off time)
  stepper.vhighchm(false);
  stepper.vhighfs(false);
  stepper.IHOLD_IRUN();
  stepper.ihold(5);
  stepper.irun(16);
  stepper.iholddelay(8);
  stepper.RAMPMODE(0);                     // Positioning mode (using all A, D and V parameters)
  stepper.a1(5000);                        // First acceleration between VSTART and V1
  stepper.AMAX(3500);                      // Second acceleration between V1 and VMAX
  stepper.VMAX(STEALTH_CHOP_VMAX);         // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
  stepper.v1(30000);                       // First acceleration / deceleration phase treshold velocity
  stepper.DMAX(3500);                      // Deceleration between VMAX and V1
  stepper.d1(5000);                        // Deceleration between V1 and VSTOP
  stepper.VSTART(0);                       // Motor start velocity (VSTOP >= VSTART)
  stepper.VSTOP(10);                       // Motor stop velocity

  // STALLGUARD2 PARAMETERS
  stepper.sg_stop(false);                  // stop by stallguard, disable to release motor after stall event

  stepper.stallGuardActive(false);
}


void configStallGuard(TMC5160Stepper_Ext &stepper) {
  stepper.GCONF();
  stepper.en_pwm_mode(true);
  stepper.en_softstop(false);
  stepper.PWMCONF();
  stepper.pwm_autoscale(true);             // Needed for stealthChop
  stepper.pwm_autograd(true);
  stepper.TZEROWAIT(10000);                // wait time before next movement, approx 300ms, range 0 - 65,536
  stepper.TPOWERDOWN(10000);
  stepper.TPWMTHRS(0);                     // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
  stepper.TCOOLTHRS(400);                  // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                          // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
  stepper.THIGH(0);                        // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
  stepper.VDCMIN(0);                       // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
  stepper.CHOPCONF();
  stepper.toff(5);                         // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
  stepper.hstrt(0);
  stepper.hend(0);
  stepper.tbl(2);                          // comparator blank time
  stepper.chm(0);                          // 0 = spreadCycle chopper mode (1 = const. off time)
  stepper.vhighchm(false);
  stepper.vhighfs(false);
  stepper.IHOLD_IRUN();
  stepper.ihold(5);
  stepper.irun(16);
  stepper.iholddelay(8);
  stepper.RAMPMODE(0);                     // Positioning mode (using all A, D and V parameters)
  stepper.a1(1000);                        // First acceleration between VSTART and V1
  stepper.AMAX(800);                       // Second acceleration between V1 and VMAX
  stepper.VMAX(50000);                     // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
  stepper.v1(30000);                       // First acceleration / deceleration phase treshold velocity
  stepper.DMAX(1200);                      // Deceleration between VMAX and V1
  stepper.d1(1000);                        // Deceleration between V1 and VSTOP
  stepper.VSTART(2);                       // Motor start velocity (VSTOP >= VSTART)
  stepper.VSTOP(12);                       // Motor stop velocity

  // STALLGUARD2 PARAMETERS
  stepper.sg_stop(true);                   // stop by stallguard, disable to release motor after stall event
  stepper.sgt(0);                          // stallguard sensitivity
  stepper.semin(2);                        // If the StallGuard2 result falls below SEMIN*32, the motor current is increased
  stepper.semax(1);                        // If the StallGuard2 result is equal to or above (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
  stepper.sedn(0b00);                      // For each 8 StallGuard2 values decrease current down step speed by one

  stepper.stallGuardActive(true);
}
