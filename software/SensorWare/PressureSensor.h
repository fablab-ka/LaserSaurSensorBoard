#include "Sensor.h"
#include <Arduino.h>

#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

class PressureSensor : public Sensor {
  public:
    PressureSensor(String name, int pin, float zero) :
      Sensor(name, pin) {
      isFlow = false;
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
      this->zero = zero;
    }
    void setAsFlow(float area) {
      isFlow = true;
      this->area = area;
    }

    void setAsDifferential() {
      isFlow = false;
    }

    String getUnit() {
      if (isFlow) {
        return "m3/h";
      }
      else {
        return "Pa";
      }
    }

    void init() {
      float nzero = 0;
      for (int i = 0; i < 1000; i++)
        nzero += analogRead(pin);
      nzero /= 1000.0;
      nzero *= 5.0 / 1024.0;
      zero = nzero;
      Serial.print(name);
      Serial.print(" zero: ");
      Serial.print(nzero, 4);
      Serial.print(" | ");
    }

    float meassure() {
      float value = 0;
      for (int i = 0; i < 1000; i++)
        value += analogRead(pin);
      value /= 1000.0;
      value *= 5.0 / 1024.0;
      value -= zero;
      value *= 1000;

      //for(int i = 1; i < 10; i++)
      //  lastValues[i-1] = lastValues[i];
      //lastValues[9] = value;

      if (isFlow)
        value = sqrt(abs(value) * 2.0 / 1.2041) * area * 3600.0;

      return abs(value);
    }

  private:
    boolean isFlow;
    float area;
    float zero;
    float lastValues[10];
};

#endif

