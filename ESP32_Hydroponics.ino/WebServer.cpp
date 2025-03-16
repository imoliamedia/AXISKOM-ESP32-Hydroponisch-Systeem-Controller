//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Web server en API module
//=========================================================================

#include "Settings.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>  // Zorg ervoor dat WiFi is geïncludeerd
#include "WebUI.h"
#include "EmailNotification.h"

// Web server instantie
WebServer server(80);

void setupWebServer() {
  // API endpoints configureren
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleGetStatus);
  server.on("/api/settings", HTTP_GET, handleGetSettings);
  server.on("/api/settings", HTTP_POST, handlePostSettings);
  server.on("/api/override", HTTP_POST, handlePostOverride);
  
  // Nieuwe API endpoints voor flowsensor
  #ifdef ENABLE_FLOW_SENSOR
    server.on("/api/config", HTTP_GET, handleGetConfig);
    server.on("/api/flowsettings", HTTP_GET, handleGetFlowSettings);
    server.on("/api/flowsettings", HTTP_POST, handlePostFlowSettings);
    server.on("/api/resetflow", HTTP_POST, handleResetFlow);
    #ifdef ENABLE_EMAIL_NOTIFICATION
      server.on("/api/testemail", HTTP_POST, handleTestEmail);
    #endif
  #endif
  
  // Optioneel: Toevoegen van een handler voor 404 Not Found
  server.onNotFound([]() {
    server.send(404, "text/plain", "Pagina niet gevonden");
  });
  
  // Start server
  server.begin();
  Serial.println("HTTP server gestart op poort 80");
  Serial.print("Je kunt de interface benaderen op: http://");
  Serial.println(WiFi.localIP());
}

void handleWebServer() {
  // Verwerk binnenkomende verzoeken
  server.handleClient();
}

// Hoofdpagina met web UI
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", WEBUI_HTML);
}

