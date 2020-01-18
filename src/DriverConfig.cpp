#include "DriverConfig.h"

// config for TMC2130 when running homeRail() as StallGuard is required
void stallGuardConfig() {
  // select SPI slave
  digitalWrite(CS_PIN, LOW);

  //set TMC2130 config
  stepper.setAcceleration(1000);
  driver.push(); // reset registers
  driver.toff(3);
  driver.tbl(1);
  driver.hysteresis_start(8);
  driver.hysteresis_end(1);
  driver.diag1_stall(1);
  driver.diag1_active_high(1);
  driver.coolstep_min_speed(0x0009F); // 20bit max
  driver.semin(3);
  driver.semax(2);
  driver.sg_min(1); // if sg_result < sg_min*32, current increase
  driver.sg_max(3); // if sg_result >= (sg_min+sg_max+1)*32, current decrease
  driver.sedn(0b01);
  driver.sg_stall_value(-5);
  driver.stealthChop(0);

  // unselect SPI slave
  digitalWrite(CS_PIN, HIGH);

  stallGuardConfigured = true;
}

// config for TMC2130 when running all other functions as stealthChop is preferred
void silentStepConfig() {
  // select SPI slave
  digitalWrite(CS_PIN, LOW);

  stepper.setAcceleration(2000);
  driver.push(); // reset registers
  driver.stealthChop(1);
  driver.stealth_autoscale(1);
  driver.stealth_gradient(0xF); // 1 to 15
  driver.interpolate(1);
  driver.push();
  driver.diag1_stall(0);
  driver.diag1_active_high(0);
  driver.coolstep_min_speed(0x0);
  driver.stealth_freq(0x0); // 0 or 1 for 16MHz
//    driver.chopper_mode(0);
//    driver.stealth_max_speed(0x0002F);
//    driver.double_edge_step(1);
//    driver.chopper_mode(1);
//    driver.sync_phases(1);

  // unselect SPI slave 
  digitalWrite(CS_PIN, HIGH);

  stallGuardConfigured = false;
}
