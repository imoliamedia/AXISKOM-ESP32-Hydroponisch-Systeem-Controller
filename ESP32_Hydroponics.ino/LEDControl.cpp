/*
 * LEDControl.cpp
 *
 * Implementatie van de LED verlichting besturing module
 */

#include "LEDControl.h"

#ifdef ENABLE_LED_CONTROL

// Globale variabelen
int currentLEDBrightness = 0;
bool ledIsOn = false;
bool ledOverrideActive = false;
int ledOverrideBrightness = 0;

#ifdef ENABLE_LIGHT_SENSOR
  #include <Wire.h>
  
  // De volgende regel alleen behouden als de BH1750 bibliotheek geïnstalleerd is
  // #include <BH1750.h>
  
  class BH1750Stub {
  public:
    BH1750Stub() {}
    
    bool begin(int mode) { 
      return true; 
    }
    
    bool measurementReady() { 
      return true; 
    }
    
    float readLightLevel() { 
      return 500.0; // Default waarde
    }
  };
  
  BH1750Stub lightMeter;
  float currentLuxLevel = 0;
  unsigned long lastLuxReading = 0;
#endif

void setupLEDControl() {
  // Configureer pin als output
  pinMode(LED_PWM_PIN, OUTPUT);
  digitalWrite(LED_PWM_PIN, LOW);  // Start met LED uit
  
  #ifdef ENABLE_LIGHT_SENSOR
    // Initialiseer I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Simuleer initialisatie van de sensor
    Serial.println("Lichtsensor geïnitialiseerd (dummy-modus)");
  #endif
  
  Serial.println("LED besturing geïnitialiseerd op pin " + String(LED_PWM_PIN));
}

#ifdef ENABLE_LIGHT_SENSOR
void readLightSensor() {
  static unsigned long lastLogTime = 0;
  
  // Simuleer het lezen van de sensor
  currentLuxLevel = 500.0; // Vaste dummy waarde
  
  // Log alleen elke 60 seconden om serial monitor niet te spammen
  if (millis() - lastLogTime > 60000) {
    Serial.print("Lichtniveau (dummy): ");
    Serial.print(currentLuxLevel);
    Serial.println(" lux");
    lastLogTime = millis();
  }
  
  lastLuxReading = millis();
}
#endif

// Stel LED helderheid in (0-255)
void setLEDBrightness(int brightness) {
  // Begrens waarde tussen 0-255
  brightness = constrain(brightness, 0, 255);
  
  // Update interne status
  currentLEDBrightness = brightness;
  ledIsOn = (brightness > 0);
  
  // Debug output
  Serial.print("LED helderheid ingesteld op ");
  Serial.print(brightness);
  Serial.print(" (");
  Serial.print(map(brightness, 0, 255, 0, 100));
  Serial.println("%)");
  
  if (brightness > 0) {
    // Ook de fysieke pin aansturen als die is aangesloten
    if (brightness > 128) {
      digitalWrite(LED_PWM_PIN, HIGH);
    } else {
      digitalWrite(LED_PWM_PIN, LOW);
    }
  } else {
    digitalWrite(LED_PWM_PIN, LOW);
    Serial.println("LED uitgeschakeld");
  }
}

// Stel LED in op handmatige bediening
void setLEDOverride(bool active, int brightness) {
  Serial.print("LED override: active=");
  Serial.print(active ? "true" : "false");
  Serial.print(", brightness=");
  Serial.println(brightness);
 
  ledOverrideActive = active;
  
  if (active) {
    ledOverrideBrightness = brightness;
    setLEDBrightness(brightness);
    Serial.println("LED handmatige bediening geactiveerd");
  } else {
    Serial.println("LED handmatige bediening gedeactiveerd, terug naar automatisch");
    // Update LED status gebaseerd op huidige instellingen
    updateLEDState();
  }
}

