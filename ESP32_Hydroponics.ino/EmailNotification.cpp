/*
 * ESP32 Hydroponisch Systeem Controller
 * 
 * Copyright (C) 2024 AXISKOM
 * Website: https://axiskom.nl
 * 
 * Dit programma is vrije software: je mag het herdistribueren en/of wijzigen
 * onder de voorwaarden van de GNU General Public License zoals gepubliceerd door
 * de Free Software Foundation, ofwel versie 3 van de licentie, of
 * (naar jouw keuze) een latere versie.
 * 
 * Deze software is ontwikkeld als onderdeel van het AXISKOM kennisplatform
 * voor zelfredzaamheid en zelfvoorzienend leven.
 *
 * EmailNotification.cpp
 *
 * Implementatie van de e-mail notificatie module voor waarschuwingen
 */

#include "EmailNotification.h"

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true && defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true

// E-mail client instances
SMTPSession smtp;
Session_Config config;

// E-mail variabelen
bool emailClientReady = false;
unsigned long lastEmailSent = 0;
unsigned long emailServiceStartTime = 0;
String lastEmailError = "";
const unsigned long MIN_EMAIL_INTERVAL = 300000; // 5 minuten tussen e-mails

// Constanten voor Gmail SMTP-server
#define GMAIL_SMTP_SERVER "smtp.gmail.com"
#define GMAIL_SMTP_PORT 465

// Initialiseer e-mail notificatie
void setupEmailNotification() {
  Serial.println("E-mail notificatie module initialiseren");
  
  // Controleer of e-mailinstellingen zijn geconfigureerd
  if (strlen(settings.emailUsername) < 5 || strlen(settings.emailPassword) < 5 || 
      strlen(settings.emailRecipient) < 5) {
    Serial.println("FOUT: E-mailinstellingen niet correct geconfigureerd");
    emailClientReady = false;
    lastEmailError = "E-mailinstellingen niet correct geconfigureerd";
    return;
  }

  // Configureer de sessie
  config.server.host_name = GMAIL_SMTP_SERVER;
  config.server.port = GMAIL_SMTP_PORT;
  config.login.email = settings.emailUsername;
  config.login.password = settings.emailPassword;
  config.login.user_domain = "ESP32_Hydro";

  // Configureer tijdsynchronisatie
  config.time.ntp_server = "pool.ntp.org,time.nist.gov";
  config.time.gmt_offset = 1; // CET tijdzone (Nederland)
  config.time.day_light_offset = 1; // Zomertijd correctie

  // Debug modus instellen als nodig
  if (settings.emailDebug) {
    smtp.debug(1);
  }
  
  emailClientReady = true;
  emailServiceStartTime = millis();
  Serial.println("E-mail client geïnitialiseerd");
}

// Verkrijg de laatste e-mail foutmelding
String getLastEmailError() {
  if (lastEmailError.length() > 0) {
    return lastEmailError;
  }
  
  if (!emailClientReady) return "E-mail client niet geïnitialiseerd";
  if (millis() - emailServiceStartTime < 120000) return "Systeem nog in opstarttijd";
  if (millis() - lastEmailSent < MIN_EMAIL_INTERVAL) return "Te korte tijd sinds laatste e-mail";
  return "Onbekende fout";
}

// Stuur waarschuwing bij flowprobleem
bool sendFlowAlertEmail() {
  // Deze functie wordt alleen aangeroepen als er een flowprobleem is
  String subject = "WAARSCHUWING: Geen waterstroming in ";
  subject += settings.systeemnaam;
  
  String message = "Er is geen waterstroming gedetecteerd terwijl de pomp aan staat!\n\n";
  message += "Controleer onmiddellijk op mogelijke problemen zoals:\n";
  message += "- Verstopte leidingen\n";
  message += "- Lucht in het systeem\n";
  message += "- Pomp defect\n";
  message += "- Waterreservoir leeg\n\n";
  message += "Dit kan schade veroorzaken aan je planten of systeemcomponenten.";
  
  bool result = sendEmailAlert(subject.c_str(), message.c_str());
  if (result) {
    Serial.println("Flow probleem e-mail verzonden");
  } else {
    Serial.println("Kon geen flow probleem e-mail verzenden - Reden: " + getLastEmailError());
  }
  return result;
}

