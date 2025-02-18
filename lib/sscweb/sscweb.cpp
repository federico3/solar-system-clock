

#include "sscweb.hpp"

void getSpeed(AsyncWebServerRequest *request)
{
    Serial.print("Received request at endpoint /speed");
    char speed_str[50];

    if (clock_mode==LIVE){
      sprintf(speed_str, "%lf", 1.);
    } else {
      sprintf(speed_str, "%lf", playback_speedup);
    }
    request->send(200, "text/plain", speed_str); 
    Serial.println("...serviced!");
}

void setSpeed(AsyncWebServerRequest *request)
{
    Serial.print("Received request at endpoint /setSpeed");
    int reqcode = 200;

    // get who param
    String reqspeed="";
    if (request->hasParam("speed", true)) {
      reqspeed = request->getParam("speed", true)->value();
      double reqspeed_double = atof(reqspeed.c_str());
      Serial.printf("Setting speed to %lf\n", reqspeed_double);
      playback_speedup = reqspeed_double;
      if (playback_speedup!=1.0){
        clock_mode = PLAYBACK;
      }
    } else {
      reqspeed = "No message sent";
      reqcode = 400;
    }
    request->send(reqcode, "text/plain", reqspeed);

    Serial.println("...serviced!");
}

void getDate(AsyncWebServerRequest *request)
{
  Serial.print("Received request at endpoint /date");
  char date_str[50];
  sprintf(date_str, "%lf", (days_since_j2k_epoch*(24*60*60)+UNIX_TO_J2K_OFFSET_S)*1e3);
  request->send(200, "text/plain", date_str); 
  Serial.println("...serviced!");
}

void setDate(AsyncWebServerRequest *request)
{
    Serial.print("Received request at endpoint /setDate");

    // get param
    String reqdate="";
    int reqcode = 200;
    if (request->hasParam("date", true)) {
      reqdate = request->getParam("date", true)->value();
      double reqdate_double = atof(reqdate.c_str());
      if (reqdate_double<=-156838003622*1e3){
        Serial.print("Date is before 3000 BCE, ignoring!");
        reqcode = 400;
      } else if (reqdate_double>=32503708800*1e3) {
        Serial.print("Date is after 3000 CE, ignoring!");
        reqcode = 400;
      } else {
        double j2kdate = (reqdate_double/1e3-UNIX_TO_J2K_OFFSET_S)/(60.*60.*24.);
        Serial.printf("Setting date to %lf unixtime, or %lf days since J2K\n", reqdate_double, j2kdate);
        days_since_j2k_epoch = j2kdate;
      }
    } else {
      reqdate = "No message sent";
    }
    request->send(reqcode, "text/plain", reqdate);

    Serial.println("...serviced!");
}

void getLongitudes(AsyncWebServerRequest *request)
{
  Serial.print("Received request at endpoint /longitudes");
  String longitudes_out;
  JsonDocument doc;
  doc["longitudes"] = JsonArray();
  for (uint8_t planet_id=0; planet_id<NUM_PLANETS; planet_id++){
    doc["longitudes"].add(planet_longitudes[planet_id]);
  }
  serializeJson(doc, longitudes_out);
  request->send(200, "text/json", longitudes_out); 
  Serial.println("...serviced!");
}

void getMode(AsyncWebServerRequest *request)
{
    Serial.print("Received request at endpoint /mode");
    char mode_str[8];
    if (clock_mode==LIVE){
      request->send(200, "text/plain", "LIVE"); 
    }else{
      request->send(200, "text/plain", "PLAYBACK"); 
    }
    Serial.println("...serviced!");
}

void setMode(AsyncWebServerRequest *request)
{
  Serial.print("Received request at endpoint /setMode");

  // get param
  String reqmode="";
  int reqcode = 200;
  if (request->hasParam("mode", true)) {
    reqmode = request->getParam("mode", true)->value();
    if (reqmode=="LIVE"){
      if (rtc_available){
        clock_mode = LIVE;
        playback_speedup = 1; // We won't use this in LIVE mode, but it helps viz.
        Serial.println("Switching mode to LIVE");
      } else {
        reqmode = "RTC not available!";
        Serial.println("RTC not available, ignoring request to switch to LIVE");
        reqcode = 503;
      }
    } else if (reqmode=="PLAYBACK") {
      clock_mode = PLAYBACK;
      Serial.println("Switching mode to PLAYBACK");
    } else {
      Serial.printf("Mode %s unknown, ignoring\n",reqmode);
    }

  } else {
    reqmode = "No message sent";
    reqcode = 400;
  }
  request->send(reqcode, "text/plain", reqmode);

  Serial.println("...serviced!");
}