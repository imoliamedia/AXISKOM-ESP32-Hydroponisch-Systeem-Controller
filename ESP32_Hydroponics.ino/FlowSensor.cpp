//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Flow sensor module - Implementatie
//=========================================================================

#include "FlowSensor.h"

#ifdef ENABLE_FLOW_SENSOR

// Globale variabelen voor flowsensor
float currentFlowRate = 0.0;      // Huidige doorstroomsnelheid in L/min
float totalFlowVolume = 0.0;      // Totale doorgestroomde volume in liters
bool flowSensorError = false;     // Foutindicator voor flowsensor
bool noFlowDetected = false;      // Indicator voor "geen flow" terwijl pomp aan staat
unsigned long lastFlowTime = 0;   // Tijdstip van laatste flow detectie

// Lokale (private) variabelen
volatile unsigned long pulseCount = 0;      // Aantal getelde pulsen (volatile omdat aangepast in ISR)
unsigned long oldPulseCount = 0;            // Vorige waarde voor berekeningen
unsigned long flowRateTimer = 0;            // Timer voor berekening van doorstroomsnelheid
unsigned long pumpStartTime = 0;            // Tijdstip waarop pomp is ingeschakeld
bool checkingFlowEnabled = false;           // Staat flow controle aan (na vertraging)
bool prevPumpState = false;                 // Vorige status van de pomp
unsigned long lastDebugReport = 0;          // Laatste tijdstip van debug rapport

// Interrupt handler voor pulstellingen
void IRAM_ATTR flowPulseCounter() {
  pulseCount++;
  lastFlowTime = millis();
}

// Initialisatie van de flowsensor
void setupFlowSensor() {
  Serial.println("Flowsensor initialiseren...");
  
  // Configureer flow sensor pin als input met pull-up weerstand
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  // Configureer interrupt op de flow sensor pin
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
  
  // Initialiseer variabelen
  resetFlowSensor();
  
  Serial.println("Flowsensor geïnitialiseerd op pin " + String(FLOW_SENSOR_PIN));
}

// Reset flowsensor tellingen en statussen
void resetFlowSensor() {
  pulseCount = 0;
  oldPulseCount = 0;
  currentFlowRate = 0.0;
  flowSensorError = false;
  noFlowDetected = false;
  lastFlowTime = 0;
  checkingFlowEnabled = false;
  flowRateTimer = millis();
  lastDebugReport = 0;
}

// Verwerk de sensorgegevens en bereken doorstroomsnelheid
void processFlowSensorReading() {
  // Bereken elke seconde de doorstroomsnelheid
  if (millis() - flowRateTimer >= 1000) {
    // Bereken de doorstroomsnelheid op basis van pulsen per seconde
    // Detach interrupt tijdens berekeningen om race conditions te voorkomen
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    
    // Bereken aantal pulsen in de afgelopen seconde
    unsigned long pulsesDelta = pulseCount - oldPulseCount;
    oldPulseCount = pulseCount;
    
    // Bereken doorstroomsnelheid op basis van pulsen per seconde
    // Volgens datasheet: flow (L/min) = frequency (Hz) / 7.5
    currentFlowRate = (float)pulsesDelta / FLOW_PULSE_FACTOR;
    
    // Bereken totale volume
    totalFlowVolume += currentFlowRate / 60.0; // Omzetten van L/min naar L/sec en dan naar totaal
    
    // VERBETERDE LOGGING LOGICA:
    // Log alleen als:
    // 1. De pomp AAN staat en er significante flow is (> 0.5 L/min), OF
    // 2. Debug modus is actief EN de timer is verlopen (max één bericht per 60 seconden)
    bool significantFlow = (currentFlowRate > 0.5);
    bool timeForDebugReport = (settings.flowSensorDebug && (millis() - lastDebugReport > 60000));
    
    if ((pumpState && significantFlow) || timeForDebugReport) {
      Serial.print("Waterstroming: ");
      Serial.print(currentFlowRate, 2);
      Serial.print(" L/min, Totaal: ");
      Serial.print(totalFlowVolume, 2);
      Serial.println(" L");
      
      if (timeForDebugReport) {
        lastDebugReport = millis();
      }
    }
    
    // Reset timer voor volgende meting
    flowRateTimer = millis();
    
    // Her-attacheer interrupt
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
  }
}

// Controleer de flowstatus en genereer waarschuwingen indien nodig
void checkFlowState() {
  // Controleer of de pompstatus is veranderd
  if (prevPumpState != pumpState) {
    prevPumpState = pumpState;
    
    if (pumpState) {
      // Pomp is ingeschakeld - start de timer
      pumpStartTime = millis();
      checkingFlowEnabled = false; // Schakel flow controle uit tijdens startvertraging
      
      // Reset flow error om nieuwe controle mogelijk te maken
      noFlowDetected = false;
      
      Serial.println("Pomp ingeschakeld. Flow controle start na " + String(FLOW_CHECK_DELAY) + "ms");
    } else {
      // Pomp is uitgeschakeld - geen flow controle nodig
      checkingFlowEnabled = false;
      noFlowDetected = false;
      
      // Log alleen als er een status is veranderd (niet constant)
      Serial.println("Pomp uitgeschakeld. Flow controle gedeactiveerd.");
    }
  }
  
  // Controleer of de startvertraging is verstreken om flow controle te beginnen
  if (pumpState && !checkingFlowEnabled && (millis() - pumpStartTime >= FLOW_CHECK_DELAY)) {
    checkingFlowEnabled = true;
    Serial.println("Flow controle actief");
  }
  
  // Controleer op "geen flow" conditie, maar alleen als:
  // 1. De pomp aan staat
  // 2. De startvertraging is verstreken
  // 3. We niet in override mode zijn (handmatige besturing)
  if (pumpState && checkingFlowEnabled && !overrideActive) {
    // Controleer of de flow onder de minimale drempel ligt
    if (currentFlowRate < settings.minFlowRate) {
      // Controleer hoe lang er geen flow is (minstens 20 seconden geen significante flow)
      if (millis() - lastFlowTime > 20000) {
        if (!noFlowDetected) {
          noFlowDetected = true;
          Serial.println("WAARSCHUWING: Geen waterstroming gedetecteerd terwijl de pomp actief is!");
          
          // Hier zou een e-mailnotificatie worden getriggerd (wordt geïmplementeerd in EmailNotification.cpp)
          #ifdef ENABLE_EMAIL_NOTIFICATION
            if (settings.flowAlertEnabled) {
              triggerFlowAlert();
            }
          #endif
        }
      }
    } else {
      // Flow is OK, reset error status
      noFlowDetected = false;
    }
  }
}

// Update flowsensor functies, wordt aangeroepen vanuit loop()
void updateFlowSensor() {
  // Verwerk sensorgegevens
  processFlowSensorReading();
  
  // Controleer flowstatus
  checkFlowState();
}

#endif // ENABLE_FLOW_SENSOR