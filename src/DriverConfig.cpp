#include "DriverConfig.h"

void configStealthChop() {
  driver.GCONF();
  driver.en_pwm_mode(true);
  driver.en_softstop(true);
  driver.PWMCONF();
  driver.pwm_autoscale(true);             // Needed for stealthChop
  driver.pwm_autograd(true);
  driver.TZEROWAIT(5000);                 // wait time before next movement, approx 300ms, range 0 - 65,536
  driver.TPOWERDOWN(10);
  driver.TPWMTHRS(0);                     // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
  driver.TCOOLTHRS(0);                    // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                          // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
  driver.THIGH(100);                      // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
  driver.VDCMIN(0);                       // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
  driver.CHOPCONF();
  driver.toff(5);                         // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
  driver.hstrt(0);
  driver.hend(0);
  driver.tbl(2);                          // comparator blank time
  driver.chm(0);                          // 0 = spreadCycle chopper mode (1 = const. off time)
  driver.vhighchm(false);
  driver.vhighfs(false);
  driver.IHOLD_IRUN();
  driver.ihold(5);
  driver.irun(16);
  driver.iholddelay(8);
  driver.RAMPMODE(0);                     // Positioning mode (using all A, D and V parameters)
  driver.A1(5000);                        // First acceleration between VSTART and V1
  driver.AMAX(3500);                      // Second acceleration between V1 and VMAX
  driver.VMAX(stealthChopMaxVelocity);    // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
  driver.V1(30000);                       // First acceleration / deceleration phase treshold velocity
  driver.DMAX(3500);                      // Deceleration between VMAX and V1
  driver.D1(5000);                        // Deceleration between V1 and VSTOP
  driver.VSTART(0);                       // Motor start velocity (VSTOP >= VSTART)
  driver.VSTOP(10);                       // Motor stop velocity

  // STALLGUARD2 PARAMETERS
  driver.sg_stop(false);                  // stop by stallguard, disable to release motor after stall event

  stallGuardConfigured = false;
}

void configStallGuard() {
  driver.GCONF();
  driver.en_pwm_mode(true);
  driver.en_softstop(false);
  driver.PWMCONF();
  driver.pwm_autoscale(true);             // Needed for stealthChop
  driver.pwm_autograd(true);
  driver.TZEROWAIT(10000);                // wait time before next movement, approx 300ms, range 0 - 65,536
  driver.TPOWERDOWN(10000);
  driver.TPWMTHRS(0);                     // TSTEP ≥ TPWMTHRS: StealthChop enabled, DcStep disabled
  driver.TCOOLTHRS(400);                  // TCOOLTHRS ≥ TSTEP ≥ THIGH: CoolStep enabled, StealthChop PWM disabled.
                                          // TCOOLTHRS ≥ TSTEP: Stop on stall enabled.
  driver.THIGH(0);                        // TSTEP ≤ THIGH: CoolStep disabled, StealthChop PWM mode disabled. Stall Detection uses DcStep
  driver.VDCMIN(0);                       // |VACT| ≥ VDCMIN ≥ 256: same as if THIGH exceeded, DcStep auto commutation
  driver.CHOPCONF();
  driver.toff(5);                         // General enable for motorm driver, 0 = off, >= 1 driver enabled. Value doesn't affect stealthChop
  driver.hstrt(0);
  driver.hend(0);
  driver.tbl(2);                          // comparator blank time
  driver.chm(0);                          // 0 = spreadCycle chopper mode (1 = const. off time)
  driver.vhighchm(false);
  driver.vhighfs(false);
  driver.IHOLD_IRUN();
  driver.ihold(5);
  driver.irun(16);
  driver.iholddelay(8);
  driver.RAMPMODE(0);                     // Positioning mode (using all A, D and V parameters)
  driver.A1(1000);                        // First acceleration between VSTART and V1
  driver.AMAX(800);                       // Second acceleration between V1 and VMAX
  driver.VMAX(50000);                     // Motion ramp target velocity (VMAX >= VSTART). Can be changed during a motion.
  driver.V1(30000);                       // First acceleration / deceleration phase treshold velocity
  driver.DMAX(1200);                      // Deceleration between VMAX and V1
  driver.D1(1000);                        // Deceleration between V1 and VSTOP
  driver.VSTART(2);                       // Motor start velocity (VSTOP >= VSTART)
  driver.VSTOP(12);                       // Motor stop velocity

  // STALLGUARD2 PARAMETERS
  driver.sg_stop(true);                 // stop by stallguard, disable to release motor after stall event
  driver.sgt(0);                        // stallguard sensitivity
  driver.semin(2);                      // If the StallGuard2 result falls below SEMIN*32, the motor current is increased
  driver.semax(1);                      // If the StallGuard2 result is equal to or above (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
  driver.sedn(0b00);                    // For each 8 StallGuard2 values decrease current down step speed by one

  stallGuardConfigured = true;
}
