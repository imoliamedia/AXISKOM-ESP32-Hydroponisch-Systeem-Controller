# ESP32 Hydroponisch Systeem Controller

Een open-source controller voor hydroponische systemen gebaseerd op de ESP32 microcontroller, ontwikkeld als onderdeel van het AXISKOM kennisplatform voor zelfredzaamheid en zelfvoorzienend leven. Deze controller regelt de pompwerking op basis van temperatuur en tijd, en kan optioneel ook de waterstroming monitoren en LED-verlichting aansturen.

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
- **Flowsensor:** Waterstroming monitoring met de YF-S201 flowsensor (optioneel)
- **E-mail:** Waarschuwingen bij detectie van problemen (optioneel)
- **NIEUW:** LED-verlichting besturing met tijdschema (optioneel)
- **NIEUW:** Lichtsensor (BH1750) voor automatische LED-helderheid (optioneel)

## Hardware Vereisten

- ESP32 development board
- DS18B20 temperatuursensor
- Relaismodule (voor pompbesturing)
- DC waterpomp (of een ander apparaat dat je wilt aansturen)
- Voeding voor de ESP32 en pomp
- **Optioneel:** YF-S201 waterstroomsensor
- **Optioneel:** MOSFET-module voor LED-aansturing (bijv. IRLZ44N of FQP30N06L)
- **Optioneel:** LED-verlichting (12V of 24V strips)
- **Optioneel:** BH1750 lichtsensor (voor automatische LED-helderheid)
- **Optioneel:** Behuizing, aansluitklemmen, etc.

## Aansluitschema

```
ESP32 GPIO4  ──── DS18B20 Data (met 4.7kΩ pull-up weerstand naar 3.3V)
ESP32 GPIO5  ──── Relais ingang
ESP32 GPIO14 ──── YF-S201 Flowsensor signaal (gele draad) (optioneel)
ESP32 GPIO16 ──── MOSFET Gate voor LED-besturing (optioneel)
ESP32 3.3V   ──── DS18B20 VCC
ESP32 GND    ──── DS18B20 GND
ESP32 GND    ──── Relais GND
ESP32 5V     ──── Relais VCC
ESP32 5V     ──── YF-S201 VCC (rode draad) (optioneel)
ESP32 GND    ──── YF-S201 GND (zwarte draad) (optioneel)
ESP32 GND    ──── MOSFET Source (optioneel)

# Lichtsensor (optioneel)
ESP32 GPIO21 ──── BH1750 SDA
ESP32 GPIO22 ──── BH1750 SCL
ESP32 3.3V   ──── BH1750 VCC
ESP32 GND    ──── BH1750 GND
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
- **FlowSensor.h/.cpp** - Flowsensor module (optioneel)
- **EmailNotification.h/.cpp** - E-mailnotificaties (optioneel)
- **LEDControl.h/.cpp** - LED-verlichting besturing (optioneel)

## Installatie

1. Installeer de Arduino IDE
2. Installeer ESP32 board support via Boardbeheer
3. Installeer de volgende bibliotheken via Bibliothekenbeheer:
   - ArduinoJson
   - OneWire
   - DallasTemperature
   - ESP Mail Client (alleen nodig voor e-mail notificaties)
   - BH1750 (alleen nodig voor lichtsensor)
4. Clone of download deze repository
5. Open het `ESP32_Hydroponics.ino` bestand in de Arduino IDE
6. Configureer de WiFi instellingen in `SettingsImpl.cpp`
7. Controleer de pin instellingen in `Settings.h` en pas aan indien nodig
8. Activeer optionele functies in `Settings.h` indien gewenst
9. Upload de code naar je ESP32
10. Open de seriële monitor om het IP-adres te vinden
11. Navigeer naar dat IP-adres in een webbrowser om de interface te openen

# Benodigde Bibliotheken

## Basis Bibliotheken (altijd vereist)
- **WiFi.h** - Ingebouwd in ESP32 core
- **EEPROM.h** - Ingebouwd in ESP32 core
- **WebServer.h** - Ingebouwd in ESP32 core
- **ArduinoJson** - Installeren via Arduino Library Manager (min. v6.x)
- **OneWire** - Installeren via Arduino Library Manager (min. v2.3.5)
- **DallasTemperature** - Installeren via Arduino Library Manager (min. v3.8.0)
- **time.h** - Ingebouwd in ESP32 core

## Aanvullende Bibliotheken (optioneel)

### Voor E-mail Notificaties
- **ESP_Mail_Client** - Installeren via Arduino Library Manager (min. v2.x)
  - Alleen nodig als `ENABLE_EMAIL_NOTIFICATION` is ingeschakeld

### Voor Lichtsensor
- **BH1750** - Installeren via Arduino Library Manager
  - Alleen nodig als `ENABLE_LIGHT_SENSOR` is ingeschakeld

## Configuratie Aanpassen

### WiFi Instellingen

Wijzig de WiFi-instellingen in het bestand `SettingsImpl.cpp`:

```cpp
// WiFi instellingen
const char* ssid = "JouwWiFiNaam";
const char* password = "JouwWiFiWachtwoord";
```

### Statische IP-Configuratie

```cpp
// Statische IP configuratie
bool useStaticIP = true;                    // Zet op true voor vast IP, false voor DHCP
IPAddress staticIP(192, 168, 0, 21);        // Het gewenste vaste IP-adres
IPAddress gateway(192, 168, 0, 1);          // Je router/gateway IP
IPAddress subnet(255, 255, 255, 0);         // Subnet mask
IPAddress dns(192, 168, 0, 1);              // DNS server
```

### Hardware Pinnen

Als je andere pinnen gebruikt voor je sensoren of relais, pas deze aan in `Settings.h`:

```cpp
// Pindefinities
#define ONE_WIRE_BUS 4    // GPIO4 voor DS18B20 temperatuursensor
#define RELAY_PIN 5       // GPIO5 voor relais
#define FLOW_SENSOR_PIN 14 // GPIO14 voor YF-S201 flowsensor (optioneel)
#define LED_PWM_PIN 16    // GPIO16 voor LED MOSFET aansturing (optioneel)

