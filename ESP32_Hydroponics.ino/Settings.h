/*
 * Settings.h
 * 
 * Header bestand met instellingen, definities en functieprototypes
 * voor de ESP32 Hydroponisch Systeem Controller.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <time.h>

// Configuratie voor optionele functionaliteit
// TRUE = ingeschakeld, FALSE = uitgeschakeld
#define ENABLE_FLOW_SENSOR false      // Waterstroomsensor
#define ENABLE_EMAIL_NOTIFICATION false  // E-mail notificaties (alleen relevant als ENABLE_FLOW_SENSOR = true)
#define ENABLE_LED_CONTROL false     // LED verlichting besturing
#define ENABLE_LIGHT_SENSOR false   // GY-302 BH1750 lichtsensor (alleen relevant als LED_CONTROL = true)

// Pindefinities
#define ONE_WIRE_BUS 4    // GPIO4 voor DS18B20 temperatuursensor
#define RELAY_PIN 5       // GPIO5 voor relais

#ifdef ENABLE_FLOW_SENSOR
  #define FLOW_SENSOR_PIN 14 // GPIO14 voor YF-S201 flowsensor
  #define FLOW_PULSE_FACTOR 7.5  // Pulsen per liter voor YF-S201
  #define FLOW_CHECK_DELAY 5000  // Wachttijd na pompstart (ms)
#endif

#ifdef ENABLE_LED_CONTROL
  #define LED_PWM_PIN 16  // GPIO16 voor LED MOSFET aansturing via PWM
  #define LED_PWM_FREQ 5000  // PWM frequentie in Hz
  #define LED_PWM_CHANNEL 0  // PWM kanaal (0-15)
  #define LED_PWM_RESOLUTION 8 // PWM resolutie (8-bit = 0-255)
  
  #ifdef ENABLE_LIGHT_SENSOR
    #define I2C_SDA 21  // GPIO21 voor I2C SDA
    #define I2C_SCL 22  // GPIO22 voor I2C SCL
  #endif
#endif

// Overige constanten
#define EEPROM_SIZE 512
#define EEPROM_MAGIC 0xABCD
const int NACHT_START_UUR = 22; // Nacht begint om 22:00
const int NACHT_EIND_UUR = 6;   // Nacht eindigt om 06:00

// Instellingen struct
struct TempSettings {
  uint16_t magic = EEPROM_MAGIC;   // Magic number voor EEPROM validatie
  
  // Interval modus pomp instellingen (seconden)
  int temp_laag_aan = 120;     // 2 minuten AAN als temp < 18°C
  int temp_laag_uit = 1080;    // 18 minuten UIT als temp < 18°C
  int temp_midden_aan = 120;   // 2 minuten AAN als temp 18-25°C
  int temp_midden_uit = 780;   // 13 minuten UIT als temp 18-25°C
  int temp_hoog_aan = 120;     // 2 minuten AAN als temp > 25°C
  int temp_hoog_uit = 480;     // 8 minuten UIT als temp > 25°C
  int nacht_aan = 60;          // 1 minuut AAN 's nachts
  int nacht_uit = 1740;        // 29 minuten UIT 's nachts
  
  // Temperatuurgrenzen
  float temp_laag_grens = 18.0;  // Grens tussen laag en midden °C
  float temp_hoog_grens = 25.0;  // Grens tussen midden en hoog °C
  
  // Systeeminstellingen
  char systeemnaam[32] = "Hydro Systeem 1";  // Naam van het systeem
  bool continuModus = false;     // Pomp continu aan (voor NFT/DFT)
  
  // Flowsensor instellingen
  #ifdef ENABLE_FLOW_SENSOR
    float minFlowRate = 1.0;         // Minimale stroming in L/min
    bool flowSensorDebug = false;    // Extra debug output
    bool flowAlertEnabled = true;    // Waarschuwingen bij problemen
  #endif
  
  // E-mail instellingen
  #if defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION)
    char emailUsername[64] = "jouw.email@gmail.com";  // Gmail adres
    char emailPassword[64] = "jouw-app-wachtwoord";   // App-wachtwoord (niet normaal wachtwoord)
    char emailRecipient[64] = "ontvanger@email.com";  // Ontvangende e-mail
    bool emailDebug = false;         // Debug modus voor e-mail
  #endif
  
  // LED verlichting instellingen
  #ifdef ENABLE_LED_CONTROL
    int ledStartHour = 8;  // LED aan om 8:00
    int ledEndHour = 20;   // LED uit om 20:00
    int ledBrightness = 255; // Helderheid (0-255)
    bool ledAutoMode = true; // Automatisch schakelen op basis van tijd
    
    #ifdef ENABLE_LIGHT_SENSOR
      bool ledSensorMode = false;    // Automatisch helderheid op basis van sensor
      int ledMinLux = 200;           // Minimale lux waarbij LED 100% aan
      int ledMaxLux = 1000;          // Maximale lux waarbij LED 0% aan
      int ledSensorReadInterval = 60; // Seconden tussen sensor metingen
    #endif
  #endif
};

// Externe variabelen
extern TempSettings settings;
extern float currentTemp;
extern bool pumpActive;
extern unsigned long pumpStartTime;
extern unsigned long pumpStopTime;
extern unsigned long pumpRunTime;
extern bool manualOverride;
extern int currentCycleOn;
extern int currentCycleOff;

// SettingsImpl.cpp prototypes
void loadSettings();
void saveSettings();

// WiFiManager.cpp prototypes
void setupWiFi();
void checkWiFiConnection();

// TimeManager.cpp prototypes
void setupTime();
bool updateLocalTime();
void checkTimeSync();
bool isNightMode();
String getCurrentTimeString();
String getCurrentDateString();
String getFullDateTimeString();

// SensorControl.cpp prototypes
void setupTemperatureSensor();
void updateTemperature();
void updatePumpCycleTimes();
void managePump();
void setRelayState(bool state);

// WebServer.cpp prototypes
void setupWebServer();
void handleWebClient();
String getSystemStatusJson();

// In ESP32_Hydroponics.ino gedefinieerd
void setPumpManual(bool state);
void setPumpAuto();
void updateRuntime();

#ifdef ENABLE_FLOW_SENSOR
  // FlowSensor.cpp prototypes
  extern volatile long flowPulseCount;
  extern float flowRate;
  extern float totalLiters;
  extern bool flowOk;
  
  void setupFlowSensor();
  void checkFlowRate();
  String getFlowStatusJson();
  void resetFlowCounter();
#endif

#if defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION)
  // EmailNotification.cpp prototypes
  extern bool emailSendInProgress;
  extern unsigned long lastEmailSent;
  
  void setupEmailNotification();
  bool sendFlowAlertEmail();
  bool sendTestEmail();
  String getEmailStatusJson();
#endif

#ifdef ENABLE_LED_CONTROL
  // LEDControl.cpp prototypes
  extern int currentLEDBrightness;
  extern bool ledIsOn;
  
  void setupLEDControl();
  void updateLEDState();
  void setLEDBrightness(int brightness);
  void setLEDOverride(bool active, int brightness);
  void cancelLEDOverride();
  String getLEDStatusJson();
  
  #ifdef ENABLE_LIGHT_SENSOR
    extern float currentLuxLevel;
    
    void readLightSensor();
  #endif
#endif

#endif // SETTINGS_H