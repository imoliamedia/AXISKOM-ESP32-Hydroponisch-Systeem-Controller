/*
 * ESP32 Hydroponisch Systeem Controller
 * 
 * Een controller voor hydroponische systemen die pompcycli regelt op basis 
 * van watertemperatuur, met optionele modules voor flowsensing,
 * e-mailnotificaties en LED-verlichting.
 * 
 * De controller biedt een webinterface voor monitoring en configuratie
 * en ondersteunt zowel interval modus (hydro toren) als continue modus (NFT/DFT).
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
  
  #ifdef ENABLE_LED_CONTROL
    setupLEDControl();
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
  
  #ifdef ENABLE_LED_CONTROL
    // Update LED status elke seconde
    static unsigned long lastLEDUpdate = 0;
    if (millis() - lastLEDUpdate > 1000) {
      updateLEDState();
      lastLEDUpdate = millis();
    }
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