# ESP32 Hydroponisch Systeem Controller

Een open-source controller voor hydroponische systemen gebaseerd op de ESP32 microcontroller, ontwikkeld als onderdeel van het AXISKOM kennisplatform voor zelfredzaamheid en zelfvoorzienend leven. Deze controller regelt de pompwerking op basis van temperatuur en tijd.

## Over AXISKOM

AXISKOM is een Nederlandstalig kennisplatform gericht op zelfredzaamheid, prepping, outdoor skills en zelfvoorzienend leven. Dit project ondersteunt de missie van AXISKOM: "Zelfredzaamheid begint bij kennis" door praktische tools te bieden voor zelfvoorzienend tuinieren en duurzame voedselproductie.  Bezoek [AXISKOM.nl](https://axiskom.nl) voor meer informatie.

## Functies

- Temperatuurgestuurde pompcycli
- Instelbare aan/uit tijden per temperatuurbereik
- Speciale nachtmodus met aangepaste cycli
- Webinterface voor monitoring en bediening
- Automatische tijd/datum synchronisatie (met zomertijdondersteuning)
- Handmatige override voor de pomp
- Persistente instellingen (blijven bewaard bij stroomuitval)
- Modulaire codestructuur voor eenvoudige uitbreidingen

## Hardware Vereisten

- ESP32 development board
- DS18B20 temperatuursensor
- Relaismodule (voor pompbesturing)
- DC waterpomp (of een ander apparaat dat je wilt aansturen)
- Voeding voor de ESP32 en pomp
- Optioneel: behuizing, aansluitklemmen, etc.

## Aansluitschema

```
ESP32 GPIO4 ──── DS18B20 Data (met 4.7kΩ pull-up weerstand naar 3.3V)
ESP32 GPIO5 ──── Relais ingang
ESP32 3.3V  ──── DS18B20 VCC
ESP32 GND   ──── DS18B20 GND
ESP32 GND   ──── Relais GND
ESP32 5V    ──── Relais VCC
```

## Project Structuur

Het project is modulair opgezet met de volgende bestanden:
- **ESP32_Hydroponics.ino** - Hoofdbestand met setup(), loop() en globale variabelen
- **Settings.h** - Header met declaraties van instellingen en prototypes
- **SettingsImpl.cpp** - Implementatie van instellingen en configuratie
- **WiFiManager.cpp** - WiFi-verbindingsbeheer
- **TimeManager.cpp** - Tijd- en datumbeheer met NTP-synchronisatie
- **SensorControl.cpp** - Temperatuursensor en pompbesturingsfuncties
- **WebServer.cpp** - Webserver en API-endpoints
- **WebUI.h** - HTML, CSS en JavaScript voor de webinterface

## Installatie

1. Installeer de Arduino IDE
2. Installeer ESP32 board support via Boardbeheer
3. Installeer de volgende bibliotheken via Bibliothekenbeheer:
   - ArduinoJson
   - OneWire
   - DallasTemperature
4. Clone of download deze repository
5. Open het `ESP32_Hydroponics.ino` bestand in de Arduino IDE
6. Configureer de WiFi instellingen in `SettingsImpl.cpp`
7. Controleer de pin instellingen in `Settings.h` en pas aan indien nodig
8. Upload de code naar je ESP32
9. Open de seriële monitor om het IP-adres te vinden
10. Navigeer naar dat IP-adres in een webbrowser om de interface te openen

## Configuratie Aanpassen

### WiFi Instellingen

Wijzig de WiFi-instellingen in het bestand `SettingsImpl.cpp`:

```cpp
// WiFi instellingen
const char* ssid = "JouwWiFiNaam";
const char* password = "JouwWiFiWachtwoord";
```

### Tijdzone en NTP Server

Je kunt de tijdzone en NTP server configureren in `SettingsImpl.cpp`:

```cpp
// NTP server configuratie
const char* ntpServer = "pool.ntp.org";
const char* timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Centraal-Europese tijd met zomertijd
```

### Hardware Pinnen

Als je andere pinnen gebruikt voor je sensoren of relais, pas deze aan in `Settings.h`:

```cpp
// Pindefinities voor ESP32
#define ONE_WIRE_BUS 4    // GPIO4 voor DS18B20 temperatuursensor
#define RELAY_PIN 5       // GPIO5 voor relais
```

### Nachtmodus Tijden

De nachtmodus tijden kunnen worden aangepast in `Settings.h`:

```cpp
// Constanten voor nachtmodus
const int NACHT_START_UUR = 22; // Nacht begint om 22:00
const int NACHT_EIND_UUR = 6;   // Nacht eindigt om 06:00
```

### Pompcycli Instellingen

De standaard instellingen voor de pompcycli kunnen worden aangepast in `Settings.h`. Deze bepalen hoe lang de pomp aan en uit is, afhankelijk van de temperatuur:

```cpp
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
```

Deze waarden kunnen ook via de webinterface worden aangepast onder het tabblad "Instellingen", maar het aanpassen van de standaardwaarden in de code is handig als je meerdere systemen met dezelfde basisinstellingen wilt inrichten.

## Gebruiksaanwijzing

### Enkelvoudig Systeem

Na het uploaden en verbinden met WiFi kun je de webinterface openen op het IP-adres van de ESP32.

### Meerdere Systemen

Als je meerdere hydroponische systemen gebruikt, bieden we een Master Dashboard HTML-bestand (ESP32Hydro.html) waarmee je al je systemen op één pagina kunt beheren. Je kunt dit dashboard vinden in de GitHub repository:

1. Download het `ESP32Hydro.html` bestand van de repository
2. Open het bestand in een webbrowser
3. Klik op "+ Nieuw Systeem" om je systemen toe te voegen
4. Vul de systeemnaam en het IP-adres in voor elk van je systemen
5. Alle systemen worden op het dashboard weergegeven en kunnen van daaruit worden beheerd

Het dashboard slaat je systemen lokaal op, zodat je ze niet opnieuw hoeft toe te voegen wanneer je de pagina later opnieuw bezoekt.

### Bediening

Op het tabblad "Bediening" kun je:
- De huidige temperatuur, tijd en pompstatus zien
- De pomp handmatig AAN of UIT schakelen
- Terug naar automatische modus gaan

### Instellingen

Op het tabblad "Instellingen" kun je:
- De systeemnaam aanpassen
- Temperatuurgrenzen instellen
- Aan/uit tijden voor elke temperatuurreeks configureren
- Nacht cyclustijden instellen

## AXISKOM Integratie

Dit project past binnen de volgende categorieën van het AXISKOM platform:

### Zelfvoorzienend
- **Tuinieren**: Optimaliseer je eigen voedselproductie met hydroponische systemen
- **Energie & Water**: Efficiënt waterbeheer voor duurzame tuinbouw
- **Opslag & Bewaren**: Automatisering voor consistente groeiomstandigheden

### Tools
Deze controller kan worden gezien als een praktisch hulpmiddel binnen de AXISKOM tools-categorie, vergelijkbaar met de andere calculators en planners op het platform.

## Zelf uitbreiden

De code is modulair opgezet om eenvoudig uitbreidingen mogelijk te maken. Nieuwe functies kunnen worden toegevoegd zonder de kernfunctionaliteit te verstoren.

Om nieuwe functionaliteit toe te voegen:
1. Creëer een nieuwe module (bijvoorbeeld PHSensor.cpp)
2. Voeg functieprototypes toe aan `Settings.h`
3. Voeg je initialisatiecode toe in `setup()`
4. Voeg je update/loop code toe in `loop()`
5. Update de webinterface in `WebUI.h` waar nodig

Mogelijke uitbreidingen:
- pH-sensor ondersteuning
- EC (geleidbaarheid) sensor
- LED-verlichting besturing
- Voedingsdoseersysteem
- Waterniveau monitoring
- Datalogging naar SD-kaart of externe service
- Integratie met andere AXISKOM tools zoals de Moestuin Planner

## Community

We moedigen gebruikers aan om ervaringen en verbeteringen te delen met de AXISKOM community. Bezoek [AXISKOM.nl](https://axiskom.nl) voor meer informatie over zelfredzaamheid en zelfvoorzienend leven.

## Bijdragen

Bijdragen zijn van harte welkom! Als je wilt bijdragen aan dit project:
1. Fork deze repository
2. Maak een feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit je wijzigingen (`git commit -m 'Add some AmazingFeature'`)
4. Push naar de branch (`git push origin feature/AmazingFeature`)
5. Open een Pull Request

## Licentie

Dit project is beschikbaar onder de MIT-licentie. Zie het `LICENSE` bestand voor meer informatie.

## Dankwoord

- Dank aan de Arduino en ESP32 community voor de geweldige bibliotheken
- Dank aan alle bijdragers aan dit project
- Met dank aan het AXISKOM platform voor inspiratie en ondersteuning