//=========================================================================
// ESP32 Hydroponisch Systeem Controller
// Email notificatie module - Header
//=========================================================================

#ifndef EMAIL_NOTIFICATION_H
#define EMAIL_NOTIFICATION_H

#include "Settings.h"
#include <Arduino.h>

// E-mail gerelateerde definities
#define GMAIL_SMTP_SERVER "smtp.gmail.com"
#define GMAIL_SMTP_PORT 465
#define EMAIL_SEND_TIMEOUT 20000      // Timeout voor e-mailcommunicatie in ms
#define MIN_EMAIL_INTERVAL 300000     // Minimale tijd tussen e-mails (5 minuten) om spam te voorkomen

// Functiedeclaraties
void setupEmailClient();
bool sendEmailAlert(const char* subject, const char* message);
void triggerFlowAlert();
bool sendTestEmail();
void handleTestEmail();

// Status variabelen
extern bool emailClientReady;
extern unsigned long lastEmailSent;

#endif // EMAIL_NOTIFICATION_H