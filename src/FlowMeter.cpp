#include <Arduino.h>
#include "FlowMeter.h"

FlowMeter::FlowMeter(uint8_t pin) {
  // Set sensor pin
  _flowPin = pin;

  // Set Default configuratioun parameters
  _calibrationParams.flowRateMeasurementPeriod = 1000;
  _calibrationParams.minMillisPerReading = 10;
  _calibrationParams.volPerFlowCount = 1;

  // Set counters to 0
  _flowVolumeCounts = 0;
  _flowRateCounts = 0;
  _flowRateCountsPrevious = 0;

  // Set timers
  _lastFlowRateMillis = millis();
}

FlowMeter::~FlowMeter() {
  detachInterrupt(_flowPin);
}

void FlowMeter::begin() {
  // Set pin to input and attach interrupt
  pinMode(_flowPin, INPUT_PULLUP);
  resume();
}

void FlowMeter::pause() {
  detachInterrupt(_flowPin);
  _isRunning = false;
}

void FlowMeter::resume() {
  attachInterrupt(digitalPinToInterrupt(_flowPin), std::bind(&FlowMeter::flowInterrupt,this), RISING);
  _isRunning = true;
}

bool FlowMeter::isRunning() {
  return _isRunning;
}

FlowMeterCalibrationParams FlowMeter::getCalibrationParams() {
  return _calibrationParams;
}

void FlowMeter::setCalibrationParams (FlowMeterCalibrationParams params) {
  // Copy calibration parameters
  memcpy(&_calibrationParams, &params, sizeof(FlowMeterCalibrationParams));
}

float FlowMeter::getVolume() {
  _volume = _volume + (_flowVolumeCounts * _calibrationParams.volPerFlowCount);
  _flowVolumeCounts = 0;
  return _volume;
}

float FlowMeter::getFlowRate() {
  float flowRate;

  // Check to see if flow rate needs to be updated
  updateFlowRate();

  // Calculate flow rate from previous window
  // First compute the counts per ms
  flowRate = (float) _flowRateCountsPrevious / _calibrationParams.flowRateMeasurementPeriod;

  // Convert ms to seconds and counts to volume
  flowRate *= 1000 * _calibrationParams.volPerFlowCount;

  return flowRate;
}

void FlowMeter::resetVolme() {
  _volume = 0;
  _flowVolumeCounts = 0;
}

// Flow rate code was adapted from the SparkFun Weather Meter Kit Arduino Library wind speed algorithm
// https://github.com/sparkfun/SparkFun_Weather_Meter_Kit_Arduino_Library
void FlowMeter::updateFlowRate() {
  // The flow meter generates interrupts as it spins. Because these are
  // discrete pulses, we can't get an instantaneous measurement of the flow
  // rate. Instead, we need to track these signals over time and perform some
  // filtering to get an estimate of the current flow rate. There's lots of
  // ways to do this, but this library uses a modifed version of a moving
  // window filter.
  //
  // A moving window filter would require an array of values to be stored,
  // indicating when each pulse occurred. However for a fixed time window, the
  // number of pulses is unknown, so we don't know how big the array needs to
  // be. There are some solutions to this, but the one used here is to change
  // the moving time window to a static time window, which is illustrated in
  // this timing diagram with variable time between pulses:
  //
  // Pulses     | |  |   |    |     |      |       |        |
  // Window                  Last window        Current window
  // Time       ------|-----------------------|----------------|
  //                                        t_last           t_now
  //                  |---Measurement Period--|---Measurement Period--|
  //
  // A counter is used to track the number of pulses detected in the current
  // measurement window; when pulses are detected, the counter is incremented.
  // When t_now exceeds the measurement period, the total number of pulses is
  // used to calculate the average wind speed for that window. This filter
  // only outputs wind speed for the previous window, which does result in
  // delayed measurements, but is fine for most data logging applications since
  // logs can be synced with the measurement widows
  
  // Get current time
  uint32_t tNow = millis();

  // Calculate time since start of current measurement window
  uint32_t dt = tNow - _lastFlowRateMillis;

  // Check how long it's been since the start of this measurement window
  if (dt < _calibrationParams.flowRateMeasurementPeriod) {
        // Still within the current window, nothing to do (count is not
        // incremented here, that's done by the interrupt handler)
  }
  else {
    // We've passed the end of the measurement window, so we need to update
    // some things. But first, we need to check how long it's been since the
    // last time we updated, since it's possible we've not received any
    // pulses for a long time
    if (dt > (_calibrationParams.flowRateMeasurementPeriod * 2)) {
      // Over 2 measurement periods have passed since the last update,
      // meaning the flow rate is very slow or even zero. So we'll reset
      // the flow rate and counter, and set the start of the next window
      // to be now
      _flowRateCountsPrevious = 0;
      _flowRateCounts = 0;
      _lastFlowRateMillis = tNow;
    }
    else {
      // We've only just gone past the end of the measurement period, so
      // save the flow counts for the previous window, reset current
      // counter, and update time of start of next measurement window
      _flowRateCountsPrevious = _flowRateCounts;
      _flowRateCounts = 0;
      _lastFlowRateMillis += _calibrationParams.flowRateMeasurementPeriod;
    }
  }
}

void FlowMeter::flowInterrupt() {
  // Debounce the sensor
  if ((millis() - _lastFlowRateMillis) < _calibrationParams.minMillisPerReading) {
    // Do nothing
  } 
  else {
    // Check to see if flow rate needs to be updated
    updateFlowRate();
  
    // Increment counts
    _flowVolumeCounts++;
    _flowRateCounts++;
  }
}

uint32_t FlowMeter::getFlowCounts() {
  return _flowVolumeCounts;
}
uint32_t FlowMeter::getFlowRateCounts() {
  return _flowRateCountsPrevious;
}