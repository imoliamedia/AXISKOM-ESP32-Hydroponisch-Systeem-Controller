/*
 * WebServer.cpp
 *
 * Implementatie van de webserver voor de ESP32 Hydroponisch Systeem Controller
 */

#include "Settings.h"
#include "WebUI.h"

// Webserver instance
WebServer server(80);

// Functie declaraties voor handlers
void handleRoot();
void handleGetStatus();
void handleGetSettings();
void handlePostSettings();
void handlePostOverride();
void handleGetConfig();

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
  void handleGetFlowStatus();
  void handleGetFlowSettings();
  void handlePostFlowSettings();
  void handleResetFlow();
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    void handleTestEmail();
  #endif
#endif

#ifdef ENABLE_LED_CONTROL
  void handleGetLEDStatus();
  void handleGetLEDSettings();
  void handlePostLEDSettings();
  void handlePostLEDOverride();
#endif

// Hulpfuncties
String secondsToTimeString(unsigned long seconds);
String getWiFiSignalStrength();
String getWiFiUptime();

// Webserver instellen
void setupWebServer() {
  Serial.println("Webserver configureren...");
  
  // API endpoints voor basisbesturing
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleGetStatus);
  server.on("/api/settings", HTTP_GET, handleGetSettings);
  server.on("/api/settings", HTTP_POST, handlePostSettings);
  server.on("/api/override", HTTP_POST, handlePostOverride);
  server.on("/api/config", HTTP_GET, handleGetConfig);
  
  // Optionele modules API endpoints
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
    server.on("/api/flowstatus", HTTP_GET, handleGetFlowStatus);
    server.on("/api/flowsettings", HTTP_GET, handleGetFlowSettings);
    server.on("/api/flowsettings", HTTP_POST, handlePostFlowSettings);
    server.on("/api/resetflow", HTTP_POST, handleResetFlow);
    
    #if defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true
      server.on("/api/testemail", HTTP_POST, handleTestEmail);
    #endif
  #endif
  
  #ifdef ENABLE_LED_CONTROL
    server.on("/api/led/status", HTTP_GET, handleGetLEDStatus);
    server.on("/api/led/settings", HTTP_GET, handleGetLEDSettings);
    server.on("/api/led/settings", HTTP_POST, handlePostLEDSettings);
    server.on("/api/led/override", HTTP_POST, handlePostLEDOverride);
  #endif
  
  // Start webserver
  server.begin();
  Serial.println("HTTP server gestart");
}

// Verwerk webserver verzoeken
void handleWebClient() {
  server.handleClient();
}

// Hoofdpagina handler
void handleRoot() {
  server.send(200, "text/html", WEBUI_HTML);
}

