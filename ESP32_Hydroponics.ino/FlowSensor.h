/*
 * FlowSensor.h
 *
 * Header voor de flowsensor module (YF-S201)
 */

#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include "Settings.h"

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
  // Externe variabelen
  extern volatile long flowPulseCount;
  extern float flowRate;
  extern float totalLiters;
  extern bool flowOk;
  
  // Functieprototypes
  void setupFlowSensor();
  void IRAM_ATTR flowPulseCounter();
  void checkFlowRate();
  float calculateFlowRate();
  String getFlowStatusJson();
  void resetFlowCounter();
#endif

#endif // FLOW_SENSOR_H