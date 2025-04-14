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
 * EmailNotification.h
 *
 * Header voor de e-mail notificatie module
 */

#ifndef EMAIL_NOTIFICATION_H
#define EMAIL_NOTIFICATION_H

#include "Settings.h"

#if defined(ENABLE_FLOW_SENSOR) && ENABLE_FLOW_SENSOR == true && defined(ENABLE_EMAIL_NOTIFICATION) && ENABLE_EMAIL_NOTIFICATION == true
  #include <ESP_Mail_Client.h>
  
  // Externe variabelen
  extern bool emailClientReady;
  extern unsigned long lastEmailSent;
  extern String lastEmailError;
  
  // Functieprototypes
  void setupEmailNotification();
  bool sendFlowAlertEmail();
  bool sendTestEmail();
  bool sendEmailAlert(const char* subject, const char* message);
  String getLastEmailError();
  String getEmailStatusJson();
  
  // Hernoem de setupEmailClient functie naar setupEmailNotification voor compatibiliteit
  #define setupEmailClient setupEmailNotification
#endif

#endif // EMAIL_NOTIFICATION_H