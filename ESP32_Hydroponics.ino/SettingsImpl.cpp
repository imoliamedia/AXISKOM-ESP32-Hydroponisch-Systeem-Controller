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
 * SettingsImpl.cpp
 *
 * Implementatie van instellingenfuncties, inclusief laden/opslaan in EEPROM
 *
 */

#include "Settings.h"

// WiFi instellingen - Wijzig deze naar jouw netwerkinstellingen
const char* ssid = "Orange-dd28d";
const char* password = "4MY348t4";

// Statische IP configuratie
bool useStaticIP = true;                  // Zet op true voor vast IP, false voor DHCP
IPAddress staticIP(192, 168, 0, 31);      // Het gewenste vaste IP-adres
IPAddress gateway(192, 168, 0, 1);         // Je router/gateway IP
IPAddress subnet(255, 255, 255, 0);        // Subnet mask
IPAddress dns(192, 168, 0, 1);             // DNS server

// NTP server configuratie
const char* ntpServer = "time.google.com";
// Centraal-Europese tijd met zomertijd
const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3";

// Initialiseer EEPROM en laad instellingen
void loadSettings() {
  Serial.println("EEPROM initialiseren...");
  EEPROM.begin(EEPROM_SIZE);
  
  // Lees instellingen van EEPROM
  EEPROM.get(0, settings);
  
  // Controleer of opgeslagen waarden geldig zijn
  if (settings.magic != EEPROM_MAGIC) {
    Serial.println("Geen geldige instellingen gevonden in EEPROM, gebruik standaardwaarden");
    
    // Herstel naar standaardwaarden
    settings = TempSettings();
    
    // Sla standaardwaarden op
    saveSettings();
  } else {
    Serial.println("Instellingen geladen uit EEPROM");
  }
  
  // Toon instellingen
  Serial.print("Systeemnaam: ");
  Serial.println(settings.systeemnaam);
  Serial.print("Bedrijfsmodus: ");
  Serial.println(settings.continuModus ? "Continu (NFT/DFT)" : "Interval (Hydro Toren)");
  Serial.println("Temperatuurgrenzen:");
  Serial.print("  Laag < ");
  Serial.print(settings.temp_laag_grens);
  Serial.print("°C, Midden ");
  Serial.print(settings.temp_laag_grens);
  Serial.print("-");
  Serial.print(settings.temp_hoog_grens);
  Serial.print("°C, Hoog > ");
  Serial.print(settings.temp_hoog_grens);
  Serial.println("°C");
  
  // Optionele module instellingen
  #ifdef ENABLE_FLOW_SENSOR
    Serial.println("Flowsensor ingeschakeld:");
    Serial.print("  Minimale stroming: ");
    Serial.print(settings.minFlowRate);
    Serial.println(" L/min");
    Serial.print("  Flow waarschuwingen: ");
    Serial.println(settings.flowAlertEnabled ? "Ingeschakeld" : "Uitgeschakeld");
  #endif
  
  #if defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION)
    Serial.println("E-mail notificaties ingeschakeld");
  #endif
  
  #ifdef ENABLE_LED_CONTROL
    Serial.println("LED verlichting ingeschakeld:");
    Serial.print("  LED tijd: ");
    Serial.print(settings.ledStartHour);
    Serial.print(":00 - ");
    Serial.print(settings.ledEndHour);
    Serial.println(":00");
    Serial.print("  Auto modus: ");
    Serial.println(settings.ledAutoMode ? "Ingeschakeld" : "Uitgeschakeld");
    
    #ifdef ENABLE_LIGHT_SENSOR
      Serial.println("  Lichtsensor ingeschakeld");
    #endif
  #endif
}

// Sla instellingen op in EEPROM
void saveSettings() {
  Serial.println("Instellingen opslaan in EEPROM...");
  
  // Zorg ervoor dat de magic waarde correct is
  settings.magic = EEPROM_MAGIC;
  
  // Schrijf struct naar EEPROM
  EEPROM.put(0, settings);
  
  // Commit wijzigingen
  EEPROM.commit();
  
  Serial.println("Instellingen opgeslagen");
}

