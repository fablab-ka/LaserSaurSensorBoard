#define DEBUG

#include "Pins.h"
#include "PressureSensor.h"
#include "TempSensor.h"
#include "FlowSensor.h"
#include "BinarySensor.h"
#include <avr/wdt.h>

const float TEMP_MIN = 17;
const float TEMP_MAX = 24;

const int MAX_TIME_NONCRIT_NOTOKAY = 10; //Allows the LaserSaur to run x seconds without good noncritical/filter values

#define AMOUNT_SENSORS 10
Sensor* sensors[AMOUNT_SENSORS];

void setup() {
  digitalWrite(PIN_RELAIS1, HIGH);
  pinMode(PIN_RELAIS1, OUTPUT);
  digitalWrite(PIN_RELAIS2, HIGH);
  pinMode(PIN_RELAIS2, OUTPUT);

  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Init Sensors");
  delay(3000);
#endif
  initSensors();
}

boolean isOkay, isNoncritOkay;
int timeSinceNoncritOkay = 100;

void loop() {
  //Check if All Sensor Values are in range
  isOkay = true;
  isNoncritOkay = true;
  for (int i = 0; i <  AMOUNT_SENSORS; i++)
    if (!sensors[i]->evaluate()) {
      if(sensors[i]->isCritical()) {
        isOkay = false;
        isNoncritOkay = false;
      }else{
        isNoncritOkay = false;
      }
    }

  if(!isNoncritOkay) {
    if(++timeSinceNoncritOkay > MAX_TIME_NONCRIT_NOTOKAY)
      isOkay = false;
  } else {
    timeSinceNoncritOkay = 0;
  }


  //Set Status LED and relais'
  digitalWrite(PIN_LED1, isOkay);
  digitalWrite(PIN_RELAIS1, !isOkay);
  digitalWrite(PIN_RELAIS2, !shouldChillerRun(getSensorValue("T1"), getSensorValue("Flow")));

  //Print out values to Serial
  for (int i = 0; i < AMOUNT_SENSORS; i++)
    Serial.print(sensors[i]->printValue());
  Serial.print("  ");
  Serial.print(isOkay);
  Serial.print("\n");
  digitalWrite(PIN_LED12, !digitalRead(PIN_LED12));
  delay(100);

  wdt_reset();
  
}

int chillerTurnedOn = 0;
unsigned long lastTimeChillerActivated = millis();

boolean isChillerRunning = false;
boolean shouldChillerRun(float temperature, float flow) {
  if ((temperature < TEMP_MIN || flow < 0.01) && isChillerRunning) {
    Serial.print("Turning Chiller Off! Was running for ");
    Serial.print((millis() - lastTimeChillerActivated) / 1000);
    Serial.println("s");
    isChillerRunning = false;
  }
  if ((temperature > TEMP_MAX && flow > 0.01) && !isChillerRunning) {
    Serial.print("Turning Chiller On! For the ");
    Serial.print(chillerTurnedOn++);
    Serial.println(". time");
    isChillerRunning = true;
    lastTimeChillerActivated = millis();
  }
  return isChillerRunning;
}

float getSensorValue(String name) {
  for (int i = 0; i < AMOUNT_SENSORS; i++)
    if (sensors[i]->getName() == name)
      return sensors[i]->getValue();
  return 0; //TODO do better stuff
}

inline void initSensors() {
  pinMode(SDA, OUTPUT);
  digitalWrite(SDA, LOW);
  sensors[0] = new PressureSensor("G4", PIN_MPX6, 2.5476);
  sensors[0]->setLedPin(PIN_LED2);
  sensors[0]->setBounds(10, 1500, 5);
  sensors[0]->setNonCritical();

  sensors[1] = new PressureSensor("F7", PIN_MPX4, 2.5793);
  sensors[1]->setLedPin(PIN_LED3);
  sensors[1]->setBounds(-10, 3000, 10);
  sensors[1]->setNonCritical();

  sensors[2] = new PressureSensor("F9", PIN_MPX3, 2.5389);
  sensors[2]->setLedPin(PIN_LED4);
  sensors[2]->setBounds(15, 3000, 10);
  sensors[2]->setNonCritical();

  sensors[3] = new PressureSensor("H13", PIN_MPX2, 2.5538);
  sensors[3]->setLedPin(PIN_LED5);
  sensors[3]->setBounds(15, 7000, 10);
  sensors[3]->setNonCritical();

  sensors[4] = new PressureSensor("AK", PIN_MPX1, 2.5917);
  sensors[4]->setLedPin(PIN_LED6);
  sensors[4]->setBounds(15, 500, 10);
  sensors[4]->setNonCritical();

  sensors[5] = new PressureSensor("AirFlow", PIN_MPX5, 2.5587);
  sensors[5]->setLedPin(PIN_LED7);
  sensors[5]->setBounds(400, 1500, 100);
  sensors[5]->setAsFlow(3.14 * 0.075 * 0.075);
  sensors[5]->setNonCritical();

  sensors[6] = new TempSensor("T1", PIN_THERMISTOR1);
  sensors[6]->setLedPin(PIN_LED7);
  sensors[6]->setBounds(15, 30, 1);

  sensors[7] = new TempSensor("T2", PIN_THERMISTOR2);
  sensors[7]->setLedPin(PIN_LED8);
  sensors[7]->setBounds(15, 30, 1);

  sensors[8] = new FlowSensor("Flow", PIN_PULSE);
  sensors[8]->setLedPin(PIN_LED9);
  sensors[8]->setBounds(3, 10, 1);

  sensors[9] = new BinarySensor("Break", SCL);
  sensors[9]->setLedPin(PIN_LED10);
  sensors[9]->setBounds(0.5, 1.5, 0);
#ifdef DEBUG
  for (int i = 0; i < AMOUNT_SENSORS; i++)
    sensors[i]->init();
  Serial.println("");
#endif

  wdt_enable (WDTO_4S);
}

