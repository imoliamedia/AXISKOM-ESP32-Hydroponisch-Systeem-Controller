/*
 * SensorControl.cpp
 *
 * Functies voor temperatuursensor en pompaansturing
 */

#include "Settings.h"

// Initialiseer OneWire en DallasTemperature instances
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Lokale variabelen voor pompcycli
unsigned long lastPumpStateChange = 0;
bool lastNightModeState = false;
unsigned long totalPumpRunTime = 0;  // Totale pompdraaitijd
bool pumpCycleActive = false;        // Actief met pompcyclus
float lastTempReading = 0;           // Laatste temperatuurmeting

// Configureer temperatuursensor
void setupTemperatureSensor() {
  Serial.print("DS18B20 temperatuursensor initialiseren op pin ");
  Serial.println(ONE_WIRE_BUS);
  
  sensors.begin();
  delay(100);  // Kort wachten op initialisatie
  
  // Controleer of er temperatuursensoren zijn gevonden
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Gevonden DS18B20 sensoren: ");
  Serial.println(deviceCount);
  
  if (deviceCount == 0) {
    Serial.println("WAARSCHUWING: Geen temperatuursensoren gevonden!");
  } else {
    // Stel hoge resolutie in (9-12 bits, 0.5°C tot 0.0625°C)
    sensors.setResolution(12);
    
    // Probeer direct een temperatuur te meten
    updateTemperature();
  }
}

// Update temperatuurmeting
void updateTemperature() {
  static unsigned long lastErrorLog = 0;
  // Vraag een temperatuurmeting aan
  sensors.requestTemperatures();
  
  // Haal temperatuur op (index 0 = eerste sensor)
  float tempC = sensors.getTempCByIndex(0);
  
  if (tempC == DEVICE_DISCONNECTED_C) {
    if (millis() - lastErrorLog > 100000) {
      Serial.println("FOUT: Kon temperatuur niet lezen, sensor mogelijk niet aangesloten");
      lastErrorLog = millis();
    }
    return;
  }
  
  // Update de globale temperatuurvariabele
  lastTempReading = currentTemp;  // Bewaar laatste meting
  currentTemp = tempC;
  
  // Toon temperatuur in Serial monitor (alleen als verandering > 0.1°C)
  if (abs(currentTemp - lastTempReading) > 0.1) {
    Serial.print("Watertemperatuur: ");
    Serial.print(currentTemp);
    Serial.println("°C");
  }
}

