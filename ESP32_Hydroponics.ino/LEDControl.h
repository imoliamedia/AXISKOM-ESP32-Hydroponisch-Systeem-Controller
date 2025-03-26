/*
 * LEDControl.h
 *
 * Header voor de LED verlichting besturing module
 */

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "Settings.h"

#ifdef ENABLE_LED_CONTROL
  // Externe variabelen
  extern int currentLEDBrightness;
  extern bool ledIsOn;
  extern bool ledOverrideActive;
  extern int ledOverrideBrightness;
  
  // Functieprototypes
  void setupLEDControl();
  void updateLEDState();
  void setLEDBrightness(int brightness);
  void setLEDOverride(bool active, int brightness);
  void cancelLEDOverride();
  String getLEDStatusJson();
  
  #ifdef ENABLE_LIGHT_SENSOR
    extern float currentLuxLevel;
    extern unsigned long lastLuxReading;
    
    void readLightSensor();
  #endif
#endif

#endif // LED_CONTROL_H