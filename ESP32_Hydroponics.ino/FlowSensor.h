//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Flow sensor module - Header
//=========================================================================

#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include "Settings.h"

#ifdef ENABLE_FLOW_SENSOR
  // Definities voor flowsensor
  #define FLOW_PULSE_FACTOR 7.5  // Omrekenfactor volgens datasheet: frequentie (Hz) = 7.5 * flow (L/min)
  #define MIN_FLOW_RATE 1.0      // Minimale flow in L/min wanneer pomp actief is
  #define FLOW_CHECK_DELAY 5000  // Wachttijd in ms na inschakelen pomp voordat flow wordt gecontroleerd
  
  // Externe variabelen voor toegang vanuit andere modules
  extern float currentFlowRate;     // Huidige doorstroomsnelheid in L/min
  extern float totalFlowVolume;     // Totale doorgestroomde volume in liters
  extern bool flowSensorError;      // Foutindicator voor flowsensor
  extern bool noFlowDetected;       // Indicator voor "geen flow" terwijl pomp aan staat
  extern unsigned long lastFlowTime; // Tijdstip van laatste flow detectie
  
  // Functiedeclaraties
  void setupFlowSensor();
  void updateFlowSensor();
  void processFlowSensorReading();
  void checkFlowState();
  void resetFlowSensor();
  void IRAM_ATTR flowPulseCounter(); // Interrupt handler voor pulstellingen
#endif

#endif // FLOW_SENSOR_H