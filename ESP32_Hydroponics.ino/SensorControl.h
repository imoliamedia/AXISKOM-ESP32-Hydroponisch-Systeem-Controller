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
 * Sensor en pompbesturing header
 */


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