// Update pompcyclustijden op basis van huidige temperatuur en nachtmodus
void updatePumpCycleTimes() {
  static unsigned long lastCyclusLog = 0;
  static int lastCyclusType = -1; // -1=geen, 0=continu, 1=laag, 2=midden, 3=hoog, 4=nacht
  
  // Detecteer nachtmodus
  bool nightMode = isNightMode();
  
  // Als we in continue modus staan, geen pompcycli instellen
  if (settings.continuModus) {
    currentCycleOn = 1; // Dummy waarden, worden niet gebruikt in continue modus
    currentCycleOff = 0;
    
    // Loggen alleen bij wijziging
    if (lastCyclusType != 0) {
      Serial.println("Continue modus actief (NFT/DFT). Pomp blijft continu aan.");
      lastCyclusType = 0;
    }
    return;
  }
  
  // Reset cyclus vlag voor interval modus
  pumpCycleActive = false;
  
  // Als in nachtmodus, gebruik nachtinstellingen
  if (nightMode) {
    currentCycleOn = settings.nacht_aan;
    currentCycleOff = settings.nacht_uit;
    
    // Log alleen als we net nachtmodus ingaan
    if (!lastNightModeState) {
      Serial.println("Nachtmodus actief, aangepaste pompcyclus van toepassing");
      lastNightModeState = true;
      lastCyclusType = 4;
    }
  } else {
    // Dagmodus, bepaal op basis van temperatuur
    lastNightModeState = false;
    
    if (currentTemp < settings.temp_laag_grens) {
      // Lage temperatuur cyclus
      currentCycleOn = settings.temp_laag_aan;
      currentCycleOff = settings.temp_laag_uit;
      
      // Log alleen bij wijziging of elke 5 minuten
      if (lastCyclusType != 1 || (millis() - lastCyclusLog > 100000)) {
        Serial.print("Lage temperatuur cyclus actief (");
        Serial.print(currentTemp);
        Serial.print("°C < ");
        Serial.print(settings.temp_laag_grens);
        Serial.println("°C)");
        lastCyclusLog = millis();
        lastCyclusType = 1;
      }
    } 
    else if (currentTemp >= settings.temp_laag_grens && currentTemp < settings.temp_hoog_grens) {
      // Midden temperatuur cyclus
      currentCycleOn = settings.temp_midden_aan;
      currentCycleOff = settings.temp_midden_uit;
      
      // Log alleen bij wijziging of elke 5 minuten
      if (lastCyclusType != 2 || (millis() - lastCyclusLog > 100000)) {
        Serial.print("Midden temperatuur cyclus actief (");
        Serial.print(settings.temp_laag_grens);
        Serial.print("°C <= ");
        Serial.print(currentTemp);
        Serial.print("°C < ");
        Serial.print(settings.temp_hoog_grens);
        Serial.println("°C)");
        lastCyclusLog = millis();
        lastCyclusType = 2;
      }
    } 
    else {
      // Hoge temperatuur cyclus
      currentCycleOn = settings.temp_hoog_aan;
      currentCycleOff = settings.temp_hoog_uit;
      
      // Log alleen bij wijziging of elke 5 minuten
      if (lastCyclusType != 3 || (millis() - lastCyclusLog > 100000)) {
        Serial.print("Hoge temperatuur cyclus actief (");
        Serial.print(currentTemp);
        Serial.print("°C >= ");
        Serial.print(settings.temp_hoog_grens);
        Serial.println("°C)");
        lastCyclusLog = millis();
        lastCyclusType = 3;
      }
    }
  }
}

// Beheer de pompbesturing (cycli of continue modus)
void managePump() {
  // Als we in continue modus zijn, houd de pomp gewoon aan
  if (settings.continuModus) {
    setRelayState(true);
    return;
  }
  
  // Anders beheer normale cyclus
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastPumpStateChange;
  
  // Controleer of huidige status moet worden gewijzigd
  if (pumpActive) {
    // Pomp is aan, controleer of uitschakeltijd is bereikt
    if (elapsedTime >= (unsigned long)(currentCycleOn * 1000)) {
      // Tijd om de pomp uit te schakelen
      setRelayState(false);
      lastPumpStateChange = currentTime;
    }
  } else {
    // Pomp is uit, controleer of inschakeltijd is bereikt
    if (elapsedTime >= (unsigned long)(currentCycleOff * 1000)) {
      // Tijd om de pomp in te schakelen
      setRelayState(true);
      lastPumpStateChange = currentTime;
    }
  }
}

// Stel relaisstatus in
void setRelayState(bool state) {
  // Update globale pompstatus
  if (state != pumpActive) {
    // Status verandert, log de wijziging
    Serial.print("Pomp ");
    Serial.println(state ? "AAN" : "UIT");
    
    if (state) {
      // Pomp gaat aan
      pumpStartTime = millis();
    } else {
      // Pomp gaat uit
      pumpStopTime = millis();
      
      // Bereken draaitijd voor deze cyclus
      unsigned long thisCycleTime = pumpStopTime - pumpStartTime;
      // Converteer naar seconden en tel op bij totaal
      totalPumpRunTime += thisCycleTime / 1000;
    }
  }
  
  // Stel de hardware status in
  digitalWrite(RELAY_PIN, state);
  pumpActive = state;
}