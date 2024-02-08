#include <Arduino.h>
#include <FlowMeter.h>

//////////////////////////////////////////
// Constants
//////////////////////////////////////////
const uint8_t FLOW_PIN = 25;
const uint64_t DISPLAY_TIMER = 300;


//////////////////////////////////////////
// Global variables and objects
//////////////////////////////////////////
FlowMeter meter(FLOW_PIN);


///////////////////////////////
// Arduino setup and loop
///////////////////////////////
void setup() {
  Serial.begin(115200);

  // Set Meter Calibration
  FlowMeterCalibrationParams calParams = meter.getCalibrationParams();
  calParams.flowRateMeasurementPeriod = 250;  // 250ms
  calParams.minMillisPerReading = 5;          // 5ms
  calParams.volPerFlowCount = 1;              // use 1 for calibration runs
  meter.setCalibrationParams(calParams);

  meter.begin();

}

void loop() {
  static uint64_t timer;

  uint64_t currentTime = millis();
  if (currentTime - timer > DISPLAY_TIMER) {
    timer = currentTime;
    Serial.print("V = ");
    Serial.print(meter.getVolume();, 1);
    Serial.print(" R: ");
    Serial.println(meter0.getFlowRate(), 2);
  }
}

