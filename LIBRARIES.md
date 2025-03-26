# ESP32 Hydroponisch Systeem Controller - Benodigde Bibliotheken

Dit document bevat een gedetailleerd overzicht van alle bibliotheken die nodig zijn voor het ESP32 Hydroponisch Systeem Controller project, inclusief installatie-instructies en links naar bronnen.

## Overzicht van Bibliotheken

### Basis Bibliotheken (altijd vereist)

| Bibliotheek | Versie | Beschrijving | Installatiemethode |
|-------------|--------|--------------|-------------------|
| WiFi.h | Ingebouwd | Wifi-connectiviteit voor ESP32 | Onderdeel van ESP32 core |
| EEPROM.h | Ingebouwd | Opslag van instellingen | Onderdeel van ESP32 core |
| WebServer.h | Ingebouwd | Webserver functionaliteit | Onderdeel van ESP32 core |
| time.h | Ingebouwd | Tijd en datum functies | Onderdeel van ESP32 core |
| ArduinoJson | ≥ 6.19.4 | JSON parsing en generatie | Arduino Library Manager |
| OneWire | ≥ 2.3.5 | Communicatie met 1-Wire apparaten | Arduino Library Manager |
| DallasTemperature | ≥ 3.8.0 | DS18B20 temperatuursensor ondersteuning | Arduino Library Manager |

### Optionele Bibliotheken (afhankelijk van geactiveerde functies)

| Bibliotheek | Versie | Beschrijving | Nodig voor | Installatiemethode |
|-------------|--------|--------------|-----------|-------------------|
| ESP_Mail_Client | ≥ 2.7.8 | E-mailondersteuning voor ESP32 | E-mailnotificaties | Arduino Library Manager |
| Wire.h | Ingebouwd | I2C communicatie | Lichtsensor | Onderdeel van ESP32 core |
| BH1750 | ≥ 1.3.0 | BH1750 lichtsensor ondersteuning | Lichtsensor | Arduino Library Manager |

## Gedetailleerde Installatie-instructies

### ESP32 Core Installeren
1. Open Arduino IDE
2. Ga naar File > Preferences
3. In "Additional Boards Manager URLs" voeg toe: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Ga naar Tools > Board > Boards Manager...
5. Zoek naar "esp32"
6. Installeer "esp32 by Espressif Systems" (aanbevolen versie: 1.0.6 of hoger)

### ArduinoJson
ArduinoJson is een efficiënte JSON bibliotheek voor Arduino en embedded C++.

1. In Arduino IDE: Sketch > Include Library > Manage Libraries...
2. Zoek naar "ArduinoJson"
3. Installeer "ArduinoJson by Benoît Blanchon" (minimaal versie 6.19.4)
4. GitHub: https://github.com/bblanchon/ArduinoJson
5. Documentatie: https://arduinojson.org/

### OneWire
OneWire maakt communicatie mogelijk via het 1-Wire protocol, nodig voor de DS18B20 temperatuursensor.

1. In Arduino IDE: Sketch > Include Library > Manage Libraries...
2. Zoek naar "OneWire"
3. Installeer "OneWire by Jim Studt, Tom Pollard, ..." (minimaal versie 2.3.5)
4. GitHub: https://github.com/PaulStoffregen/OneWire

### DallasTemperature
Deze bibliotheek maakt het gebruik van Dallas temperatuursensoren eenvoudig, inclusief de DS18B20.

1. In Arduino IDE: Sketch > Include Library > Manage Libraries...
2. Zoek naar "DallasTemperature"
3. Installeer "DallasTemperature by Miles Burton, Tim Newsome, ..." (minimaal versie 3.8.0)
4. GitHub: https://github.com/milesburton/Arduino-Temperature-Control-Library

### ESP_Mail_Client (optioneel)
Nodig voor de e-mailnotificatiefunctie. Biedt SMTP en IMAP functionaliteit voor ESP32.

1. In Arduino IDE: Sketch > Include Library > Manage Libraries...
2. Zoek naar "ESP Mail Client"
3. Installeer "ESP Mail Client by Mobizt" (minimaal versie 2.7.8)
4. GitHub: https://github.com/mobizt/ESP-Mail-Client
5. Documentatie: https://github.com/mobizt/ESP-Mail-Client/wiki/Get-Started

### BH1750 (optioneel)
Nodig voor het gebruik van de BH1750 lichtsensor voor automatische LED-helderheidsregeling.

1. In Arduino IDE: Sketch > Include Library > Manage Libraries...
2. Zoek naar "BH1750"
3. Installeer "BH1750 by Christopher Laws" (minimaal versie 1.3.0)
4. GitHub: https://github.com/claws/BH1750