// Configuratie informatie ophalen
void handleGetConfig() {
  // Maak JSON object voor configuratie
  DynamicJsonDocument doc(512);
  
  doc["temp_sensor_enabled"] = true; // Altijd ingeschakeld
  
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
    doc["flow_sensor_enabled"] = true;
  #else
    doc["flow_sensor_enabled"] = false;
  #endif
  
  #if defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_FLOW_SENSOR == true && ENABLE_EMAIL_NOTIFICATION == true
    doc["email_notification_enabled"] = true;
  #else
    doc["email_notification_enabled"] = false;
  #endif
  
  #if defined(ENABLE_LED_CONTROL) && ENABLE_LED_CONTROL == true
    doc["led_control_enabled"] = true;
    
    #if defined(ENABLE_LIGHT_SENSOR) && ENABLE_LIGHT_SENSOR == true
      doc["light_sensor_enabled"] = true;
    #else
      doc["light_sensor_enabled"] = false;
    #endif
  #else
    doc["led_control_enabled"] = false;
    doc["light_sensor_enabled"] = false;
  #endif
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Systeemstatus ophalen
void handleGetStatus() {
  String response = getSystemStatusJson();
  server.send(200, "application/json", response);
}

// Systeeminstellingen ophalen
void handleGetSettings() {
  DynamicJsonDocument doc(1024);
  
  // Basisinstellingen
  doc["systeemnaam"] = settings.systeemnaam;
  doc["temp_laag_grens"] = settings.temp_laag_grens;
  doc["temp_hoog_grens"] = settings.temp_hoog_grens;
  doc["continuModus"] = settings.continuModus;
  
  // Cyclustijden (in seconden)
  doc["temp_laag_aan"] = settings.temp_laag_aan;
  doc["temp_laag_uit"] = settings.temp_laag_uit;
  doc["temp_midden_aan"] = settings.temp_midden_aan;
  doc["temp_midden_uit"] = settings.temp_midden_uit;
  doc["temp_hoog_aan"] = settings.temp_hoog_aan;
  doc["temp_hoog_uit"] = settings.temp_hoog_uit;
  doc["nacht_aan"] = settings.nacht_aan;
  doc["nacht_uit"] = settings.nacht_uit;
  
  // Huidige cyclustijden
  doc["currentCycleOn"] = currentCycleOn;
  doc["currentCycleOff"] = currentCycleOff;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Instellingen opslaan
void handlePostSettings() {
  // Controleer of er JSON data is ontvangen
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen JSON data ontvangen");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(1024);
  
  // Probeer JSON te parsen
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON data: " + String(error.c_str()));
    return;
  }
  
  // Update systeemnaam
  if (doc.containsKey("systeemnaam")) {
    const char* newName = doc["systeemnaam"];
    strncpy(settings.systeemnaam, newName, sizeof(settings.systeemnaam) - 1);
    settings.systeemnaam[sizeof(settings.systeemnaam) - 1] = '\0';
  }
  
  // Update temperatuurgrenzen
  if (doc.containsKey("temp_laag_grens")) {
    settings.temp_laag_grens = doc["temp_laag_grens"];
  }
  
  if (doc.containsKey("temp_hoog_grens")) {
    settings.temp_hoog_grens = doc["temp_hoog_grens"];
  }
  
  // Update pompcycli tijden
  if (doc.containsKey("temp_laag_aan")) {
    settings.temp_laag_aan = doc["temp_laag_aan"];
  }
  
  if (doc.containsKey("temp_laag_uit")) {
    settings.temp_laag_uit = doc["temp_laag_uit"];
  }
  
  if (doc.containsKey("temp_midden_aan")) {
    settings.temp_midden_aan = doc["temp_midden_aan"];
  }
  
  if (doc.containsKey("temp_midden_uit")) {
    settings.temp_midden_uit = doc["temp_midden_uit"];
  }
  
  if (doc.containsKey("temp_hoog_aan")) {
    settings.temp_hoog_aan = doc["temp_hoog_aan"];
  }
  
  if (doc.containsKey("temp_hoog_uit")) {
    settings.temp_hoog_uit = doc["temp_hoog_uit"];
  }
  
  if (doc.containsKey("nacht_aan")) {
    settings.nacht_aan = doc["nacht_aan"];
  }
  
  if (doc.containsKey("nacht_uit")) {
    settings.nacht_uit = doc["nacht_uit"];
  }
  
  // Update bedrijfsmodus
  if (doc.containsKey("continuModus")) {
    settings.continuModus = doc["continuModus"];
  }
  
  // Sla instellingen op in EEPROM
  saveSettings();
  
  // Bereken nieuwe pompcyclustijden
  updatePumpCycleTimes();
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["message"] = "Instellingen opgeslagen";
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}

// Handmatige bediening verwerken
void handlePostOverride() {
  // Controleer of er JSON data is ontvangen
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen JSON data ontvangen");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(256);
  
  // Probeer JSON te parsen
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON data: " + String(error.c_str()));
    return;
  }
  
  // Verwerk override opdracht
  if (doc.containsKey("active")) {
    bool active = doc["active"];
    
    if (active) {
      // Activeer handmatige bediening
      if (doc.containsKey("pumpState")) {
        bool pumpState = doc["pumpState"];
        manualOverride = true;
        setPumpManual(pumpState);
      }
    } else {
      // Terug naar automatische modus
      setPumpAuto();
    }
  }
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["overrideActive"] = manualOverride;
  responseDoc["pumpState"] = pumpActive;
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}

