#include "Sensor.h"
#include <Arduino.h>

#ifndef BINARYSENSOR_H
#define BINARYSENSOR_H

class BinarySensor :  public Sensor {
  public:
    BinarySensor(String name, int pin) :
      Sensor(name, pin) {
    }

    void init() {
      pinMode(pin, INPUT);
      digitalWrite(pin, HIGH);
    }

    float meassure() {
      return digitalRead(pin);
    }

    String getUnit() {
      return "";
    }

    void setAsFlow(float area) {
    } //TODO rework
};

#endif