// Update systeemnaam
void updateSystemName(const char* newName) {
  // Kopieer de nieuwe naam (maximaal 31 tekens + NULL)
  strncpy(settings.systeemnaam, newName, sizeof(settings.systeemnaam) - 1);
  
  // Zorg voor juiste null-terminatie
  settings.systeemnaam[sizeof(settings.systeemnaam) - 1] = '\0';
  
  Serial.print("Systeemnaam bijgewerkt naar: ");
  Serial.println(settings.systeemnaam);
  
  // Sla op in EEPROM
  saveSettings();
}

// Update bedrijfsmodus (interval/continu)
void updateOperatingMode(bool continuMode) {
  settings.continuModus = continuMode;
  
  Serial.print("Bedrijfsmodus bijgewerkt naar: ");
  Serial.println(continuMode ? "Continu (NFT/DFT)" : "Interval (Hydro Toren)");
  
  // Sla op in EEPROM
  saveSettings();
}

// Update temperatuurinstellingen
void updateTemperatureSettings(float lowThreshold, float highThreshold,
                              int lowOn, int lowOff,
                              int midOn, int midOff,
                              int highOn, int highOff,
                              int nightOn, int nightOff) {
  
  // Update grenzen
  settings.temp_laag_grens = lowThreshold;
  settings.temp_hoog_grens = highThreshold;
  
  // Update cyclustijden
  settings.temp_laag_aan = lowOn;
  settings.temp_laag_uit = lowOff;
  settings.temp_midden_aan = midOn;
  settings.temp_midden_uit = midOff;
  settings.temp_hoog_aan = highOn;
  settings.temp_hoog_uit = highOff;
  settings.nacht_aan = nightOn;
  settings.nacht_uit = nightOff;
  
  Serial.println("Temperatuurinstellingen bijgewerkt");
  
  // Sla op in EEPROM
  saveSettings();
}

#ifdef ENABLE_FLOW_SENSOR
// Update flowsensor instellingen
void updateFlowSettings(float minFlow, bool alertEnabled) {
  settings.minFlowRate = minFlow;
  settings.flowAlertEnabled = alertEnabled;
  
  Serial.println("Flowsensor instellingen bijgewerkt");
  
  // Sla op in EEPROM
  saveSettings();
}
#endif

#if defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION)
// Update e-mail instellingen
void updateEmailSettings(const char* username, const char* password, const char* recipient) {
  // Kopieer de nieuwe instellingen
  strncpy(settings.emailUsername, username, sizeof(settings.emailUsername) - 1);
  strncpy(settings.emailPassword, password, sizeof(settings.emailPassword) - 1);
  strncpy(settings.emailRecipient, recipient, sizeof(settings.emailRecipient) - 1);
  
  // Zorg voor juiste null-terminatie
  settings.emailUsername[sizeof(settings.emailUsername) - 1] = '\0';
  settings.emailPassword[sizeof(settings.emailPassword) - 1] = '\0';
  settings.emailRecipient[sizeof(settings.emailRecipient) - 1] = '\0';
  
  Serial.println("E-mail instellingen bijgewerkt");
  
  // Sla op in EEPROM
  saveSettings();
}
#endif

#ifdef ENABLE_LED_CONTROL
// Update LED instellingen
void updateLEDSettings(int startHour, int endHour, int brightness, bool autoMode) {
  settings.ledStartHour = startHour;
  settings.ledEndHour = endHour;
  settings.ledBrightness = brightness;
  settings.ledAutoMode = autoMode;
  
  Serial.println("LED instellingen bijgewerkt");
  
  #ifdef ENABLE_LIGHT_SENSOR
  // Lichtsensor-specifieke instellingen worden hier niet bijgewerkt
  #endif
  
  // Sla op in EEPROM
  saveSettings();
}

#ifdef ENABLE_LIGHT_SENSOR
// Update LED lichtsensor instellingen
void updateLEDSensorSettings(bool sensorMode, int minLux, int maxLux, int readInterval) {
  settings.ledSensorMode = sensorMode;
  settings.ledMinLux = minLux;
  settings.ledMaxLux = maxLux;
  settings.ledSensorReadInterval = readInterval;
  
  Serial.println("LED lichtsensor instellingen bijgewerkt");
  
  // Sla op in EEPROM
  saveSettings();
}
#endif
#endif