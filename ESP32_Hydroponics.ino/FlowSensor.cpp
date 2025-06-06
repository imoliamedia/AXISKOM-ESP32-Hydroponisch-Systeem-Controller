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
 * FlowSensor.cpp
 *
 * Implementatie van de flowsensor module (YF-S201)
 */

#include "FlowSensor.h"

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true

// Flowsensor variabelen
volatile long flowPulseCount = 0;  // Aantal pulsen (flow)
float flowRate = 0.0;             // Huidige stromingssnelheid in l/min
float totalLiters = 0.0;          // Totaal aantal liters
bool flowOk = true;               // Flowstatus (OK/probleem)
unsigned long lastFlowCheck = 0;  // Tijdstip laatste controle
unsigned long lastPulseTime = 0;  // Tijdstip laatste puls

// Interrupt functie voor flowsensor
void IRAM_ATTR flowPulseCounter() {
  flowPulseCount++;
  lastPulseTime = millis();
}

// Initialiseer flowsensor
void setupFlowSensor() {
  Serial.print("Flowsensor YF-S201 initialiseren op pin ");
  Serial.println(FLOW_SENSOR_PIN);
  
  // Configureer pin
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  // Koppel interrupt aan pin
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
  
  // Reset variabelen
  flowPulseCount = 0;
  flowRate = 0.0;
  totalLiters = 0.0;
  flowOk = true;
  
  Serial.println("YF-S201 flowsensor geïnitialiseerd");
  Serial.println("YF-S201 specificatie: 450 pulsen per liter water");
}

// Bereken huidige flowsnelheid
float calculateFlowRate() {
  // Ontkoppel interrupt tijdelijk
  detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
  
  // Sla huidige pulsstand op en reset counter
  long pulseCount = flowPulseCount;
  flowPulseCount = 0;
  
  // Bepaal tijdsverschil sinds laatste meting
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastFlowCheck;
  lastFlowCheck = currentTime;
  
  // Voorkom deling door nul of onrealistische waarden
  if (elapsedTime < 100) {
    // Koppel interrupt opnieuw
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
    return flowRate; // Behoud vorige waarde
  }
  
  // Zet om naar seconden
  float elapsedTimeSeconds = elapsedTime / 1000.0;
  
  // YF-S201 specificatie: 450 pulsen per liter
  // Bereken flowrate: (pulsen / 450) / tijd_in_seconden * 60 = L/min
  float currentFlowRate = 0.0;
  
  if (pulseCount > 0) {
    float litersThisPeriod = pulseCount / 450.0;  // YF-S201: 450 pulsen per liter
    currentFlowRate = (litersThisPeriod / elapsedTimeSeconds) * 60.0;  // naar L/min
    
    // Update totaal volume
    totalLiters += litersThisPeriod;
  } else {
    currentFlowRate = 0.0;
  }
  
  // Koppel interrupt opnieuw
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
  
  return currentFlowRate;
}

// Controleer waterstroming
void checkFlowRate() {
  // Bereken actuele flowrate
  flowRate = calculateFlowRate();
  
  // Als de pomp aan staat, controleer op problemen
  if (pumpActive) {
    // Wacht even na het inschakelen van de pomp (FLOW_CHECK_DELAY)
    unsigned long pumpOnTime = millis() - pumpStartTime;
    
    if (pumpOnTime > FLOW_CHECK_DELAY) {
      // Controleer of we flow hebben
      unsigned long timeSinceLastPulse = millis() - lastPulseTime;
      
      // Als er geen flow is gedetecteerd en de minimale flowrate niet wordt gehaald
      if (flowRate < settings.minFlowRate && timeSinceLastPulse > 10000) {
        if (flowOk) {  // Als dit de eerste keer is dat we een probleem detecteren
          Serial.println("WAARSCHUWING: Geen of onvoldoende waterstroming gedetecteerd!");
          Serial.print("Huidige flow: ");
          Serial.print(flowRate);
          Serial.print(" L/min (minimum: ");
          Serial.print(settings.minFlowRate);
          Serial.println(" L/min)");
          Serial.print("Pomp capaciteit instelling: ");
          Serial.print(settings.pumpCapacityLPH);
          Serial.println(" L/h");
          
          flowOk = false;
          
          // Stuur e-mail notificatie indien ingeschakeld
          #if defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true
            if (settings.flowAlertEnabled) {
              sendFlowAlertEmail();
            }
          #endif
        }
      } else {
        if (!flowOk) {  // Als we net hersteld zijn van een probleem
          Serial.println("Waterstroming is nu OK");
          flowOk = true;
        }
      }
    }
  } else {
    // Als de pomp uit staat, reset de flowOk status
    flowOk = true;
  }
}

// Reset flow teller
void resetFlowCounter() {
  totalLiters = 0.0;
  Serial.println("Flow teller gereset");
}

// Genereer JSON met flowsensor status
String getFlowStatusJson() {
  DynamicJsonDocument doc(512);
  
  doc["flowRate"] = flowRate;
  doc["totalLiters"] = totalLiters;
  doc["flowOk"] = flowOk;
  doc["minFlowRate"] = settings.minFlowRate;
  doc["flowAlertEnabled"] = settings.flowAlertEnabled;
  doc["pumpCapacityLPH"] = settings.pumpCapacityLPH;
  doc["sensorType"] = "YF-S201";
  doc["pulsesPerLiter"] = 450;
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    doc["emailEnabled"] = true;
  #else
    doc["emailEnabled"] = false;
  #endif
  
  String response;
  serializeJson(doc, response);
  return response;
}

#endif // ENABLE_FLOW_SENSOR