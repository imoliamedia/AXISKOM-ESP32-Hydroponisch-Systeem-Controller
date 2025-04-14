/*
 * ESP32 Hydroponisch Systeem Controller
 * 
 * Copyright (C) 2024 AXISKOM
 * Website: https://axiskom.nl
 * 
 * Dit programma is vrije software: je mag het herdistribueren en/of wijzigen
 * onder de voorwaarden van de GNU General Public License zoals gepubliceerd door
 * de Free Software Foundation, ofwel versie 3 van de licentie, of
 * (naar jouw keuze) een latere versie.
 * 
 * Deze software is ontwikkeld als onderdeel van het AXISKOM kennisplatform
 * voor zelfredzaamheid en zelfvoorzienend leven.
 *
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