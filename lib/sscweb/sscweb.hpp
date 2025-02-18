#ifndef SSC_WEB_HPP
#define SSC_WEB_HPP

// Base Arduino library
#include <Arduino.h>

// Webserver
#include <ESPAsyncWebServer.h>

// JSON
#include "ArduinoJson.h"

// #include "main.hpp"

#include "ssctypes.hpp"

extern double playback_speedup;
extern double days_since_j2k_epoch;
extern double planet_longitudes[];
extern ClockMode clock_mode;
extern bool rtc_available;

void getSpeed(AsyncWebServerRequest *request);
void getDate(AsyncWebServerRequest *request);
void getLongitudes(AsyncWebServerRequest *request);
void getMode(AsyncWebServerRequest *request);

void setSpeed(AsyncWebServerRequest *request);
void setDate(AsyncWebServerRequest *request);
void setMode(AsyncWebServerRequest *request);

#endif