### Wire.h (ingebouwd)
De Wire bibliotheek wordt gebruikt voor I2C-communicatie, nodig voor de BH1750 lichtsensor.
Deze bibliotheek is standaard ingebouwd in de Arduino IDE en ESP32 core, dus geen aparte installatie vereist.

## LED Besturing

Voor LED-besturing is geen extra bibliotheek nodig omdat we directe GPIO-besturing gebruiken via de `digitalWrite()` functie. Voor geavanceerdere PWM-modulatie voor LED-helderheid gebruiken we de ingebouwde `analogWrite()` mogelijkheden van de ESP32.

### LED Besturing Configuratie
In het bestand `Settings.h`:
```cpp
#define ENABLE_LED_CONTROL true      // LED verlichting besturing
#define LED_PWM_PIN 16               // GPIO16 voor LED MOSFET aansturing
```

### Lichtsensor Configuratie
Als je de lichtsensor wilt gebruiken voor automatische helderheidsregeling:
```cpp
#define ENABLE_LIGHT_SENSOR true     // GY-302 BH1750 lichtsensor
#define I2C_SDA 21                   // GPIO21 voor I2C SDA
#define I2C_SCL 22                   // GPIO22 voor I2C SCL
```

## Bibliotheekdependencies

Sommige bibliotheken hebben onderlinge afhankelijkheden:

- DallasTemperature vereist OneWire
- BH1750 vereist Wire.h (ingebouwd)
- ESP_Mail_Client kan tijdens installatie andere afhankelijkheden downloaden

## Compatibiliteit Met ESP32 Boards

Dit project is getest met de volgende ESP32 development boards:

- ESP32 DevKit V1
- ESP32 WROOM 32
- NodeMCU-32S

Andere ESP32-gebaseerde boards zouden ook moeten werken, maar mogelijk zijn er kleine aanpassingen nodig in de pin configuratie.

## Problemen Oplossen

### Algemene Installatieproblemen
- Sluit Arduino IDE en heropen als een bibliotheek niet installeert
- Controleer of je internetverbinding actief is tijdens installatie
- In geval van foutmeldingen bij compilatie, controleer of alle bibliotheken compatible zijn met je ESP32 core versie

### Specifieke Bibliotheekproblemen

#### ArduinoJson
- Versie 6.x is niet backward-compatibel met versie 5.x
- Controleer of je code is aangepast voor ArduinoJson 6.x
- Zie migratiegids: https://arduinojson.org/v6/doc/upgrade/

#### ESP_Mail_Client
- Vereist voldoende geheugen, kan problemen geven op sommige ESP32 configuraties
- Controleer in de code of SSL-certificaatverificatie is uitgeschakeld als je verbindingsproblemen ondervindt met Gmail

#### BH1750
- Controleer het I2C-adres van je sensor (standaard 0x23 of 0x5C)
- Verifieer dat pull-up weerstanden (4.7kΩ) zijn aangesloten op de SDA en SCL lijnen
- Als je meerdere I2C-apparaten gebruikt, zorg ervoor dat er geen adresconflicten zijn

### LED Besturingsproblemen
- Bij gebruik van MOSFETs controleer of er een pull-down weerstand (10kΩ) is geplaatst tussen Gate en Source
- Bij hogere vermogen LEDs, zorg voor adequate koeling van de MOSFET
- Gebruik een gemeenschappelijke GND tussen ESP32 en LED-voeding

## Alternatieve Bibliotheken

In sommige gevallen kunnen deze alternatieven overwogen worden:

- **Voor ArduinoJson**: ESP8266_and_ESP32_OLED_driver_for_SSD1306_displays door ThingPulse
- **Voor E-mail**: EMailSender door xreef (lichter maar minder functies)
- **Voor BH1750**: hp_BH1750 door Starmbi (alternatieve implementatie)

## I2C Adres Scanner

Als je problemen hebt met het detecteren van je BH1750 lichtsensor, kun je deze code gebruiken om alle aangesloten I2C-apparaten te vinden:

```cpp
#include <Wire.h>

void setup() {
  Wire.begin(21, 22); // SDA, SCL
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  
  Serial.println("Scanning...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C apparaat gevonden op adres 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("Geen I2C apparaten gevonden\n");
  } else {
    Serial.println("Scan voltooid\n");
  }
  
  delay(5000);
}
```

## Externe Bronnen

- ESP32 Arduino Core Documentatie: https://docs.espressif.com/projects/arduino-esp32/
- ArduinoJson Documentatie: https://arduinojson.org/
- ESP Mail Client Documentatie: https://github.com/mobizt/ESP-Mail-Client/wiki
- BH1750 Datasheet: https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf
- BH1750 Bibliotheek Documentatie: https://github.com/claws/BH1750/blob/master/README.md