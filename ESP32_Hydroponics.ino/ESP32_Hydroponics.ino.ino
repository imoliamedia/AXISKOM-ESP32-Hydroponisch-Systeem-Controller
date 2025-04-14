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
 */

#include "Settings.h"

// Globale variabelen
float currentTemp = 0.0;          // Huidige temperatuur in °C
bool pumpActive = false;          // Huidige pompstatus
unsigned long pumpStartTime = 0;  // Tijdstip waarop de pomp is ingeschakeld
unsigned long pumpStopTime = 0;   // Tijdstip waarop de pomp is uitgeschakeld
unsigned long pumpRunTime = 0;    // Totale draaitijd van de pomp (seconden)
int currentCycleOn = 0;           // Huidige AAN tijd in seconden
int currentCycleOff = 0;          // Huidige UIT tijd in seconden
bool manualOverride = false;      // Handmatige besturing actief
TempSettings settings;            // Instellingen geladen uit EEPROM

void setup() {
  // Start seriële communicatie
  Serial.begin(115200);
  Serial.println("\n\nESP32 Hydroponisch Systeem Controller");
  Serial.println("© AXISKOM kennisplatform (https://axiskom.nl)");
  Serial.println("Opstarten...");
  
  // Laad instellingen uit EEPROM
  loadSettings();
  
  // Configureer pompaansturing
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Pomp uit bij opstarten
  
  // Initialiseer DS18B20 temperatuursensor
  setupTemperatureSensor();
  
  // Initialiseer WiFi
  setupWiFi();
  
  // Initialiseer NTP tijd synchronisatie
  setupTime();
  
  // Initialiseer optionele modules
  
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
    setupFlowSensor();
  #endif
  
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true && defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true
    setupEmailNotification();
  #endif
  
  // Initialiseer webserver
  setupWebServer();
  
  // Haal huidige temperatuur op
  updateTemperature();
  
  // Bereken initiële pompcyclus tijden
  updatePumpCycleTimes();
  
  Serial.println("Systeem geïnitialiseerd en klaar voor gebruik!");
  Serial.print("Je kunt de webinterface openen op: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Beheer WiFi verbinding
  checkWiFiConnection();
  
  // Beheer tijd synchronisatie
  checkTimeSync();
  
  // Verwerk webserver verzoeken
  handleWebClient();
  
  // Update temperatuur periodiek (elke 10 seconden)
  static unsigned long lastTempUpdate = 0;
  if (millis() - lastTempUpdate > 10000) {
    updateTemperature();
    updatePumpCycleTimes();
    lastTempUpdate = millis();
  }
  
  // Beheer pompbesturing, tenzij in handmatige modus
  if (!manualOverride) {
    managePump();
  }
  
  // Update systeemstatistieken (elke seconde)
  static unsigned long lastStatsUpdate = 0;
  if (millis() - lastStatsUpdate > 1000) {
    updateRuntime();
    lastStatsUpdate = millis();
  }
  
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
    checkFlowRate();
  #endif
}

// Update pomp draaitijd statistieken
void updateRuntime() {
  if (pumpActive) {
    pumpRunTime = pumpRunTime + 1;
  }
}

// Stel de pomp in op handmatige besturing
void setPumpManual(bool state) {
  manualOverride = true;
  setRelayState(state);
  Serial.print("Pomp handmatig ingesteld op: ");
  Serial.println(state ? "AAN" : "UIT");
}

// Schakel terug naar automatische modus
void setPumpAuto() {
  manualOverride = false;
  Serial.println("Pomp terug naar automatische modus");
}