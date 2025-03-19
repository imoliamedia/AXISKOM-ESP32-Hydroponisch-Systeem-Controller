//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Settings implementatie
//=========================================================================

#include "Settings.h"

// WiFi instellingen
// VERANDER DEZE WAARDEN NAAR JE EIGEN WIFI NETWERK GEGEVENS
const char* ssid = "Jouw_WiFi_Naam";
const char* password = "Jouw_WiFi_Wachtwoord";

// Statische IP configuratie
bool useStaticIP = true;                    // Zet op false om DHCP te gebruiken
IPAddress staticIP(192, 168, 0, 21);        // Het vaste IP adres dat je wilt gebruiken
IPAddress gateway(192, 168, 0, 1);          // Je router IP adres
IPAddress subnet(255, 255, 255, 0);         // Subnet mask
IPAddress dns(192, 168, 0, 1);              // DNS server (meestal hetzelfde als gateway)

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