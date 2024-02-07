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

The flow rate 