// Verstuur een e-mailalert met het opgegeven onderwerp en bericht
bool sendEmailAlert(const char* subject, const char* message) {
  // Reset de laatste foutmelding
  lastEmailError = "";
  
  // Test internetverbinding
  if (WiFi.status() != WL_CONNECTED) {
    lastEmailError = "Geen WiFi-verbinding";
    Serial.println("FOUT: Geen WiFi-verbinding, kan geen e-mail versturen");
    return false;
  }
  
  if (!emailClientReady) {
    lastEmailError = "E-mail client niet geïnitialiseerd";
    Serial.println("FOUT: E-mail client niet geïnitialiseerd");
    return false;
  }
  
  // Wacht ten minste 2 minuten na opstarten voordat de eerste waarschuwing wordt verzonden
  if (millis() - emailServiceStartTime < 120000) {
    lastEmailError = "Systeem nog in opstarttijd";
    Serial.println("WAARSCHUWING: E-mail service nog in opstarttijd, wacht nog even");
    return false;
  }
  
  // Controleer of er niet te snel achter elkaar e-mails worden verstuurd
  if (millis() - lastEmailSent < MIN_EMAIL_INTERVAL) {
    lastEmailError = "Te korte tijd sinds laatste e-mail";
    Serial.println("WAARSCHUWING: Te veel e-mails in korte tijd, wacht minimaal 5 minuten tussen notificaties");
    return false;
  }
  
  // Bereid het bericht voor
  SMTP_Message emailMessage;
  
  // Stel de bericht headers in
  emailMessage.sender.name = "ESP32 Hydro Controller";
  emailMessage.sender.email = settings.emailUsername;
  emailMessage.subject = subject;
  emailMessage.addRecipient("Hydroponisch Systeem", settings.emailRecipient);
  
  // Haal datum en tijd op
  String currentDateTime = getFullDateTimeString();
  
  // Maak de inhoud van het bericht
  String textContent = String(message);
  textContent += "\n\n---------------------------\n";
  textContent += "Systeem: ";
  textContent += settings.systeemnaam;
  textContent += "\nTijdstip: ";
  textContent += currentDateTime;
  textContent += "\nHuidige temperatuur: ";
  textContent += String(currentTemp);
  textContent += "°C\n";
  
  #ifdef ENABLE_FLOW_SENSOR
  textContent += "Huidige waterstroming: ";
  textContent += String(flowRate);
  textContent += " L/min\n";
  textContent += "Totaal doorgestroomd: ";
  textContent += String(totalLiters);
  textContent += " L\n";
  #endif
  
  // Stel de tekst in
  emailMessage.text.content = textContent.c_str();
  emailMessage.text.charSet = "utf-8";
  emailMessage.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  emailMessage.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  
  // Verbind met de server en verzend de e-mail
  Serial.println("E-mail verzenden...");
  
  // Verbind met de server
  if (!smtp.connect(&config)) {
    lastEmailError = "Kan niet verbinden met e-mailserver";
    Serial.println("FOUT: Kan niet verbinden met e-mailserver");
    return false;
  }
  
  // Verzend het bericht
  if (!MailClient.sendMail(&smtp, &emailMessage)) {
    lastEmailError = String(smtp.errorReason());
    Serial.println("FOUT: Verzenden e-mail mislukt");
    Serial.println(smtp.errorReason());
    return false;
  }
  
  Serial.println("E-mail succesvol verzonden");
  lastEmailSent = millis();
  return true;
}

// Verstuur test e-mail
bool sendTestEmail() {
  // Deze functie omzeilt de interval check voor test doeleinden
  String subject = "Test Email - ESP32 Hydro Systeem";
  String message = "Dit is een test e-mail van je ESP32 Hydroponisch Systeem Controller.\n";
  message += "Als je deze e-mail ontvangt, is de e-mailconfiguratie correct.";
  
  // Reset de laatste foutmelding
  lastEmailError = "";
  
  // Test internetverbinding
  if (WiFi.status() != WL_CONNECTED) {
    lastEmailError = "Geen WiFi-verbinding";
    Serial.println("FOUT: Geen WiFi-verbinding, kan geen e-mail versturen");
    return false;
  }
  
  if (!emailClientReady) {
    lastEmailError = "E-mail client niet geïnitialiseerd";
    Serial.println("FOUT: E-mail client niet geïnitialiseerd");
    return false;
  }
  
  // Bereid het bericht voor
  SMTP_Message emailMessage;
  
  // Stel de bericht headers in
  emailMessage.sender.name = "ESP32 Hydro Controller";
  emailMessage.sender.email = settings.emailUsername;
  emailMessage.subject = subject.c_str();
  emailMessage.addRecipient("Hydroponisch Systeem", settings.emailRecipient);
  
  // Haal datum en tijd op
  String currentDateTime = getFullDateTimeString();
  
  // Maak de inhoud van het bericht
  String textContent = message;
  textContent += "\n\n---------------------------\n";
  textContent += "Systeem: ";
  textContent += settings.systeemnaam;
  textContent += "\nTijdstip: ";
  textContent += currentDateTime;
  textContent += "\nHuidige temperatuur: ";
  textContent += String(currentTemp);
  textContent += "°C\n";
  
  #ifdef ENABLE_FLOW_SENSOR
  textContent += "Huidige waterstroming: ";
  textContent += String(flowRate);
  textContent += " L/min\n";
  textContent += "Totaal doorgestroomd: ";
  textContent += String(totalLiters);
  textContent += " L\n";
  #endif
  
  // Stel de tekst in
  emailMessage.text.content = textContent.c_str();
  emailMessage.text.charSet = "utf-8";
  emailMessage.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  emailMessage.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  
  // Verbind met de server en verzend de e-mail
  Serial.println("Test e-mail verzenden...");
  
  // Verbind met de server
  if (!smtp.connect(&config)) {
    lastEmailError = "Kan niet verbinden met e-mailserver";
    Serial.println("FOUT: Kan niet verbinden met e-mailserver");
    return false;
  }
  
  // Verzend het bericht
  if (!MailClient.sendMail(&smtp, &emailMessage)) {
    lastEmailError = String(smtp.errorReason());
    Serial.println("FOUT: Verzenden test e-mail mislukt");
    Serial.println(smtp.errorReason());
    return false;
  }
  
  Serial.println("Test e-mail succesvol verzonden");
  // We updaten lastEmailSent NIET voor testmails om het normale alarm systeem niet te beïnvloeden
  return true;
}

// Genereer JSON met e-mail status
String getEmailStatusJson() {
  DynamicJsonDocument doc(512);
  
  doc["emailReady"] = emailClientReady;
  doc["lastEmailSent"] = lastEmailSent;
  doc["lastEmailError"] = lastEmailError;
  
  // Bereken tijd sinds laatste e-mail
  unsigned long timeSince = millis() - lastEmailSent;
  unsigned long minutesSince = timeSince / 60000;
  
  doc["minutesSinceLastEmail"] = minutesSince;
  doc["canSendEmail"] = (timeSince > MIN_EMAIL_INTERVAL);
  
  String response;
  serializeJson(doc, response);
  return response;
}

#endif // defined(ENABLE_FLOW_SENSOR) && defined(ENABLE_EMAIL_NOTIFICATION)