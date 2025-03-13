//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Tijd en datum module
//=========================================================================

#include "Settings.h"
#include <time.h>

// Maximale wachttijd voor tijdsynchronisatie in milliseconden
#define TIME_SYNC_TIMEOUT 10000

// Tijd sinds de laatste succesvolle tijdupdate
unsigned long lastTimeSync = 0;

void setupTimeSync() {
  Serial.println("NTP tijdsynchronisatie starten...");
  
  // Configureer de tijdserver met zomertijd ondersteuning
  configTime(0, 0, ntpServer); // Eerst UTC tijd instellen
  setenv("TZ", timezone, 1);   // Daarna tijdzone instellen met zomertijd
  tzset();
  
  // Wacht op initiële synchronisatie maar niet te lang
  unsigned long startAttempt = millis();
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo) && (millis() - startAttempt < TIME_SYNC_TIMEOUT)) {
    Serial.print(".");
    delay(500);
  }
  
  if (getLocalTime(&timeinfo)) {
    Serial.println("Tijdsynchronisatie geslaagd!");
    lastTimeSync = millis();
  } else {
    Serial.println("Tijdsynchronisatie niet geslaagd. Controller zal later opnieuw proberen.");
  }
}

void updateCurrentTime() {
  struct tm timeinfo;
  
  if (getLocalTime(&timeinfo)) {
    // Formatteer de tijd naar een leesbare string
    strftime(currentDateTime, sizeof(currentDateTime), "%d-%m-%Y %H:%M:%S", &timeinfo);
    Serial.print("Huidige tijd: ");
    Serial.println(currentDateTime);
    
    // Update de geslaagde synchronisatietijd
    lastTimeSync = millis();
    
    // Check of het nu nacht is
    isNachtModus = checkIfNight();
    Serial.println(isNachtModus ? "Het is nacht" : "Het is dag");
  } else {
    // Als er te lang geen synchronisatie is geweest, toon een waarschuwing
    if (millis() - lastTimeSync > 3600000) { // 1 uur
      Serial.println("WAARSCHUWING: Geen tijdsynchronisatie in meer dan een uur!");
      strcpy(currentDateTime, "Tijd niet beschikbaar (sync >1u geleden)");
    } else {
      Serial.println("Tijdsynchronisatie mislukt, gebruik laatste bekende tijd");
    }
  }
}

bool checkIfNight() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false; // Als tijd niet beschikbaar is, ga uit van dag
  }
  
  int currentHour = timeinfo.tm_hour;
  
  // Het is nacht als het uur >= NACHT_START_UUR of < NACHT_EIND_UUR
  return (currentHour >= NACHT_START_UUR || currentHour < NACHT_EIND_UUR);
}