// Annuleer override en ga terug naar automatische modus
void cancelLEDOverride() {
  if (ledOverrideActive) {
    ledOverrideActive = false;
    Serial.println("LED handmatige bediening geannuleerd, terug naar automatisch");
    
    // Update LED status gebaseerd op huidige instellingen en tijd
    updateLEDState();
  }
}

// Update LED status op basis van instellingen
void updateLEDState() {
  // Als handmatige override actief is, doe niets
  if (ledOverrideActive) {
    return;
  }
  
  // Haal huidige tijd op
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Kan LED niet updaten: tijd niet beschikbaar");
    return;
  }
  
  int currentHour = timeinfo.tm_hour;
  int newBrightness = 0;
  
  // Controleer of automatische modus actief is
  if (settings.ledAutoMode) {
    // Controleer of huidige tijd binnen de ingestelde tijdsperiode valt
    bool shouldBeOn = false;
    
    // Als start uur < eind uur, dan is het een normale dag cyclus
    if (settings.ledStartHour < settings.ledEndHour) {
      shouldBeOn = (currentHour >= settings.ledStartHour && currentHour < settings.ledEndHour);
    } 
    // Anders is het een nacht-overspannende cyclus (bijv. 22:00 - 06:00)
    else {
      shouldBeOn = (currentHour >= settings.ledStartHour || currentHour < settings.ledEndHour);
    }
    
    if (shouldBeOn) {
      // Bepaal helderheid op basis van instellingen
      #ifdef ENABLE_LIGHT_SENSOR
        if (settings.ledSensorMode) {
          // Bereken helderheid op basis van lichtsensor
          // (Minder omgevingslicht = hogere LED helderheid)
          if (currentLuxLevel <= settings.ledMinLux) {
            // Onder minimum lux, maximale helderheid
            newBrightness = settings.ledBrightness;
          } 
          else if (currentLuxLevel >= settings.ledMaxLux) {
            // Boven maximum lux, minimale helderheid (uit)
            newBrightness = 0;
          } 
          else {
            // Geleidelijke overgang tussen min en max
            float factor = 1.0 - ((currentLuxLevel - settings.ledMinLux) / 
                       (float)(settings.ledMaxLux - settings.ledMinLux));
            newBrightness = factor * settings.ledBrightness;
          }
        } else {
          // Sensor niet gebruikt voor helderheid
          newBrightness = settings.ledBrightness;
        }
      #else
        // Geen sensor, gebruik ingestelde helderheid
        newBrightness = settings.ledBrightness;
      #endif
    } else {
      // Buiten de ingestelde tijdsperiode, LED uit
      newBrightness = 0;
    }
  } else {
    // Automatische modus niet actief, LED uit
    newBrightness = 0;
  }
  
  // Update LED helderheid als er een verandering is
  if (newBrightness != currentLEDBrightness) {
    setLEDBrightness(newBrightness);
  }
}

// Genereer JSON met LED status
String getLEDStatusJson() {
  DynamicJsonDocument doc(512);
  
  doc["ledIsOn"] = ledIsOn;
  doc["currentBrightness"] = currentLEDBrightness;
  doc["brightness"] = settings.ledBrightness;
  doc["autoMode"] = settings.ledAutoMode;
  doc["startHour"] = settings.ledStartHour;
  doc["endHour"] = settings.ledEndHour;
  doc["overrideActive"] = ledOverrideActive;
  
  #ifdef ENABLE_LIGHT_SENSOR
    doc["lightSensorEnabled"] = true;
    doc["currentLuxLevel"] = currentLuxLevel;
    doc["sensorMode"] = settings.ledSensorMode;
    doc["minLux"] = settings.ledMinLux;
    doc["maxLux"] = settings.ledMaxLux;
  #else
    doc["lightSensorEnabled"] = false;
  #endif
  
  String response;
  serializeJson(doc, response);
  return response;
}

#endif // ENABLE_LED_CONTROL