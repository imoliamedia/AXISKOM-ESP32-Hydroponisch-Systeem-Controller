//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Sensor en pomp besturing module
//=========================================================================

#include "Settings.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// OneWire en DallasTemperature instanties
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Temperatuur foutdetectie
bool tempSensorError = false;
const int MAX_TEMP_ERRORS = 5;  // Maximaal aantal opeenvolgende fouten voordat we een waarschuwing geven
int tempErrorCount = 0;

void setupSensors() {
  Serial.println("Temperatuursensor initialiseren...");
  sensors.begin();
  
  // Controleer of er sensoren zijn gevonden
  int deviceCount = sensors.getDeviceCount();
  if (deviceCount == 0) {
    Serial.println("WAARSCHUWING: Geen temperatuursensoren gevonden! Controleer de aansluiting.");
    tempSensorError = true;
  } else {
    Serial.print("Aantal temperatuursensoren gevonden: ");
    Serial.println(deviceCount);
    tempSensorError = false;
  }
}

void readTemperature() {
  // Verzoek om temperatuurmeting
  sensors.requestTemperatures();
  
  // Lees de waarde uit
  float tempReading = sensors.getTempCByIndex(0);
  
  if (tempReading == DEVICE_DISCONNECTED_C) {
    tempErrorCount++;
    
    if (tempErrorCount >= MAX_TEMP_ERRORS) {
      Serial.println("FOUT: Temperatuursensor kan niet worden gelezen. Controleer de aansluiting!");
      tempSensorError = true;
    } else {
      Serial.println("Fout bij het lezen van de temperatuur, nieuwe poging over 5 seconden");
    }
    
    // Bij fout, houden we de laatste waarde (of zet een fallback waarde)
    if (currentTemp == -127.0) {
      // Als er nog geen geldige lezing is, gebruik een veilige standaardwaarde
      currentTemp = 20.0; // Veilige middelwaarde
      Serial.println("Gebruik standaardtemperatuur van 20.0°C");
    }
  } else {
    // Bij succesvolle meting, reset de foutenteller
    tempErrorCount = 0;
    tempSensorError = false;
    
    // Update de huidige temperatuur
    currentTemp = tempReading;
    Serial.print("Temperatuur: ");
    Serial.print(currentTemp);
    Serial.println("°C");
  }
}

void updatePumpCycle() {
  unsigned long currentTime = millis();
  
  // Bepalen van de juiste aan/uit tijden op basis van temperatuur
  int activeTimeOn;
  int activeTimeOff;
  String cycleType;
  
  if (isNachtModus) {
    activeTimeOn = settings.nacht_aan;
    activeTimeOff = settings.nacht_uit;
    cycleType = "nacht";
  } else if (currentTemp < settings.temp_laag_grens) {
    activeTimeOn = settings.temp_laag_aan;
    activeTimeOff = settings.temp_laag_uit;
    cycleType = "lage temperatuur";
  } else if (currentTemp < settings.temp_hoog_grens) {
    activeTimeOn = settings.temp_midden_aan;
    activeTimeOff = settings.temp_midden_uit;
    cycleType = "midden temperatuur";
  } else {
    activeTimeOn = settings.temp_hoog_aan;
    activeTimeOff = settings.temp_hoog_uit;
    cycleType = "hoge temperatuur";
  }
  
  // Omzetten naar milliseconden
  unsigned long cycleOn = activeTimeOn * 1000;
  unsigned long cycleOff = activeTimeOff * 1000;
  unsigned long totalCycle = cycleOn + cycleOff;
  
  // Bepaal hoeveel tijd er verstreken is in de huidige cyclus
  // Gebruik signed long om overflow problemen te voorkomen
  unsigned long elapsedTime = ((long)(currentTime - cycleStartTime)) % totalCycle;
  
  // Bepaal of de pomp aan of uit moet zijn
  bool newPumpState = (elapsedTime < cycleOn);
  
  // Als de pompstatus moet veranderen
  if (pumpState != newPumpState) {
    pumpState = newPumpState;
    digitalWrite(RELAY_PIN, pumpState ? HIGH : LOW);
    
    // Log de wijziging
    Serial.print("Pomp ");
    Serial.print(pumpState ? "AAN" : "UIT");
    Serial.print(" geschakeld (");
    Serial.print(cycleType);
    Serial.print(" cyclus: ");
    Serial.print(activeTimeOn);
    Serial.print("s aan, ");
    Serial.print(activeTimeOff);
    Serial.println("s uit)");
  }
}