// Genereer JSON met systeemstatus
String getSystemStatusJson() {
  DynamicJsonDocument doc(1024);
  
  // Basisstatus
  doc["temperature"] = currentTemp;
  doc["pumpState"] = pumpActive;
  doc["overrideActive"] = manualOverride;
  doc["currentDateTime"] = getFullDateTimeString();
  doc["isNightMode"] = isNightMode();
  doc["continuModus"] = settings.continuModus;
  
  // Cyclustijden
  doc["currentCycleOn"] = currentCycleOn;
  doc["currentCycleOff"] = currentCycleOff;
  
  // Wifi informatie
  doc["wifiSignal"] = getWiFiSignalStrength();
  doc["wifiUptime"] = getWiFiUptime();
  
  // Pomp statistieken
  doc["pumpRuntime"] = secondsToTimeString(pumpRunTime);
  
  // Voeg flowsensor data toe indien ingeschakeld
  #if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
    doc["flow_sensor_enabled"] = true;
    doc["flowRate"] = flowRate;
    doc["totalFlowVolume"] = totalLiters;
    doc["noFlowDetected"] = !flowOk && pumpActive;
  #else
    doc["flow_sensor_enabled"] = false;
  #endif
  
  // Voeg LED status toe indien ingeschakeld
  #ifdef ENABLE_LED_CONTROL
    doc["led_control_enabled"] = true;
    doc["ledIsOn"] = ledIsOn;
    doc["ledBrightness"] = currentLEDBrightness;
    
    #ifdef ENABLE_LIGHT_SENSOR
      doc["light_sensor_enabled"] = true;
      doc["currentLuxLevel"] = currentLuxLevel;
    #else
      doc["light_sensor_enabled"] = false;
    #endif
  #else
    doc["led_control_enabled"] = false;
  #endif
  
  String response;
  serializeJson(doc, response);
  return response;
}

// Converteer seconden naar leesbare tijd
String secondsToTimeString(unsigned long seconds) {
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  char buffer[50];
  
  if (days > 0) {
    sprintf(buffer, "%lu dagen, %02lu:%02lu:%02lu", days, hours, minutes, seconds);
  } else {
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  }
  
  return String(buffer);
}

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true
// Flowsensor status ophalen
void handleGetFlowStatus() {
  String response = getFlowStatusJson();
  server.send(200, "application/json", response);
}

// Flowsensor instellingen ophalen
void handleGetFlowSettings() {
  DynamicJsonDocument doc(512);
  
  doc["minFlowRate"] = settings.minFlowRate;
  doc["flowAlertEnabled"] = settings.flowAlertEnabled;
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    doc["emailUsername"] = settings.emailUsername;
    doc["emailPassword"] = ""; // Niet het wachtwoord terugsturen
    doc["emailRecipient"] = settings.emailRecipient;
  #endif
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Flowsensor instellingen opslaan
void handlePostFlowSettings() {
  // Controleer of er JSON data is ontvangen
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen JSON data ontvangen");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(512);
  
  // Probeer JSON te parsen
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON data: " + String(error.c_str()));
    return;
  }
  
  // Update flowsensor instellingen
  if (doc.containsKey("minFlowRate")) {
    settings.minFlowRate = doc["minFlowRate"];
  }
  
  if (doc.containsKey("flowAlertEnabled")) {
    settings.flowAlertEnabled = doc["flowAlertEnabled"];
  }
  
  #ifdef ENABLE_EMAIL_NOTIFICATION
    // Update e-mail instellingen
    if (doc.containsKey("emailUsername")) {
      const char* username = doc["emailUsername"];
      strncpy(settings.emailUsername, username, sizeof(settings.emailUsername) - 1);
      settings.emailUsername[sizeof(settings.emailUsername) - 1] = '\0';
    }
    
    if (doc.containsKey("emailPassword") && strlen(doc["emailPassword"]) > 0) {
      const char* password = doc["emailPassword"];
      strncpy(settings.emailPassword, password, sizeof(settings.emailPassword) - 1);
      settings.emailPassword[sizeof(settings.emailPassword) - 1] = '\0';
    }
    
    if (doc.containsKey("emailRecipient")) {
      const char* recipient = doc["emailRecipient"];
      strncpy(settings.emailRecipient, recipient, sizeof(settings.emailRecipient) - 1);
      settings.emailRecipient[sizeof(settings.emailRecipient) - 1] = '\0';
    }
  #endif
  
  // Sla instellingen op in EEPROM
  saveSettings();
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["message"] = "Flowsensor instellingen opgeslagen";
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}

