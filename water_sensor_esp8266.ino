/* 
 * (c) © 2018 Xavier Lüthi xavier@luthi.eu
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See LICENSE.txt for the full license text.
 */
#include <ESP8266WiFi.h>
#include "private.h"

#define DOMOTICZ  // unset to not send values to domoticz
#define DEBUG
#include "xl_debug.h"

// PIN definition
#define reedLedPin D2
#define reedPin    D1
#define delayPulse 4 // wait between two pulses (multiple of delay_timer0). Avoid false reading with slow rotation

// Internal timer values, used for interrupts.
unsigned long delay_timer0 = 80000000L; // 80 MHz == 1 sec

unsigned long pulses = 0;
unsigned long previousPulses = 0;
volatile byte ledState = LOW;
volatile unsigned long timerDebounce = 0;
volatile unsigned long lastDebounce = 0;


void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  delay(10);
#endif
  // Setup pins
  pinMode(reedLedPin,OUTPUT);
  pinMode(reedPin,   INPUT);

  connectWifi(wifi_ssid, wifi_password, wifi_hostname);

  // configure "master" timer to 1 second.
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + delay_timer0);
  interrupts();             // enable all interrupts

  // Interrupt for reed switch when the Pin goes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(reedPin), debounceInterrupt, RISING);
}

void loop() {
  // Turn on LED when actuel pulse is detected.
  // The ledState is controlled in interrupt functions.
  if (ledState == HIGH) {
    int value = pulses;
    pulses = 0;
#ifdef DOMOTICZ
    sendPulseToDomoticz(value);
#endif
    digitalWrite(reedLedPin,HIGH);
    delay(2000);
    digitalWrite(reedLedPin,LOW);
    ledState = LOW;
  }
}


// Reed switch interrupt with debounce
void debounceInterrupt() {

  DEBUG_PRINTLN("Int !!!");
  DEBUG_PRINTLN(timerDebounce);
  DEBUG_PRINTLN(lastDebounce);

  if ((timerDebounce - lastDebounce) >= delayPulse) 
  {                                                  
    lastDebounce = 0;
    timerDebounce = 0;
    // Actual Reed pulse detected.
    pulses++;

  DEBUG_PRINTLN("New pulse ! pulse number: ");
  DEBUG_PRINTLN(pulses);

  ledState = HIGH;      // actual LED control is in the loop()
  }
}

// timer0 interrupt
void timer0_ISR (void) {
  timerDebounce++;
  timer0_write(ESP.getCycleCount() + delay_timer0);

  DEBUG_PRINTLN("timer0_ISR");
}

