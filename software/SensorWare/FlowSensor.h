#include "Sensor.h"
#include <Arduino.h>

#ifndef FLOWSENSOR_H
#define FLOWSENSOR_H

unsigned long time;
uint32_t timePoints[2];

void ISR_FlowSensor() {
  timePoints[0] = timePoints[1];
  timePoints[1] = millis();
}

class FlowSensor : public Sensor {
  public:
    FlowSensor(String name, int pin) :
      Sensor(name, pin) {
      time = millis();
      digitalWrite(3, HIGH);
      attachInterrupt(1, ISR_FlowSensor, RISING);
      sei();
      //Serial.println("Flow: Interrupt enabled");
    }

    void init() {
    }

    float meassure() {
      value = 1.0 / (float)(timePoints[1] - timePoints[0]) * 1000.0;
      if (timePoints[0] == 0)
        value = 0;
      timePoints[0] = 0;
      timePoints[1] = 0;
      //value /= 169.0 * 60.0;
      return value;
    }

    String getUnit() {
      return "l/min"; //TODO get real unit m3/s
    }

    void setAsFlow(float area) {
    } //TODO rework
  private:
    float area;
};

#endif