// API endpoint voor het ophalen van de huidige status
void handleGetStatus() {
  String jsonResponse;
  DynamicJsonDocument doc(512);
  
  doc["temperature"] = currentTemp;
  doc["pumpState"] = pumpState;
  doc["overrideActive"] = overrideActive;
  doc["isNightMode"] = isNachtModus;
  doc["currentDateTime"] = currentDateTime;
  
  #ifdef ENABLE_FLOW_SENSOR
    doc["flow_sensor_enabled"] = true;
    doc["flowRate"] = currentFlowRate;
    doc["totalFlowVolume"] = totalFlowVolume;
    doc["flowSensorError"] = flowSensorError;
    doc["noFlowDetected"] = noFlowDetected;
  #else
    doc["flow_sensor_enabled"] = false;
  #endif
  
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

// API endpoint voor het ophalen van instellingen
void handleGetSettings() {
  String jsonResponse;
  DynamicJsonDocument doc(512);
  
  doc["systeemnaam"] = settings.systeemnaam;
  doc["temp_laag_grens"] = settings.temp_laag_grens;
  doc["temp_hoog_grens"] = settings.temp_hoog_grens;
  doc["temp_laag_aan"] = settings.temp_laag_aan;
  doc["temp_laag_uit"] = settings.temp_laag_uit;
  doc["temp_midden_aan"] = settings.temp_midden_aan;
  doc["temp_midden_uit"] = settings.temp_midden_uit;
  doc["temp_hoog_aan"] = settings.temp_hoog_aan;
  doc["temp_hoog_uit"] = settings.temp_hoog_uit;
  doc["nacht_aan"] = settings.nacht_aan;
  doc["nacht_uit"] = settings.nacht_uit;
  
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

// API endpoint voor het opslaan van instellingen
void handlePostSettings() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen data ontvangen");
    return;
  }
  
  String jsonString = server.arg("plain");
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON: " + String(error.c_str()));
    return;
  }
  
  // Valideer instellingen
  bool validationError = false;
  String errorMessage = "";
  
  // Controleer temperatuurgrenzen
  if (doc.containsKey("temp_laag_grens") && doc.containsKey("temp_hoog_grens")) {
    if (doc["temp_laag_grens"].as<float>() >= doc["temp_hoog_grens"].as<float>()) {
      validationError = true;
      errorMessage = "Ongeldige temperatuurgrenzen: lage grens moet kleiner zijn dan hoge grens";
    }
  }
  
  // Controleer minimale cycli
  if (doc.containsKey("temp_laag_aan") && doc["temp_laag_aan"].as<int>() < 1) {
    validationError = true;
    errorMessage = "Ongeldige cyclustijd: minimum AAN tijd is 1 seconde";
  }
  
  if (validationError) {
    server.send(400, "text/plain", errorMessage);
    return;
  }
  
  // Update instellingen
  if (doc.containsKey("systeemnaam")) {
    // Veilige manier om een string naar char array te kopiëren
    const char* naam = doc["systeemnaam"].as<const char*>();
    strncpy(settings.systeemnaam, naam, sizeof(settings.systeemnaam) - 1);
    settings.systeemnaam[sizeof(settings.systeemnaam) - 1] = '\0'; // Zorg voor null-terminatie
  }
  
  // Update numerieke waarden met minimale waarde controle
  if (doc.containsKey("temp_laag_grens")) settings.temp_laag_grens = doc["temp_laag_grens"].as<float>();
  if (doc.containsKey("temp_hoog_grens")) settings.temp_hoog_grens = doc["temp_hoog_grens"].as<float>();
  if (doc.containsKey("temp_laag_aan")) settings.temp_laag_aan = max(1, doc["temp_laag_aan"].as<int>());
  if (doc.containsKey("temp_laag_uit")) settings.temp_laag_uit = max(1, doc["temp_laag_uit"].as<int>());
  if (doc.containsKey("temp_midden_aan")) settings.temp_midden_aan = max(1, doc["temp_midden_aan"].as<int>());
  if (doc.containsKey("temp_midden_uit")) settings.temp_midden_uit = max(1, doc["temp_midden_uit"].as<int>());
  if (doc.containsKey("temp_hoog_aan")) settings.temp_hoog_aan = max(1, doc["temp_hoog_aan"].as<int>());
  if (doc.containsKey("temp_hoog_uit")) settings.temp_hoog_uit = max(1, doc["temp_hoog_uit"].as<int>());
  if (doc.containsKey("nacht_aan")) settings.nacht_aan = max(1, doc["nacht_aan"].as<int>());
  if (doc.containsKey("nacht_uit")) settings.nacht_uit = max(1, doc["nacht_uit"].as<int>());
  
  // Zorg ervoor dat de magic number behouden blijft
  settings.magic = EEPROM_MAGIC;
  
  // Sla instellingen op in EEPROM
  saveSettings();
  
  Serial.println("Nieuwe instellingen ontvangen en opgeslagen");
  server.send(200, "application/json", "{\"status\":\"success\"}");
}

// API endpoint voor het schakelen van de handmatige besturing
void handlePostOverride() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen data ontvangen");
    return;
  }
  
  String jsonString = server.arg("plain");
  DynamicJsonDocument doc(128);
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON: " + String(error.c_str()));
    return;
  }
  
  // Update override status
  if (doc.containsKey("active")) {
    overrideActive = doc["active"].as<bool>();
    
    if (overrideActive && doc.containsKey("pumpState")) {
      overridePumpState = doc["pumpState"].as<bool>();
      digitalWrite(RELAY_PIN, overridePumpState ? HIGH : LOW);
      pumpState = overridePumpState;
      
      Serial.print("Handmatige override: Pomp ");
      Serial.println(overridePumpState ? "AAN" : "UIT");
    } else if (!overrideActive) {
      Serial.println("Handmatige override geannuleerd, terug naar automatische modus");
    }
  }
  
  // Reset cycleStartTime wanneer we terugkeren naar automatische modus
  if (!overrideActive) {
    cycleStartTime = millis();
  }
  
  server.send(200, "application/json", "{\"status\":\"success\"}");
}

// NIEUWE HANDLERS VOOR FLOWSENSOR
#ifdef ENABLE_FLOW_SENSOR

