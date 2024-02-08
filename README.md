# Flow Meter Library for Arduino

## Introduction

A library for using hall effect flow meters with ESP 32 devices.  The library will work with a variety of sensors and multiple instances of the library can exist in the same sketch.  An ESP32 microcontroller is required as the library relies on the FunctionalHeaders.h library of the ESP32.  The library uses a hardware interrupt to measure the flow rate and volume and has parameters to calibare to the meter's specifications

## Usage

`#include <FlowMeter.h>`

### Constructor and begin

Constructor takes one attribute, the GPIO pin to use as the interrupt for the sensor

`FlowMeter meter(PIN);`

Run the .begin() funtion in the Arduino setup() to intitialize the meter. The initalization attaches the pin to the interrupt and sets default calibration parameters. .begin() returns true if the interrupt attaches and false if there is an error
 
### Basic 

The library can start and stop the meter by attaching and detaching the interrupt and check if the meter is running (attached to the interrupt).

`meter.pause();` - stops the meter  
`meter.resume();` - starts the meter: returns true if successful / false if not  
`meter.isRunning();` - returns true if the meter is running / false if not  

There are three functions for data collecton

### Calibration

The library takes three variables for calibrating the meter.  They are containted in the structure **FlowCalibrationParams**.  The first parameter is the duration of time in ms that the sensor uses to calcuate the flow rate. The default is one second.  The second parameter is used to debounce the sensor to remove any inadvertent readings. The default is 10ms.  The final is used to set the amount of fluid that passes the sensor each time the sensor triggers the interrupt.  The default is one, therefore it needs to be calculated and set on a per sensor basis.

```c++
struct FlowMeterCalibrationParams {
  // Time between flow rate recorings (ms)
  uint32_t flowRateMeasurementPeriod;

  // Debounce Timer
  uint32_t minMillisPerReading;

  // Volume of fluid passing sensor per reading (unit of choice)
  float volPerFlowCount;
};
```

The flow rate is calculated via a moving window filter.  To simplify calculations a fixed window time period is used to count the number of pulses from the flow meter.  The flow rate is calculated for the last complete window so there is a slight lag in the flow rate measurement.  The **flowEateMeasurementPeriod** variable defines the size of the moving window filter in milliseconds.  A smaller value gives more instantaneous readings while a larger one gives a better average flow. Anywhere from 250 to 1000 milliseconds should be a good range for most applications.  

Like all mechanical switches, there can be some bounce in the contacts and the **minMillisPerReading** variable sets a time period where multiple contacts will be ignored to remove the bouncing and stabilize the readings.  A range of 3 - 10 milliseconds is normal.  

In order to be unit agnostic for flow rate and volume the library requires the user to calculate what volume of liquid passes through the sensor for each contact of the counter.  The **volPerFlowCount** variable can then be set for each application.  To calculate the **volPerFlowCount** initially set the variable to 1 and send a known quantity of fluid through the meter.  The volume reported will be the number of counts for that volume.  Divide the volume by the number of counts and put that value in the variable.  For example if you pour 200ml through and it reports 1257 tics the value of **volPerFlowCount** would be 200 / 1257 = 0.159.  Ideally you you perform multiple calibration runs and calculate the average volume per counter.

There are also two functions that can be used for calibration **getFlowCounts** returns the number of volume counts since last reset and **getFlowRateCounts** gives the number of flow rate counts in the current time window.
