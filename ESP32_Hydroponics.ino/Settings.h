//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Instellingen en configuratie
//=========================================================================

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

//=====================================================================
// CONFIGURATIE - GEBRUIKERS KUNNEN DEZE INSTELLINGEN AANPASSEN
//=====================================================================

// WiFi instellingen
// VERANDER DEZE WAARDEN NAAR JE EIGEN WIFI NETWERK GEGEVENS
extern const char* ssid;
extern const char* password;

// Pindefinities voor ESP32
// VERANDER DEZE PINNEN INDIEN NODIG OM AAN TE SLUITEN OP JE HARDWARE
#define ONE_WIRE_BUS 4    // GPIO4 voor DS18B20 temperatuursensor
#define RELAY_PIN 5       // GPIO5 voor relais

// NTP server configuratie
// JE KUNT EEN ANDERE NTP SERVER KIEZEN INDIEN GEWENST
extern const char* ntpServer;
// TIJDZONE CONFIGURATIE - STANDAARD OP CENTRAAL-EUROPESE TIJD MET ZOMERTIJD
// Voor andere tijdzones, raadpleeg: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
extern const char* timezone;

// Constanten voor nachtmodus
// VERANDER DEZE TIJDEN INDIEN JE DE NACHTMODUS OP ANDERE TIJDEN WILT LATEN WERKEN
const int NACHT_START_UUR = 22; // Nacht begint om 22:00
const int NACHT_EIND_UUR = 6;   // Nacht eindigt om 06:00

//=====================================================================
// EINDE GEBRUIKERSCONFIGURATIE
//=====================================================================

// Magic number voor EEPROM validatie
#define EEPROM_MAGIC 0xABCD

// Standaard instellingen voor pomp cycli (in seconden)
struct TempSettings {
  uint16_t magic = EEPROM_MAGIC;   // Magic number om te controleren of EEPROM geldig is
  int temp_laag_aan = 120;     // 2 minuten AAN als temp < 18°C
  int temp_laag_uit = 1080;    // 18 minuten UIT als temp < 18°C
  
  int temp_midden_aan = 120;   // 2 minuten AAN als temp 18-25°C
  int temp_midden_uit = 780;   // 13 minuten UIT als temp 18-25°C
  
  int temp_hoog_aan = 120;     // 2 minuten AAN als temp > 25°C
  int temp_hoog_uit = 480;     // 8 minuten UIT als temp > 25°C
  
  int nacht_aan = 60;          // 1 minuut AAN 's nachts
  int nacht_uit = 1740;        // 29 minuten UIT 's nachts
  
  float temp_laag_grens = 18.0;  // Grens tussen laag en midden °C
  float temp_hoog_grens = 25.0;  // Grens tussen midden en hoog °C
  
  char systeemnaam[32] = "Hydro Systeem 1";  // Systeemnaam als char array
};

// Controleer of de structuur past in de gereserveerde EEPROM ruimte
static_assert(sizeof(TempSettings) <= 512, "TempSettings te groot voor gereserveerde EEPROM ruimte");

// Extern declareren van globale variabelen die in verschillende modules worden gebruikt
extern TempSettings settings;
extern float currentTemp;
extern bool pumpState;
extern bool overrideActive;
extern bool overridePumpState;
extern unsigned long cycleStartTime;
extern bool isNachtModus;
extern char currentDateTime[64];

// Functie declaraties - alleen de prototypes
void saveSettings();
void loadSettings();

// Functiedeclaraties uit andere modules (prototypes)
void setupSensors();
void readTemperature();
void updatePumpCycle();
void setupWiFi();
void checkWiFiConnection();
void setupTimeSync();
void updateCurrentTime();
bool checkIfNight();
void setupWebServer();
void handleWebServer();
void handleRoot();
void handleGetStatus();
void handleGetSettings();
void handlePostSettings();
void handlePostOverride();

#endif // SETTINGS_H