// API endpoint voor het ophalen van systeemconfiguratie
void handleGetConfig() {
  Serial.println("handleGetConfig aangeroepen");
  String jsonResponse;
  DynamicJsonDocument doc(256);
  
  doc["flow_sensor_enabled"] = ENABLE_FLOW_SENSOR;
  doc["email_notification_enabled"] = ENABLE_EMAIL_NOTIFICATION;
  
  serializeJson(doc, jsonResponse);
  Serial.println("Sending response: " + jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

// API endpoint voor het ophalen van flowsensor instellingen
void handleGetFlowSettings() {
  String jsonResponse;
  DynamicJsonDocument doc(512);
  
  doc["minFlowRate"] = settings.minFlowRate;
  doc["flowSensorDebug"] = settings.flowSensorDebug;
  doc["flowAlertEnabled"] = settings.flowAlertEnabled;
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    doc["emailUsername"] = settings.emailUsername;
    // Stuur wachtwoord niet terug voor veiligheid, stuur alleen een placeholder
    doc["emailPassword"] = settings.emailPassword[0] != '\0' ? "********" : "";
    doc["emailRecipient"] = settings.emailRecipient;
    doc["emailDebug"] = settings.emailDebug;
  #endif
  
  serializeJson(doc, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

// API endpoint voor het opslaan van flowsensor instellingen
void handlePostFlowSettings() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen data ontvangen");
    return;
  }
  
  String jsonString = server.arg("plain");
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON: " + String(error.c_str()));
    return;
  }
  
  // Valideer instellingen
  bool validationError = false;
  String errorMessage = "";
  
  // Controleer minimale flow rate
  if (doc.containsKey("minFlowRate") && doc["minFlowRate"].as<float>() < 0.1) {
    validationError = true;
    errorMessage = "Ongeldige flow rate: minimale waarde is 0.1 L/min";
  }
  
  if (validationError) {
    server.send(400, "text/plain", errorMessage);
    return;
  }
  
  // Update instellingen
  if (doc.containsKey("minFlowRate")) settings.minFlowRate = doc["minFlowRate"].as<float>();
  if (doc.containsKey("flowSensorDebug")) settings.flowSensorDebug = doc["flowSensorDebug"].as<bool>();
  if (doc.containsKey("flowAlertEnabled")) settings.flowAlertEnabled = doc["flowAlertEnabled"].as<bool>();
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    if (doc.containsKey("emailUsername")) {
      const char* username = doc["emailUsername"].as<const char*>();
      strncpy(settings.emailUsername, username, sizeof(settings.emailUsername) - 1);
      settings.emailUsername[sizeof(settings.emailUsername) - 1] = '\0';
    }
    
    // Update wachtwoord alleen als het niet de placeholder is
    if (doc.containsKey("emailPassword") && strcmp(doc["emailPassword"].as<const char*>(), "********") != 0) {
      const char* password = doc["emailPassword"].as<const char*>();
      strncpy(settings.emailPassword, password, sizeof(settings.emailPassword) - 1);
      settings.emailPassword[sizeof(settings.emailPassword) - 1] = '\0';
    }
    
    if (doc.containsKey("emailRecipient")) {
      const char* recipient = doc["emailRecipient"].as<const char*>();
      strncpy(settings.emailRecipient, recipient, sizeof(settings.emailRecipient) - 1);
      settings.emailRecipient[sizeof(settings.emailRecipient) - 1] = '\0';
    }
    
    if (doc.containsKey("emailDebug")) settings.emailDebug = doc["emailDebug"].as<bool>();
  #endif
  
  // Zorg ervoor dat de magic number behouden blijft
  settings.magic = EEPROM_MAGIC;
  
  // Sla instellingen op in EEPROM
  saveSettings();
  
  Serial.println("Nieuwe flowsensor instellingen opgeslagen");
  server.send(200, "application/json", "{\"status\":\"success\"}");
}

// API endpoint voor het resetten van de flow teller
void handleResetFlow() {
  resetFlowSensor();
  Serial.println("Flowsensor teller gereset");
  server.send(200, "application/json", "{\"status\":\"success\"}");
}

#ifdef ENABLE_EMAIL_NOTIFICATION
// API endpoint voor het versturen van een test e-mail
void handleTestEmail() {
  bool success = sendTestEmail();
  
  if (success) {
    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Test e-mail succesvol verzonden\"}");
  } else {
    String errorJson = "{\"status\":\"error\",\"message\":\"Kon geen test e-mail verzenden: ";
    errorJson += getLastEmailError();
    errorJson += "\"}";
    server.send(500, "application/json", errorJson);
  }
}
#endif // ENABLE_EMAIL_NOTIFICATION

#endif // ENABLE_FLOW_SENSOR