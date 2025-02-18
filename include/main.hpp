#ifndef SSC_MAIN_HPP
#define SSC_MAIN_HPP
// Base Arduino library
#include <Arduino.h>

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
CRGB getPlanetColor(int planetIndex);
void notFound(AsyncWebServerRequest *request);

// Note: Wokwi uses a special WiFi network called Wokwi-GUEST (no password).
// This virtual network provides internet access to the simulated ESP32.
// To learn more: https://docs.wokwi.com/guides/esp32-wifi#connecting-to-the-wifi
#ifdef WIFI_STATION_MODE
const char* ssid = "Wokwi-GUEST";
#else
const char* ssid = "SolarSystemClock";
#endif
const char* password = "";
const int WIFI_CHANNEL = 6; // Speeds up the connection in Wokwi

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

#endif