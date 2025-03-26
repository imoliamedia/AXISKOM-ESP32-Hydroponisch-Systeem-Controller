/*
 * WiFiManager.cpp
 *
 * Beheer van WiFi verbinding met automatisch herstel en statuscontrole
 */

#include "Settings.h"

// WiFi verbindingsvariabelen
unsigned long lastReconnectAttempt = 0;
const int reconnectInterval = 30000; // 30 seconden tussen pogingen
unsigned long wifiConnectTime = 0;   // Tijdstip van succesvolle verbinding

// Configureer en verbind met WiFi
void setupWiFi() {
  Serial.println("\nWiFi verbinding configureren...");
  
  // Ophalen van SSID en wachtwoord uit SettingsImpl.cpp
  extern const char* ssid;
  extern const char* password;
  extern bool useStaticIP;
  extern IPAddress staticIP;
  extern IPAddress gateway;
  extern IPAddress subnet;
  extern IPAddress dns;
  
  // Begin in station modus
  WiFi.mode(WIFI_STA);
  
  // Configureer statisch IP indien ingeschakeld
  if (useStaticIP) {
    Serial.println("Statisch IP geconfigureerd:");
    Serial.print("  IP:      ");
    Serial.println(staticIP.toString());
    Serial.print("  Gateway: ");
    Serial.println(gateway.toString());
    Serial.print("  Subnet:  ");
    Serial.println(subnet.toString());
    Serial.print("  DNS:     ");
    Serial.println(dns.toString());
    
    if (!WiFi.config(staticIP, gateway, subnet, dns)) {
      Serial.println("Fout bij statische IP configuratie, terugvallen op DHCP");
    }
  }
  
  // Verbinden met WiFi netwerk
  Serial.print("Verbinden met WiFi netwerk: ");
  Serial.println(ssid);
  
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  
  // Wacht maximaal 20 seconden op verbinding
  const int maxWaitTime = 20000;
  unsigned long startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < maxWaitTime) {
    delay(500);
    Serial.print(".");
  }
  
  // Controleer of we verbonden zijn
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnectTime = millis();
    Serial.println("\nVerbonden met WiFi!");
    Serial.print("IP-adres: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nKon niet verbinden met WiFi. Systeem functioneert met beperkte functionaliteit.");
    Serial.println("Zal periodiek proberen opnieuw te verbinden...");
  }
}

// Controleer WiFi verbinding en herstel indien nodig
void checkWiFiConnection() {
  // Controleer of we verbonden zijn
  if (WiFi.status() != WL_CONNECTED) {
    // Als het tijd is voor een nieuwe verbindingspoging
    if (millis() - lastReconnectAttempt > reconnectInterval) {
      Serial.println("WiFi verbinding verbroken, opnieuw verbinden...");
      
      // Probeer opnieuw verbinding te maken
      WiFi.disconnect();
      delay(1000);
      WiFi.reconnect();
      
      // Update tijdstip van laatste poging
      lastReconnectAttempt = millis();
      
      // Wacht kort om verbinding te maken
      for (int i = 0; i < 10; i++) {
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("WiFi verbinding hersteld!");
          Serial.print("IP-adres: ");
          Serial.println(WiFi.localIP());
          wifiConnectTime = millis();
          break;
        }
        delay(500);
        Serial.print(".");
      }
      
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Kon WiFi verbinding niet herstellen. Probeer later opnieuw.");
      }
    }
  }
}

// Haal WiFi-signaalsterkte op als string
String getWiFiSignalStrength() {
  if (WiFi.status() != WL_CONNECTED) {
    return "Niet verbonden";
  }
  
  int rssi = WiFi.RSSI();
  String quality;
  
  if (rssi > -50) {
    quality = "Uitstekend";
  } else if (rssi > -60) {
    quality = "Zeer goed";
  } else if (rssi > -70) {
    quality = "Goed";
  } else if (rssi > -80) {
    quality = "Redelijk";
  } else {
    quality = "Zwak";
  }
  
  return quality + " (" + String(rssi) + " dBm)";
}

// Haal WiFi verbindingsduur op
String getWiFiUptime() {
  if (WiFi.status() != WL_CONNECTED) {
    return "Niet verbonden";
  }
  
  unsigned long uptimeMillis = millis() - wifiConnectTime;
  unsigned long uptimeSeconds = uptimeMillis / 1000;
  
  unsigned long days = uptimeSeconds / 86400;
  uptimeSeconds %= 86400;
  unsigned long hours = uptimeSeconds / 3600;
  uptimeSeconds %= 3600;
  unsigned long minutes = uptimeSeconds / 60;
  uptimeSeconds %= 60;
  
  String uptime = "";
  
  if (days > 0) {
    uptime += String(days) + " dagen, ";
  }
  
  char buffer[20];
  sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, uptimeSeconds);
  uptime += buffer;
  
  return uptime;
}