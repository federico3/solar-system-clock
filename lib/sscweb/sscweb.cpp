

#include "sscweb.hpp"

Preferences webpreferences;

void getSpeed(AsyncWebServerRequest *request)
{
    char speed_str[50];
    if (clock_mode==LIVE){
      sprintf(speed_str, "%lf", 1.);
    } else {
      sprintf(speed_str, "%lf", playback_speedup);
    }
    request->send(200, "text/plain", speed_str); 

}

void setSpeed(AsyncWebServerRequest *request)
{
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

      webpreferences.begin("SSC", false);
      webpreferences.putDouble("play_speedup", playback_speedup);
      webpreferences.putBool("clock_is_live", (clock_mode==LIVE));
      webpreferences.end();

    } else {
      reqspeed = "No message sent";
      reqcode = 400;
    }
    request->send(reqcode, "text/plain", reqspeed);
}

void getDate(AsyncWebServerRequest *request)
{
  char date_str[50];
  sprintf(date_str, "%lf", (days_since_j2k_epoch*(24*60*60)+UNIX_TO_J2K_OFFSET_S)*1e3);
  request->send(200, "text/plain", date_str); 
}

void setDate(AsyncWebServerRequest *request)
{

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
        clock_mode = PLAYBACK;

        webpreferences.begin("SSC", false);
        webpreferences.putDouble("j2kdate", days_since_j2k_epoch);
        webpreferences.putBool("clock_is_live", (clock_mode==LIVE));
        webpreferences.end();
      }
    } else {
      reqdate = "No message sent";
    }
    request->send(reqcode, "text/plain", reqdate);

}

void getLongitudes(AsyncWebServerRequest *request)
{
  String longitudes_out;
  JsonDocument doc;
  doc["longitudes"] = JsonArray();
  for (uint8_t planet_id=0; planet_id<NUM_PLANETS; planet_id++){
    doc["longitudes"].add(planet_longitudes[planet_id]);
  }
  serializeJson(doc, longitudes_out);
  request->send(200, "text/json", longitudes_out); 
}

void getMode(AsyncWebServerRequest *request)
{
    char mode_str[8];
    if (clock_mode==LIVE){
      request->send(200, "text/plain", "LIVE"); 
    }else{
      request->send(200, "text/plain", "PLAYBACK"); 
    }
}

void setMode(AsyncWebServerRequest *request)
{
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
    webpreferences.begin("SSC", false);
    webpreferences.putBool("clock_is_live", (clock_mode==LIVE));
    webpreferences.putFloat("play_speedup", playback_speedup);
    webpreferences.end();

  } else {
    reqmode = "No message sent";
    reqcode = 400;
  }
  request->send(reqcode, "text/plain", reqmode);

}


void getPlanetColors(AsyncWebServerRequest *request)
{
  JsonDocument doc;
  doc["planet_colors"] = JsonArray();
  for (uint8_t planet_id=0; planet_id<NUM_PLANETS; planet_id++){
    doc["planet_colors"].add(planet_colors[planet_id]);
  }
  doc["sun_color"] = sun_color;
  String planet_colors_out;
  serializeJson(doc, planet_colors_out);
  request->send(200, "text/json", planet_colors_out); 
}

void setPlanetColors(AsyncWebServerRequest *request, JsonVariant &json)
{
  Serial.print("Received request at endpoint /setPlanetcolors");
  int reply_code = 200;
  String reply_msg = "Planet colors set!";
  JsonObject jsonObj = json.as<JsonObject>();
  if (jsonObj["sun_color"].is<int>()){
    sun_color = jsonObj["sun_color"];
  } else {
    reply_code = 400;
    reply_msg = "Missing field sun_color in JSON!";
  }

  for (uint8_t planet_id = 0; planet_id<NUM_PLANETS; planet_id++){
    if (jsonObj["planet_colors"][planet_id].is<int>()){
      planet_colors[planet_id] = jsonObj["planet_colors"][planet_id];
    } else {
      reply_code = 400;
      reply_msg = "Missing planet color!";
      break;
    }
  }
  request->send(reply_code, "text/plain", reply_msg); 

  if (reply_code == 200){
    webpreferences.putInt("color_Sun", sun_color);
    webpreferences.putInt("color_Mercury", planet_colors[0]);
    webpreferences.putInt("color_Venus", planet_colors[1]);
    webpreferences.putInt("color_Earth", planet_colors[2]);
    webpreferences.putInt("color_Mars", planet_colors[3]);
    webpreferences.putInt("color_Jupiter", planet_colors[4]);
    webpreferences.putInt("color_Saturn", planet_colors[5]);
    webpreferences.putInt("color_Uranus", planet_colors[6]);
    webpreferences.putInt("color_Neptune", planet_colors[7]);
  }

  Serial.println("...serviced!");
}