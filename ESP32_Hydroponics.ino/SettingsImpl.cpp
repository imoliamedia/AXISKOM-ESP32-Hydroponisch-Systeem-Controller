//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Settings implementatie
//=========================================================================

#include "Settings.h"

// WiFi instellingen
// VERANDER DEZE WAARDEN NAAR JE EIGEN WIFI NETWERK GEGEVENS
const char* ssid = "JouwWiFiNaam";
const char* password = "JouwWiFiWachtwoord";

// NTP server configuratie
const char* ntpServer = "pool.ntp.org";
const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Centraal-Europese tijd met zomertijd

// Instellingen functies
void loadSettings() {
  TempSettings loadedSettings;
  EEPROM.get(0, loadedSettings);
  
  // Controleer of de geladen instellingen geldig zijn door de magic number te controleren
  if (loadedSettings.magic == EEPROM_MAGIC) {
    settings = loadedSettings;
    Serial.println("Instellingen geladen uit EEPROM");
  } else {
    Serial.println("Geen geldige instellingen gevonden, gebruik standaardwaarden");
    // Standaardwaarden worden al ingesteld bij declaratie van de struct
    
    // Belangrijk: Sla de standaardinstellingen direct op in EEPROM
    saveSettings();
  }
}

void saveSettings() {
  // Zorg ervoor dat de magic number correct is voordat we opslaan
  settings.magic = EEPROM_MAGIC;
  
  // Schrijf de instellingen naar EEPROM
  EEPROM.put(0, settings);
  
  // Belangrijk: commit de wijzigingen naar de flash
  if (EEPROM.commit()) {
    Serial.println("Instellingen succesvol opgeslagen in EEPROM");
  } else {
    Serial.println("FOUT: EEPROM commit mislukt");
  }
}