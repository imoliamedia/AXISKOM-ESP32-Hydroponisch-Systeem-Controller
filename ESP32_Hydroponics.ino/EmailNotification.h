/*
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