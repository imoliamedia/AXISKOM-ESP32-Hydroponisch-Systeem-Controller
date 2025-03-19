# ESP32 Hydroponisch Systeem Controller

Een open-source controller voor hydroponische systemen gebaseerd op de ESP32 microcontroller, ontwikkeld als onderdeel van het AXISKOM kennisplatform voor zelfredzaamheid en zelfvoorzienend leven. Deze controller regelt de pompwerking op basis van temperatuur en tijd, en kan optioneel ook de waterstroming monitoren.

## Over AXISKOM

AXISKOM is een Nederlandstalig kennisplatform gericht op zelfredzaamheid, prepping, outdoor skills en zelfvoorzienend leven. Dit project ondersteunt de missie van AXISKOM: "Zelfredzaamheid begint bij kennis" door praktische tools te bieden voor zelfvoorzienend tuinieren en duurzame voedselproductie. Bezoek [AXISKOM.nl](https://axiskom.nl) voor meer informatie.

## Functies

- Temperatuurgestuurde pompcycli
- Instelbare aan/uit tijden per temperatuurbereik
- Speciale nachtmodus met aangepaste cycli
- Webinterface voor monitoring en bediening
- Automatische tijd/datum synchronisatie (met zomertijdondersteuning)
- Handmatige override voor de pomp
- Persistente instellingen (blijven bewaard bij stroomuitval)
- Modulaire codestructuur voor eenvoudige uitbreidingen
- **NIEUW:** Waterstroming monitoring met de YF-S201 flowsensor (optioneel)
- **NIEUW:** E-mail waarschuwingen bij detectie van problemen (optioneel)

## Hardware Vereisten

- ESP32 development board
- DS18B20 temperatuursensor
- Relaismodule (voor pompbesturing)
- DC waterpomp (of een ander apparaat dat je wilt aansturen)
- Voeding voor de ESP32 en pomp
- **NIEUW:** YF-S201 waterstroomsensor (optioneel)
- Optioneel: behuizing, aansluitklemmen, etc.

## Aansluitschema

```
ESP32 GPIO4  ──── DS18B20 Data (met 4.7kΩ pull-up weerstand naar 3.3V)
ESP32 GPIO5  ──── Relais ingang
ESP32 GPIO14 ──── YF-S201 Flowsensor signaal (gele draad) (optioneel)
ESP32 3.3V   ──── DS18B20 VCC
ESP32 GND    ──── DS18B20 GND
ESP32 GND    ──── Relais GND
ESP32 5V     ──── Relais VCC
ESP32 5V     ──── YF-S201 VCC (rode draad) (optioneel)
ESP32 GND    ──── YF-S201 GND (zwarte draad) (optioneel)
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
- **FlowSensor.h** - Header voor de flowsensor module (optioneel)
- **FlowSensor.cpp** - Implementatie van de flowsensorfuncties (optioneel)
- **EmailNotification.h** - Header voor e-mailnotificaties (optioneel)
- **EmailNotification.cpp** - Implementatie van e-mailnotificaties (optioneel)

## Installatie

1. Installeer de Arduino IDE
2. Installeer ESP32 board support via Boardbeheer
3. Installeer de volgende bibliotheken via Bibliothekenbeheer:
   - ArduinoJson
   - OneWire
   - DallasTemperature
   - ESP Mail Client (alleen nodig voor e-mail notificaties)
4. Clone of download deze repository
5. Open het `ESP32_Hydroponics.ino` bestand in de Arduino IDE
6. Configureer de WiFi instellingen in `SettingsImpl.cpp`
7. Controleer de pin instellingen in `Settings.h` en pas aan indien nodig
8. Activeer optionele functies in `Settings.h` indien gewenst (zie 'Optionele Functionaliteit')
9. Upload de code naar je ESP32
10. Open de seriële monitor om het IP-adres te vinden
11. Navigeer naar dat IP-adres in een webbrowser om de interface te openen

# Benodigde Bibliotheken

## Basis Bibliotheken (altijd vereist)
- **WiFi.h** - Ingebouwd in ESP32 core, geen aparte installatie nodig
- **EEPROM.h** - Ingebouwd in ESP32 core, geen aparte installatie nodig
- **WebServer.h** - Ingebouwd in ESP32 core, geen aparte installatie nodig
- **ArduinoJson** - Installeren via Arduino Library Manager
  - Versie: minimaal 6.x
  - Gebruikt voor: JSON-verwerking in API-responses en instellingen
- **OneWire** - Installeren via Arduino Library Manager
  - Versie: minimaal 2.3.5
  - Gebruikt voor: Communicatie met de DS18B20 temperatuursensor
- **DallasTemperature** - Installeren via Arduino Library Manager
  - Versie: minimaal 3.8.0
  - Gebruikt voor: DS18B20 temperatuursensor besturing
- **time.h** - Ingebouwd in ESP32 core, geen aparte installatie nodig

## Aanvullende Bibliotheken (optioneel, afhankelijk van geactiveerde functies)

### Voor E-mail Notificaties
- **ESP_Mail_Client** - Installeren via Arduino Library Manager
  - Versie: minimaal 2.x
  - Gebruikt voor: Het verzenden van e-mails via SMTP (Gmail)
  - Alleen nodig als `ENABLE_EMAIL_NOTIFICATION` is ingeschakeld

## Installatie van Bibliotheken

### Via Arduino Library Manager
1. Open Arduino IDE
2. Ga naar Sketch > Include Library > Manage Libraries...
3. Zoek naar de benodigde bibliotheek
4. Klik op "Install" voor de juiste versie

### Handmatige installatie (alternatief)
1. Download de ZIP van de GitHub repository van de bibliotheek
2. In Arduino IDE: Sketch > Include Library > Add .ZIP Library...
3. Selecteer het gedownloade ZIP-bestand

## Bibliotheken Compatibiliteit

| Bibliotheek | Geteste Versie | ESP32 Core Compatibiliteit |
|-------------|----------------|----------------------------|
| ArduinoJson | 6.19.4         | 1.0.6 en hoger            |
| OneWire     | 2.3.5          | Alle versies               |
| DallasTemperature | 3.9.0    | Alle versies               |
| ESP_Mail_Client | 2.7.8      | 1.0.6 en hoger            |

## Opmerkingen
- De ESP32 core (esp32 by Espressif Systems) moet geïnstalleerd zijn via de Boards Manager
- Aanbevolen versie van ESP32 core is 1.0.6 of hoger
- Alle bibliotheken zijn getest met Arduino IDE 1.8.19 en 2.x

## Configuratie Aanpassen

### WiFi Instellingen

Wijzig de WiFi-instellingen in het bestand `SettingsImpl.cpp`:

```cpp
// WiFi instellingen
const char* ssid = "JouwWiFiNaam";
const char* password = "JouwWiFiWachtwoord";
```

### Statische IP-Configuratie
Om het beheren van meerdere systemen te vereenvoudigen, kan nu een vast IP-adres worden geconfigureerd:

1. Open het bestand `SettingsImpl.cpp`
2. Zoek de statische IP-configuratie sectie
3. Stel `useStaticIP` in op `true` om een vast IP-adres te gebruiken
4. Configureer het gewenste IP-adres, gateway, subnet en DNS:

```cpp
// Statische IP configuratie
bool useStaticIP = true;                    // Zet op true voor vast IP, false voor DHCP
IPAddress staticIP(192, 168, 0, 21);        // Het gewenste vaste IP-adres
IPAddress gateway(192, 168, 0, 1);          // Je router/gateway IP
IPAddress subnet(255, 255, 255, 0);         // Subnet mask
IPAddress dns(192, 168, 0, 1);              // DNS server

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
#define FLOW_SENSOR_PIN 14 // GPIO14 voor YF-S201 flowsensor (optioneel)
```

### Nachtmodus Tijden

De nachtmodus tijden kunnen worden aangepast in `Settings.h`:

```cpp
// Constanten voor nachtmodus
const int NACHT_START_UUR = 22; // Nacht begint om 22:00
const int NACHT_EIND_UUR = 6;   // Nacht eindigt om 06:00
```

### Pompcycli Instellingen

De standaard instellingen voor de pompcycli kunnen worden aangepast in `Settings.h` of via gebruikersinterface. Deze bepalen hoe lang de pomp aan en uit is, afhankelijk van de temperatuur:

### Weergave van Pompcycli in Minuten
De gebruikersinterface is verbeterd door de pompcycli-tijden in minuten weer te geven in plaats van seconden. Dit maakt het systeem intuïtiever in gebruik:

- In de webinterface worden alle tijden nu weergegeven en ingevoerd in minuten
- Decimale waarden worden ondersteund (bijv. 0.5 minuten voor 30 seconden)
- De conversie gebeurt automatisch in de achtergrond; het systeem werkt intern nog steeds met seconden

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
  
  // Flowsensor instellingen (indien geactiveerd)
  float minFlowRate = 1.0;       // Minimale waterstroming in L/min
  bool flowAlertEnabled = true;  // E-mail alerts voor flowproblemen
  
  // E-mail notificatie instellingen (indien geactiveerd)
  char emailUsername[64] = "jouw.email@gmail.com";  // Gmail adres
  char emailPassword[64] = "jouw-app-wachtwoord";   // App-specifiek wachtwoord
  char emailRecipient[64] = "ontvanger@email.com";  // Ontvanger e-mailadres
};
```

## Optionele Functionaliteit

Het systeem ondersteunt de volgende optionele functies die kunnen worden in- of uitgeschakeld:

- Waterstroming monitoring met de YF-S201 flowsensor
- E-mail notificaties bij problemen (zoals geen waterstroming wanneer de pomp actief is)

### Functionaliteit activeren

Om een functie te activeren, volg deze stappen:

1. Open het bestand `Settings.h` in de Arduino IDE
2. Zoek naar de CONFIGURATIE sectie bovenaan het bestand
3. Verander `false` naar `true` voor de functies die je wilt activeren
4. Upload de code opnieuw naar je ESP32

Bijvoorbeeld, om de flowsensor te activeren, verander:
```cpp
#define ENABLE_FLOW_SENSOR false      // Waterstroomsensor
```
naar:
```cpp
#define ENABLE_FLOW_SENSOR true       // Waterstroomsensor
```

### E-mail Notificaties Configureren

Als je e-mailnotificaties wilt gebruiken:

1. Activeer zowel de flowsensor als e-mailnotificaties in `Settings.h`
2. Voor Gmail-gebruikers: maak een app-specifiek wachtwoord aan in je Google Account instellingen
3. Configureer je e-mailgegevens via de webinterface onder het "Flowsensor" tabblad
4. Klik op "Test E-mail Versturen" om je instellingen te testen

## Gebruiksaanwijzing

### Enkelvoudig Systeem

Na het uploaden en verbinden met WiFi kun je de webinterface openen op het IP-adres van de ESP32.

### Flowsensor

Als je de flowsensor hebt geactiveerd:

1. De gemeten waterstroming wordt weergegeven op de statuskaart bovenaan de webinterface
2. In het tabblad "Flowsensor" kun je de minimale flowrate en e-mail instellingen configureren
3. Als de pomp actief is maar er wordt geen waterstroming gedetecteerd, verschijnt er een waarschuwing
4. Indien geconfigureerd, wordt er een e-mail verzonden bij detectie van problemen

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

## Problemen oplossen

### Flowsensor detecteert geen water
- Controleer of de sensor correct is aangesloten (rood=5V, zwart=GND, geel=GPIO14)
- Controleer of de waterstroming voldoende is (>1 L/min)
- Controleer of de sensor in de juiste richting is gemonteerd (pijl in de stroomrichting)
- Probeer de drempelwaarde te verlagen in de Flowsensor instellingen

### Geen e-mail notificaties
- Controleer je e-mail instellingen in de webinterface
- Voor Gmail: zorg ervoor dat je een app-specifiek wachtwoord gebruikt
- Controleer je internetverbinding
- Activeer debug modus voor meer informatie in de seriële monitor

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