// Reset flowsensor teller
void handleResetFlow() {
  // Reset totaal aantal liters
  totalLiters = 0;
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["message"] = "Flow teller gereset";
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true && defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true
// Test e-mail versturen
void handleTestEmail() {
  bool success = sendTestEmail();
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  
  if (success) {
    responseDoc["status"] = "success";
    responseDoc["message"] = "Test e-mail succesvol verzonden";
  } else {
    responseDoc["status"] = "error";
    responseDoc["message"] = "Kon geen test e-mail versturen";
  }
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}
#endif
#endif

#ifdef ENABLE_LED_CONTROL
// LED status ophalen
void handleGetLEDStatus() {
  DynamicJsonDocument doc(512);
  
  doc["ledIsOn"] = ledIsOn;
  doc["currentBrightness"] = currentLEDBrightness;
  doc["autoMode"] = settings.ledAutoMode;
  
  #ifdef ENABLE_LIGHT_SENSOR
    doc["sensorMode"] = settings.ledSensorMode;
    doc["currentLuxLevel"] = currentLuxLevel;
  #endif
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// LED instellingen ophalen
void handleGetLEDSettings() {
  DynamicJsonDocument doc(512);
  
  doc["startHour"] = settings.ledStartHour;
  doc["endHour"] = settings.ledEndHour;
  doc["brightness"] = settings.ledBrightness;
  doc["autoMode"] = settings.ledAutoMode;
  
  #ifdef ENABLE_LIGHT_SENSOR
    doc["sensorMode"] = settings.ledSensorMode;
    doc["minLux"] = settings.ledMinLux;
    doc["maxLux"] = settings.ledMaxLux;
    doc["readInterval"] = settings.ledSensorReadInterval;
  #endif
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// LED instellingen opslaan
void handlePostLEDSettings() {
  // Controleer of er JSON data is ontvangen
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen JSON data ontvangen");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(512);
  
  // Probeer JSON te parsen
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON data: " + String(error.c_str()));
    return;
  }
  
  // Update LED instellingen
  if (doc.containsKey("autoMode")) {
    settings.ledAutoMode = doc["autoMode"];
  }
  
  if (doc.containsKey("startHour")) {
    settings.ledStartHour = doc["startHour"];
  }
  
  if (doc.containsKey("endHour")) {
    settings.ledEndHour = doc["endHour"];
  }
  
  if (doc.containsKey("brightness")) {
    settings.ledBrightness = doc["brightness"];
  }
  
  // Lichtsensor instellingen indien beschikbaar
  #ifdef ENABLE_LIGHT_SENSOR
    if (doc.containsKey("sensorMode")) {
      settings.ledSensorMode = doc["sensorMode"];
    }
    
    if (doc.containsKey("minLux")) {
      settings.ledMinLux = doc["minLux"];
    }
    
    if (doc.containsKey("maxLux")) {
      settings.ledMaxLux = doc["maxLux"];
    }
  #endif
  
  // Sla instellingen op in EEPROM
  saveSettings();
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["message"] = "LED instellingen opgeslagen";
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}

// LED handmatige bediening
void handlePostLEDOverride() {
  // Controleer of er JSON data is ontvangen
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Geen JSON data ontvangen");
    return;
  }
  
  String jsonStr = server.arg("plain");
  DynamicJsonDocument doc(256);
  
  // Probeer JSON te parsen
  DeserializationError error = deserializeJson(doc, jsonStr);
  if (error) {
    server.send(400, "text/plain", "Ongeldige JSON data: " + String(error.c_str()));
    return;
  }
  
  bool overrideActive = false;
  int brightness = 0;
  
  if (doc.containsKey("active")) {
    overrideActive = doc["active"];
  }
  
  if (doc.containsKey("brightness")) {
    brightness = doc["brightness"];
  }
  
  if (overrideActive) {
    setLEDOverride(true, brightness);
  } else {
    cancelLEDOverride();
  }
  
  // Stuur bevestiging
  DynamicJsonDocument responseDoc(256);
  responseDoc["status"] = "success";
  responseDoc["ledIsOn"] = ledIsOn;
  responseDoc["currentBrightness"] = currentLEDBrightness;
  
  String response;
  serializeJson(responseDoc, response);
  server.send(200, "application/json", response);
}
#endif