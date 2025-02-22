#ifndef SSC_MAIN_HPP
#define SSC_MAIN_HPP
// Base Arduino library
#include <Arduino.h>
#include <Preferences.h>

// Configurations for this project that the libraries should know about
#include "ssctypes.hpp"

// LEDs
#include <FastLED.h>
// SPI for RTC
#include <Wire.h>
// RTC
#include <RTClib.h>
// Compute the orbits
#include "planets.h"
// Solar System data
#include "orbits.h"

// Webserver
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// JSON
#include "ArduinoJson.h"

// File system to serve static data
#include "SPIFFS.h"

// Custom types
#include "ssctypes.hpp"

// Function declarations
// CRGB getPlanetColor(int planetIndex);
void notFound(AsyncWebServerRequest *request);

void loadPreferences();

// Note: Wokwi uses a special WiFi network called Wokwi-GUEST (no password).
// This virtual network provides internet access to the simulated ESP32.
// To learn more: https://docs.wokwi.com/guides/esp32-wifi#connecting-to-the-wifi

String wifi_ap_ssid = "SolarSystemClock";
String wifi_ap_password = "";
String wifi_sta_ssid = "Wokwi-GUEST";
String wifi_sta_password = "";
const int WIFI_CHANNEL = 6; // Speeds up the connection in Wokwi
// #define WIFI_STATION_MODE

// Initialize the LED struct holder
CRGB leds[NUM_LEDS];
const uint8_t led_count[] = {60, 48, 40, 32, 24, 16, 12, 8, 1};
#define NUM_RINGS (sizeof(led_count) / sizeof(led_count[0]))


// Load the Keplerian elements for all planets
keplerian_elements planets[NUM_PLANETS]={
  Mercury,
  Venus,
  Earth_Moon_barycenter,
  Mars,
  Jupiter,
  Saturn,
  Uranus,
  Neptune
};

// Default planet colors, inspired by https://i.imgur.com/3pL0vJq.png
int planet_colors[8] = {
  0xBFBEBD, //CRGB::Orange;   // Mercury
  0xD9B391, //CRGB::Green;    // Venus
  0xBACBD9, //CRGB::Blue;     // Earth
  0xF27A5E, //CRGB::Red;      // Mars
  0xBF8136, //CRGB::Purple;   // Jupiter
  0xF2CD88, //CRGB::Cyan;     // Saturn
  0xCEF2F2, //CRGB::Magenta;  // Uranus
  0x789EBF, //CRGB::White;    // Neptune
};
int sun_color = 0xf28322;

#endif