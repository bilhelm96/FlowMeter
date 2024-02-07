#ifndef FLOW_METER
#define FLOW_METER

#include <Arduino.h>
#include <FunctionalInterrupt.h>

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
    ~FlowMeter();

    bool begin();  // Returns true if pin is able to be attached to an interrupt

    // Enable and disable
    void pause();
    bool resume();
    bool isRunning();
    
    // Data Collection
    float getVolume();
    float getFlowRate();
    void resetVolme();

    // Sensor calibration parameters
    FlowMeterCalibrationParams getCalibrationParams();
    void setCalibrationParams (FlowMeterCalibrationParams params);

    // Helper functions for calibrating
    uint32_t getFlowCounts();
    uint32_t getFlowRateCounts();
  
    

  private:
    // Updates flow rate
    void updateFlowRate();

    // Sensor pin
    uint8_t _flowPin;

    // Meter calibration parameters
    FlowMeterCalibrationParams _calibrationParams;

    // Interrupt handler
    void flowInterrupt();

    // Variables to track measurements
    float _volume;
    volatile uint32_t _flowVolumeCounts;
    volatile uint32_t _flowRateCounts;
    uint32_t _flowRateCountsPrevious;
    uint32_t _lastFlowRateMillis;
    bool _isRunning;

};

#endif