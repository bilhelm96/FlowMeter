#ifndef FLOW_METER
#define FLOW_METER

#include <Arduino.h>

struct FlowMeterCalibrationParams {
  // Time between flow rate recorings (ms)
  uint32_t flowRateMeasurementPeriod;

  // Debounce Timer
  uint32_t minMillisPerReading;

  // Volume of fluid passing sensor per reading (unit of choice)
  float volPerFlowCount;
};

class FlowMeter {
  public:
    // Constructor
    FlowMeter(uint8_t pin);
    static void begin();

    // Enable and disable
    static void pause();  // Detaches the interrupt so the meter doesn't trigger
    static void resume(); // Enables the interrupt for data recording
    static bool isRunning();  // Reports wheter or not the meter is collecting data
    
    // Data Collection
    static float getVolume();
    static float getFlowRate();
    static void resetVolme();

    // Sensor calibration parameters
    static FlowMeterCalibrationParams getCalibrationParams();
    static void setCalibrationParams (FlowMeterCalibrationParams params);

    // Helper functions for calibrating
    static uint32_t getFlowCounts();
    static uint32_t getFlowRateCounts();


  private:
    // Interrupt handler
    static void flowInterrupt();
    
    // Updates flow rate
    static void updateFlowRate();

    // Sensor pin
    static uint8_t _flowPin;

    // Meter calibration parameters
    static FlowMeterCalibrationParams _calibrationParams;

    // Variables to track measurements
    static float _volume;
    static uint32_t _flowVolumeCounts;
    static uint32_t _flowRateCounts;
    static uint32_t _flowRateCountsPrevious;
    static uint32_t _lastFlowRateMillis;
    static bool _isRunning;

};

#endif