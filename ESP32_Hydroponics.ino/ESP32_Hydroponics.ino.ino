//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Hoofdbestand
// 
// Een open-source hydroponisch systeem controller voor ESP32
// GitHub: https://github.com/imoliamedia/AXISKOM-ESP32-Hydroponisch-Systeem-Controller
//=========================================================================

#include "Settings.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <time.h>

// Globale variabelen die in meerdere modules worden gebruikt
TempSettings settings;
float currentTemp = 0.0;
bool pumpState = false;
bool overrideActive = false;
bool overridePumpState = false;
unsigned long cycleStartTime = 0;
bool isNachtModus = false;
char currentDateTime[64] = "Tijd wordt gesynchroniseerd...";

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  Serial.println("ESP32 Hydroponisch Systeem Controller opstarten...");
  
  // Pins configureren
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Relais uit bij opstarten
  
  // Laad instellingen uit EEPROM
  loadSettings();
  
  // Start temperatuursensor
  setupSensors();
  
  // Start WiFi
  setupWiFi();
  
  // Configureer tijdserver met zomertijd ondersteuning
  setupTimeSync();
  
  // Wacht op synchronisatie en update tijd
  updateCurrentTime();
  
  // Web server routes instellen
  setupWebServer();
  
  // Setup voltooid
  Serial.println("Setup voltooid, systeem actief");
  cycleStartTime = millis();
}

void loop() {
  // Handelt HTTP verzoeken af
  handleWebServer();
  
  // Update tijd elke minuut
  static unsigned long lastTimeUpdate = 0;
  if (millis() - lastTimeUpdate > 60000) {
    updateCurrentTime();
    lastTimeUpdate = millis();
  }
  
  // Controleer WiFi verbinding en probeer opnieuw te verbinden indien nodig
  checkWiFiConnection();
  
  // Lees temperatuur elke 5 seconden
  static unsigned long lastTempUpdate = 0;
  if (millis() - lastTempUpdate > 5000) {
    readTemperature();
    lastTempUpdate = millis();
  }
  
  // Als override niet actief is, volg dan de normale cyclus
  if (!overrideActive) {
    updatePumpCycle();
  }
}