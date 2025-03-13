//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// WiFi beheer module
//=========================================================================

#include "Settings.h"
#include <WiFi.h>

// Maximum pogingen om te verbinden met WiFi voor we opgeven
#define MAX_WIFI_CONNECT_ATTEMPTS 20
// Tijd in ms tussen nieuwe verbindingspogingen
#define WIFI_RECONNECT_DELAY 30000

// Bijhoudt wanneer de laatste reconnect poging was
unsigned long lastWiFiReconnectAttempt = 0;

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Verbinden met WiFi");
  
  // Probeer MAX_WIFI_CONNECT_ATTEMPTS keer te verbinden met 500ms tussenpozen
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_CONNECT_ATTEMPTS) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Verbonden met WiFi. IP adres: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Kon niet verbinden met WiFi. Controleer SSID en wachtwoord.");
    Serial.println("De controller zal proberen om later opnieuw te verbinden.");
  }
}

void checkWiFiConnection() {
  // Als WiFi-verbinding verloren is en er is genoeg tijd verstreken sinds de laatste poging
  if (WiFi.status() != WL_CONNECTED && (millis() - lastWiFiReconnectAttempt > WIFI_RECONNECT_DELAY)) {
    Serial.println("WiFi-verbinding verloren, probeer opnieuw te verbinden");
    
    // Onthoud wanneer we de laatste poging hebben gedaan
    lastWiFiReconnectAttempt = millis();
    
    // Restart WiFi
    WiFi.disconnect();
    delay(500);
    WiFi.begin(ssid, password);
    
    // Wacht kort om direct een eerste check te doen
    delay(2000);
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Opnieuw verbonden met WiFi. IP adres: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Verbinden met WiFi mislukt. Nieuwe poging over 30 seconden.");
    }
  }
}