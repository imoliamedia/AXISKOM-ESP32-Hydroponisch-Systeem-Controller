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
 * TimeManager.cpp
 *
 * Tijd synchronisatie via NTP en hulpfuncties voor tijdsbeheer
 */

#include "Settings.h"

// Externe variabelen uit SettingsImpl.cpp
extern const char* ntpServer;
extern const char* timezone;

// Lokale variabelen
bool timeInitialized = false;
unsigned long lastNTPSync = 0;
const unsigned long ntpResyncInterval = 86400000; // 24 uur

// Configureer tijd synchronisatie
void setupTime() {
  Serial.println("Tijd synchronisatie instellen...");
  
  // Configureer tijd met NTP
  configTime(0, 0, ntpServer);
  setenv("TZ", timezone, 1);
  tzset();
  
  // Probeer direct te synchroniseren
  if (updateLocalTime()) {
    lastNTPSync = millis();
  }
}

// Update lokale tijd via NTP
bool updateLocalTime() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Kan lokale tijd niet ophalen");
    return false;
  }
  
  timeInitialized = true;
  
  char timeBuffer[80];
  strftime(timeBuffer, 80, "%d-%m-%Y %H:%M:%S", &timeinfo);
  Serial.print("Huidige tijd: ");
  Serial.println(timeBuffer);
  
  return true;
}

// Controleer en hersynchroniseer tijd indien nodig
void checkTimeSync() {
  // Alleen resynchroniseren als er WiFi is
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // Als tijd nog niet is geïnitialiseerd of het is tijd voor resync
  if (!timeInitialized || millis() - lastNTPSync > ntpResyncInterval) {
    Serial.println("NTP tijd (her)synchroniseren...");
    
    if (updateLocalTime()) {
      lastNTPSync = millis();
      Serial.println("Tijd succesvol gesynchroniseerd met NTP server");
    }
  }
}

// Controleer of we in nachtmodus zijn
bool isNightMode() {
  struct tm timeinfo;
  
  // Standaard geen nachtmodus als we tijd niet kunnen ophalen
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  
  int currentHour = timeinfo.tm_hour;
  
  // Controleer of het huidige uur binnen nachtmodus valt
  // Als NACHT_START_UUR > NACHT_EIND_UUR, dan is het een periode die middernacht overschrijdt
  if (NACHT_START_UUR > NACHT_EIND_UUR) {
    return (currentHour >= NACHT_START_UUR || currentHour < NACHT_EIND_UUR);
  } else {
    return (currentHour >= NACHT_START_UUR && currentHour < NACHT_EIND_UUR);
  }
}

// Haal huidige tijd op als geformatteerde string
String getCurrentTimeString() {
  struct tm timeinfo;
  char timeBuffer[30];
  
  if (getLocalTime(&timeinfo)) {
    strftime(timeBuffer, 30, "%H:%M:%S", &timeinfo);
    return String(timeBuffer);
  } else {
    return "Tijd niet beschikbaar";
  }
}

// Haal huidige datum en tijd op als geformatteerde string
String getCurrentDateString() {
  struct tm timeinfo;
  char dateBuffer[30];
  
  if (getLocalTime(&timeinfo)) {
    strftime(dateBuffer, 30, "%d-%m-%Y", &timeinfo);
    return String(dateBuffer);
  } else {
    return "Datum niet beschikbaar";
  }
}

// Volledige datum en tijd string
String getFullDateTimeString() {
  struct tm timeinfo;
  char buffer[50];
  
  if (getLocalTime(&timeinfo)) {
    strftime(buffer, 50, "%d-%m-%Y %H:%M:%S", &timeinfo);
    return String(buffer);
  } else {
    return "Datum/tijd niet beschikbaar";
  }
}

// Geeft terug of daglicht besparingstijd actief is
bool isDaylightSavingActive() {
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  
  return timeinfo.tm_isdst > 0;
}