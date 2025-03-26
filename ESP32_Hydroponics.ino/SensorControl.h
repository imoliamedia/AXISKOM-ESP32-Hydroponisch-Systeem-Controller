//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Sensor en pompbesturing header
//=========================================================================

#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Arduino.h>
#include "Settings.h"

// Functie prototypes voor temperatuursensor en pomp bediening
void readTemperature();
void handlePump();
void setPumpState(bool state);
void setPumpOverride(bool active, bool state);

// Controleren of het nacht is
bool isNightTime();

#endif // SENSOR_CONTROL_H