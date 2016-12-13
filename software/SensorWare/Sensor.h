#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>

class Sensor {

  protected:
    int pin;
    int ledpin;
    String name;
    float value;
    float higher, lower, hysterese;
    boolean isInRange;
    virtual float meassure() = 0;
    virtual String getUnit() = 0;
    boolean critical = true;
  public:
    Sensor() {
    }
    Sensor(String name, int pin) {
      this->name = name;
      this->pin = pin;
      this->ledpin = 13;
      this->isInRange = false;
      this->lower = 1000;
      this->higher = 0;
      this->hysterese = 0;
      this->critical = true;
    }

    void setLedPin(int ledpin) {
      this->ledpin = ledpin;
      pinMode(ledpin, OUTPUT);
      digitalWrite(ledpin, HIGH);
    }
    void setBounds(float lower, float higher, float hysterese) {
      this->lower = lower;
      this->higher = higher;
      this->hysterese = hysterese;
    }

    void setNonCritical() {
      this->critical = false;
    }

    String getName() {
      return this->name;
    }

    boolean isCritical() {
      return critical;
    }

    virtual void setAsFlow(float area) = 0; //fuck it

    virtual void init() = 0;
    float getValue() {
      return this->value;
    };

    String printValue() {
      String out = name;
      out += ":";
      float value = getValue();
      char buffer[8];
      dtostrf(value, 5, 2, buffer);
      out += buffer;
      out += getUnit();
      if (!isInRange) {
        out += "!";
      } else {
        out += " ";
      }
      out += " ";
      return out;
    }

    boolean evaluate() {
      float value = meassure();
      this->value = value;
      if (isInRange) {
        if (value > (higher + hysterese) || value < (lower - hysterese)) {
          isInRange = false;
          digitalWrite(ledpin, HIGH);
        }
      } else {
        if (value > (lower + hysterese) && value < (higher - hysterese)) {
          isInRange = true;
          digitalWrite(ledpin, LOW);
        }

      }
      return isInRange;
    }
};

#endif