// I2C pinnen voor lichtsensor (optioneel)
#define I2C_SDA 21        // GPIO21 voor I2C SDA
#define I2C_SCL 22        // GPIO22 voor I2C SCL
```

## Optionele Functionaliteit

Het systeem ondersteunt de volgende optionele functies die kunnen worden in- of uitgeschakeld:

- Waterstroming monitoring met de YF-S201 flowsensor
- E-mail notificaties bij problemen 
- LED-verlichting besturing met tijdschema
- Lichtsensor (BH1750) voor automatische LED-helderheid

### Functionaliteit activeren

Om een functie te activeren, pas deze instellingen aan in `Settings.h`:

```cpp
// Configuratie voor optionele functionaliteit
// TRUE = ingeschakeld, FALSE = uitgeschakeld
#define ENABLE_FLOW_SENSOR true      // Waterstroomsensor
#define ENABLE_EMAIL_NOTIFICATION true  // E-mail notificaties
#define ENABLE_LED_CONTROL true      // LED verlichting besturing
#define ENABLE_LIGHT_SENSOR false    // GY-302 BH1750 lichtsensor
```

## LED Verlichting Besturing

### Basisinstellingen
De LED-besturingsmodule biedt de volgende functies:
- Automatisch in- en uitschakelen op basis van tijd
- Instelbare helderheid (0-100%)
- Handmatige override

### Lichtsensor (optioneel)
Als je een BH1750 lichtsensor aansluit, kun je:
- LED-helderheid automatisch aanpassen op basis van omgevingslicht
- Minimale en maximale lux-waarden instellen
- LED's uitschakelen bij voldoende natuurlijk licht

### LED Hardware Aansluiten
1. Sluit een MOSFET (bijv. IRLZ44N) aan op GPIO16
2. Verbind de LED-strip met de MOSFET (Source naar GND, Drain naar LED-, LED+ naar voeding)
3. Zorg voor een gemeenschappelijke GND tussen ESP32 en LED-voeding

### LED Configureren
Je kunt de LED-verlichting configureren via het tabblad "LED Verlichting" in de webinterface:
1. Stel de tijden in waarop de LED's aan en uit moeten gaan
2. Bepaal de gewenste helderheid (0-100%)
3. Kies tussen automatische en handmatige modus
4. Als je een lichtsensor gebruikt, configureer de lux-drempelwaarden

### Lichtsensor Aansluiten
Als je automatische helderheidsregeling wilt op basis van omgevingslicht:
1. Sluit een BH1750 lichtsensor aan op de I2C-pinnen (SDA=GPIO21, SCL=GPIO22)
2. Activeer de lichtsensor in `Settings.h` door `ENABLE_LIGHT_SENSOR` op `true` te zetten
3. Configureer in de webinterface de minimale en maximale lux-waarden

## Interval en Continue Modus

De controller ondersteunt twee verschillende bedrijfsmodi voor de pomp:

### Interval Modus (Hydro Toren)
Voor hydroponische torens met cyclische besproeiing:
- Configureerbare aan/uit-tijden voor verschillende temperatuurbereiken
- Speciale nachtmodus instellingen
- Automatische selectie van cyclustijden op basis van temperatuur

### Continue Modus (NFT/DFT)
Voor Nutrient Film Technique en Deep Flow Technique systemen:
- Pomp blijft 24/7 draaien zonder onderbrekingen
- Ideaal voor NFT en DFT hydroponische systemen
- Temperatuurgebaseerde cycli worden overgeslagen

### Omschakelen tussen Modi
Je kunt eenvoudig tussen interval en continue modus schakelen via de webinterface:
1. Ga naar het tabblad "Instellingen"
2. Selecteer de gewenste modus onder "Systeem Type"
3. Klik op "Instellingen opslaan"

## Problemen oplossen

### LED's werken niet
- Controleer of de LED-functionaliteit is geactiveerd in `Settings.h`
- Controleer of de MOSFET correct is aangesloten (Gate naar GPIO16)
- Meet de spanning op de Gate van de MOSFET tijdens bediening
- Verifieer dat de LED-strip en voeding correct zijn aangesloten
- Controleer in de webinterface of de tijdsinstellingen correct zijn

### Lichtsensor geeft geen waarden
- Controleer of de sensor correct is aangesloten (SDA=GPIO21, SCL=GPIO22)
- Verifieer dat de I2C pull-up weerstanden (4.7kΩ) aanwezig zijn
- Controleer het I2C-adres van je BH1750 (standaard 0x23 of 0x5C)
- Gebruik een I2C scanner sketch om de sensor te detecteren

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

## Uitbreidingsmogelijkheden

Mogelijke toekomstige uitbreidingen:
- pH-sensor ondersteuning
- EC (geleidbaarheid) sensor integratie
- Automatische nutriëntendosering
- Waterniveau monitoring
- Integratie met andere systemen via MQTT
- Datalogging naar SD-kaart of cloud diensten

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

Dit project is gelicenseerd onder de [GPL-3.0 license](LICENSE).

## Dankwoord

- Dank aan de Arduino en ESP32 community voor de geweldige bibliotheken
- Dank aan alle bijdragers aan dit project
- Met dank aan het AXISKOM platform voor inspiratie en ondersteuning
