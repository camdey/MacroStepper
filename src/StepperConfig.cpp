#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"

TMC5160Stepper_Ext::TMC5160Stepper_Ext(uint16_t pinCS, float RS, int8_t link) : TMC5160Stepper(pinCS, RS, link) {};

void TMC5160Stepper_Ext::initDriver(uint16_t rmsCurrent, uint16_t nrMicrosteps, bool shaftDirection, int pinEN, int pinCS) {
    begin();
    // 23HM22-2804S: max 1900rms, stepper rated for 2.8A
    // SY42STH47-1206A: max 850rms, stepper rated for 1.2A
    rms_current(rmsCurrent); 
    microsteps(nrMicrosteps);
    shaft(shaftDirection); // inverse shaft, large target moves away from rear, small target moves towards rear

    enablePin(pinEN);
    chipSelectPin(pinCS);
}


void TMC5160Stepper_Ext::configStealthChop() {
    GCONF();
    en_pwm_mode(true);
    en_softstop(true);
    PWMCONF();
    pwm_autoscale(true);                            // Needed for stealthChop
    pwm_autograd(true);
    TZEROWAIT(5000);                                // wait time before next movement, approx 300ms, range 0 - 65,536
    TPOWERDOWN(10);
    TPWMTHRS(0);                                    // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
    TCOOLTHRS(0);                                   // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                                    // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
    THIGH(100);                                     // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
    VDCMIN(0);                                      // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
    CHOPCONF();
    toff(5);                                        // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
    hstrt(0);
    hend(0);
    tbl(2);                                         // comparator blank time
    chm(0);                                         // 0 = spreadCycle chopper mode (1 = const. off time)
    vhighchm(false);
    vhighfs(false);
    IHOLD_IRUN();
    ihold(5);
    irun(16);
    iholddelay(8);
    RAMPMODE(0);                                    // Positioning mode (using all A, D and V parameters)
    a1(5000);                                       // First acceleration between VSTART and V1
    AMAX(3500);                                     // Second acceleration between V1 and VMAX
    VMAX(STEALTH_CHOP_VMAX);                        // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
    v1(30000);                                      // First acceleration / deceleration phase treshold velocity
    DMAX(3500);                                     // Deceleration between VMAX and V1
    d1(5000);                                       // Deceleration between V1 and VSTOP
    VSTART(0);                                      // Motor start velocity (VSTOP >= VSTART)
    VSTOP(10);                                      // Motor stop velocity

    // STALLGUARD2 PARAMETERS
    sg_stop(false);                                 // stop by stallguard, disable to release motor after stall event

    stallGuardActive(false);
}


void TMC5160Stepper_Ext::configStallGuard() {
    GCONF();
    en_pwm_mode(true);
    en_softstop(false);
    PWMCONF();
    pwm_autoscale(true);                            // Needed for stealthChop
    pwm_autograd(true);
    TZEROWAIT(10000);                               // wait time before next movement, approx 300ms, range 0 - 65,536
    TPOWERDOWN(10000);
    TPWMTHRS(0);                                    // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
    TCOOLTHRS(400);                                 // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                                    // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
    THIGH(0);                                       // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
    VDCMIN(0);                                      // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
    CHOPCONF();
    toff(5);                                        // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
    hstrt(0);
    hend(0);
    tbl(2);                                         // comparator blank time
    chm(0);                                         // 0 = spreadCycle chopper mode (1 = const. off time)
    vhighchm(false);
    vhighfs(false);
    IHOLD_IRUN();
    ihold(5);
    irun(16);
    iholddelay(8);
    RAMPMODE(0);                                    // Positioning mode (using all A, D and V parameters)
    a1(1000);                                       // First acceleration between VSTART and V1
    AMAX(800);                                      // Second acceleration between V1 and VMAX
    VMAX(50000);                                    // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
    v1(30000);                                      // First acceleration / deceleration phase treshold velocity
    DMAX(1200);                                     // Deceleration between VMAX and V1
    d1(1000);                                       // Deceleration between V1 and VSTOP
    VSTART(2);                                      // Motor start velocity (VSTOP >= VSTART)
    VSTOP(12);                                      // Motor stop velocity

    // STALLGUARD2 PARAMETERS
    sg_stop(true);                                  // stop by stallguard, disable to release motor after stall event
    sgt(0);                                         // stallguard sensitivity
    semin(2);                                       // If the StallGuard2 result falls below SEMIN*32, the motor current is increased
    semax(1);                                       // If the StallGuard2 result is equal to or above (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
    sedn(0b00);                                     // For each 8 StallGuard2 values decrease current down step speed by one

    stallGuardActive(true);
}
