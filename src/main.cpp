// #define WIFI_STATION_MODE

#include "main.hpp"
#include "sscweb.hpp"

// What HW is actually connected? We will ignore the HW if declared disconnected.
bool rtc_available=true;
bool led_available=true;

#define I2C_SDA 33
#define I2C_SCL 32

// Store preferences like colors, etc
Preferences preferences;

// Initialize the RTC
#ifdef RTC_IS_DS1307
  RTC_DS1307 rtc;
#else
  RTC_PCF8523 rtc;
#endif

enum ClockMode clock_mode=LIVE;

// Create a webserver to host the control panel
AsyncWebServer server(80);

// Global variables to be accessed in multiple places
double days_since_j2k_epoch=0;
DateTime last_frame_time;
double playback_speedup=100000;
double planet_longitudes[NUM_PLANETS] = {0.,0.,0.,0.,0.,0.,0.,0.};

bool verbose=false;

void setup() {
  // Serial monitor
  Serial.begin(115200);
  Serial.println("Started serial");

  // Preferences
  loadPreferences();

  // WiFi
  Serial.print("Connecting to WiFi...");
  IPAddress myIP;
  #ifdef WIFI_STATION_MODE
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_sta_ssid, wifi_sta_password, WIFI_CHANNEL);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
    }
    myIP = WiFi.localIP();
  #else
    // #ifdef WIFI_AP_MODE
    Serial.print("(AP mode)... ");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(wifi_ap_ssid.c_str(), wifi_ap_password.c_str(), WIFI_CHANNEL);
    myIP = WiFi.softAPIP();
    // #else // Both
    //   Serial.print("(AP+STA mode)... ");
    //   WiFi.mode(WIFI_AP_STA);
    //   WiFi.softAP(ssid, password, WIFI_CHANNEL);
    //   myIP = WiFi.softAPIP();
    // #endif
  #endif


  Serial.println(" Connected!");
  Serial.print("IP Address: ");
  Serial.println(myIP);

  // Clock
  if (rtc_available){
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("Started Wire");
    uint8_t rtc_tries = 10;
    while (!rtc.begin() && --rtc_tries>0){ 
      Serial.print("noRTC...");
      delay(10);
    }
      if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");

      clock_mode = PLAYBACK;
      rtc_available = false;
      // while (1);
    }
  }
  

  if (rtc_available &&
  #ifdef RTC_IS_DS1307
      !rtc.isrunning()
  #else
      (! rtc.initialized() || rtc.lostPower())
  #endif
  ) {
    Serial.println("RTC is NOT running!");
    // This line sets the RTC with an explicit date & time, for example to set
    // January 1, 2024 at 00:00 you would call:
    // rtc.adjust(DateTime(2024, 1, 1, 0, 0, 0));
    // And this sets the datetime to the binary compilation datetime.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    #ifndef RTC_IS_DS1307
      rtc.start();
    #endif
  }

  last_frame_time = rtc.now();


  // LEDs
  if (led_available){
    // Set up LED matrix
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  }
  // Filesystem
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
  // Webserver
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.serveStatic("/assets/", SPIFFS, "/");

  server.on("/speed", HTTP_GET, getSpeed);

  server.on("/setSpeed", HTTP_POST, setSpeed);

  server.on("/date", HTTP_GET, getDate);

  server.on("/setDate", HTTP_POST, setDate);

  AsyncCallbackJsonWebHandler* setPlanetColorsHandler = new AsyncCallbackJsonWebHandler("/setPlanetcolors", setPlanetColors);
  server.addHandler(setPlanetColorsHandler);
    // server.on("/setPlanetcolors", HTTP_POST, setPlanetColors);

  server.on("/mode", HTTP_GET, getMode);

  server.on("/setMode", HTTP_POST, setMode);

  server.on("/longitudes", HTTP_GET, getLongitudes);

  server.on("/planetcolors", HTTP_GET, getPlanetColors);

  server.onNotFound(notFound);

  server.begin();
}

void loop() {
  // Get time
  // If RTC is not available, default to J2K
  DateTime now(946728000);
  if (rtc_available){
    now = rtc.now();
    char clock_buffer[] = "DDD, DD MMM YYYY hh:mm:ss";
    if(verbose){
      Serial.println(now.toString(clock_buffer));
    }
  } 

  if (clock_mode==LIVE){  
    if (verbose) Serial.print("Seconds since UNIX epoch: ");
    uint32_t seconds_since_unix_epoch = now.unixtime();
    if (verbose) Serial.print(seconds_since_unix_epoch);
    if (verbose) Serial.print(" and s since J2K epoch: ");
    uint32_t seconds_since_j2k_epoch = seconds_since_unix_epoch-UNIX_TO_J2K_OFFSET_S;
    if (verbose) Serial.print(seconds_since_j2k_epoch);
    if (verbose) Serial.print(" corresponding to ");
    days_since_j2k_epoch = seconds_since_j2k_epoch/(60*60*24.);
    if (verbose) Serial.print(days_since_j2k_epoch);
    if (verbose) Serial.print(" days\n");
  } else {
    double playback_clock_increment_days = playback_speedup/(60*60*24);
    if (rtc_available){
      double candidate_playback_clock_increment_days = (now-last_frame_time).totalseconds()*playback_speedup/(60*60*24);
      if (!std::isnan(candidate_playback_clock_increment_days)){
        playback_clock_increment_days = candidate_playback_clock_increment_days;
      }
      last_frame_time = now;
    }
    // Increment the time by time since last frame, multiplied by playback speedup, in days.
    // Serial.printf("Updating days since J2K epoch by adding %lf to get %lf\n",playback_clock_increment_days, days_since_j2k_epoch);
    days_since_j2k_epoch += playback_clock_increment_days;
    // days_since_j2k_epoch += 0.0001*playback_speedup/(60*60*24);
  }
  if (date_is_valid(days_since_j2k_epoch)){
    preferences.begin("SSC", false);
    preferences.putDouble("j2kdate", days_since_j2k_epoch);
    preferences.end();
  }

  // Initialize LEDs
  CRGB *led = leds;
  uint8_t ring_offset = 0;
  if (verbose){
    Serial.print("Days since J2k:");
    Serial.println(days_since_j2k_epoch);
  }
  // Cycle across planets from outside in
  for(uint8_t ring_id=0; ring_id<NUM_PLANETS; ring_id++) {
    uint8_t planet_id = (NUM_PLANETS-1)-ring_id; // Skip the Sun, and -1 to count from 0.
    uint8_t count = led_count[ring_id];
    if (verbose){
      Serial.print("Planet: ");
      Serial.print(planet_id);
      Serial.print(": ");
    }
    double longitude_planet = longitude_at_date(planets[planet_id], days_since_j2k_epoch);
    // Wrapping is imperfect for negative numbers.
    if (longitude_planet<-M_PI){
      longitude_planet+=M_PI;
    }
    // Update the global var for the webserver
    planet_longitudes[planet_id] = longitude_planet;
    // Find the LED number WITHIN THE RING corresponding to the planet location
    uint8_t ring_planet_id = round(count*(longitude_planet+M_PI)/(2*M_PI));
    if (verbose){
      Serial.print(longitude_planet);
      Serial.print(" rad or ");
      Serial.print(longitude_planet*180./M_PI);
      Serial.print(" deg, corresponding to LED ");
      Serial.print(ring_planet_id);
    }
    
    // Wrap if we find the final LED
    if (ring_planet_id==count){
      ring_planet_id-=count;
    }
    // Black out the ring
    for (uint8_t i = 0; i < count ; i++) {
      *led++ = CRGB::Black;
    }
    // Color the relevant planet
    // uint8_t ring_index = seconds_since_unix_epoch%count;
    if (verbose){
      Serial.print(". We will light LED ");
      Serial.println(ring_offset+ring_planet_id);
    }
    leds[ring_offset+ring_planet_id] = planet_colors[planet_id];
    ring_offset += count;
    
  }
  // Color the Sun
  leds[NUM_LEDS-1] = sun_color; //getPlanetColor(0);

  if (led_available){
    FastLED.show();
  }
  sleep(1);
}

// CRGB getPlanetColor(int planetIndex) {
//   return planet_colors[planetIndex];
// }

void notFound(AsyncWebServerRequest *request)
{
  if (request->method() == HTTP_OPTIONS)
  {
    request->send(200);
  }
  else
  {
    request->send(404, "application/json", "{\"message\":\"Not found\"}");
  }
}

bool date_is_valid(double days_since_j2k){
  double seconds_since_unix_epoch = days_since_j2k*(60.*60.*24.)+ UNIX_TO_J2K_OFFSET_S;
  return (
    !std::isnan(days_since_j2k) &&
    seconds_since_unix_epoch>=-156838003622 &&
    seconds_since_unix_epoch<=32503708800
  );
}

void loadPreferences(){
    // Preferences
  // WiFi
  preferences.begin("SSC", false);
  if (preferences.isKey("wifi_ap_ssid")){
    Serial.print("Reading key wifi_ap_ssid...");
    wifi_ap_ssid = preferences.getString("wifi_ap_ssid");
  } else {
    Serial.print("SETTING key wifi_ap_ssid...");
    preferences.putString("wifi_ap_ssid", wifi_ap_ssid);
  }
  if (preferences.isKey("wifi_ap_pw")){
    Serial.print("Reading key wifi_ap_pw...");
    wifi_ap_password = preferences.getString("wifi_ap_pw");
  } else {
    Serial.print("SETTING key wifi_ap_pw...");
    preferences.putString("wifi_ap_pw", wifi_ap_password);
  }
  if (preferences.isKey("wifi_sta_ssid")){
    Serial.print("Reading key wifi_sta_ssid...");
    wifi_sta_ssid = preferences.getString("wifi_sta_ssid");
  } else {
    Serial.print("SETTING key wifi_sta_ssid...");
    preferences.putString("wifi_sta_ssid", wifi_sta_ssid);
  }
  if (preferences.isKey("wifi_sta_pw")){
    Serial.print("Reading key wifi_sta_pw...");
    wifi_sta_password = preferences.getString("wifi_sta_pw");
  } else {
    Serial.print("SETTING key wifi_sta_pw...");
    preferences.putString("wifi_ap_pw", wifi_sta_password);
  }

  // Mode and time
  if (preferences.isKey("clock_is_live")){
    Serial.print("Reading key clock_is_live...");
    bool clock_is_live = preferences.getBool("clock_is_live");
    clock_mode = (clock_is_live? LIVE : PLAYBACK);
  } else {
    Serial.print("SETTING key clock_is_live...");
    preferences.putBool("clock_is_live", (clock_mode==LIVE));
  }

  if (preferences.isKey("j2kdate")){
    Serial.print("Reading key j2kdate...");
    double days_since_j2k_epoch_from_prefs = preferences.getDouble("j2kdate");
    double seconds_since_unix_epoch_from_prefs = days_since_j2k_epoch_from_prefs*(60.*60.*24.)+ UNIX_TO_J2K_OFFSET_S;
    if (date_is_valid(days_since_j2k_epoch_from_prefs)){
      // The date is valid
      days_since_j2k_epoch = days_since_j2k_epoch_from_prefs;
    } // If this does not work, the date will be reset to J2K epoch, and it will be written in at the next cycle
  } else {
    Serial.print("SETTING key j2kdate...");
    preferences.putDouble("j2kdate", days_since_j2k_epoch);
  }
  if (preferences.isKey("play_speedup")){
    Serial.print("Reading key play_speedup...");
    double candidate_playback_speedup = preferences.getDouble("play_speedup");
    if (!std::isnan(candidate_playback_speedup)){
      playback_speedup=candidate_playback_speedup;
    } else {
      Serial.println("Playback speedup is nan, resetting!\n");
      preferences.putDouble("play_speedup", playback_speedup);
    }
  } else {
    Serial.print("SETTING key play_speedup...");
    preferences.putDouble("play_speedup", playback_speedup);
  }

  // Colors
  if (preferences.isKey("color_Sun")){
    Serial.print("Reading key color_Sun...");
    sun_color = preferences.getInt("color_Sun");
  } else {
    Serial.print("SETTING key color_Sun...");
    preferences.putInt("color_Sun", sun_color);
  }
  if (preferences.isKey("color_Mercury")){
    Serial.print("Reading key color_Mercury...");
    planet_colors[0] = preferences.getInt("color_Mercury");
  } else {
    Serial.print("SETTING key color_Mercury...");
    preferences.putInt("color_Mercury", planet_colors[0]);
  }
  //
  if (preferences.isKey("color_Venus")){
    Serial.print("Reading key color_Venus...\n");
    planet_colors[1] = preferences.getInt("color_Venus");
  } else {
    Serial.print("SETTING key color_Venus...\n");
    preferences.putInt("color_Venus", planet_colors[1]);
  }
  //
  if (preferences.isKey("color_Earth")){
    planet_colors[2] = preferences.getInt("color_Earth");
  } else {
    preferences.putInt("color_Earth", planet_colors[2]);
  }
  //
  if (preferences.isKey("color_Mars")){
    planet_colors[3] = preferences.getInt("color_Mars");
  } else {
    preferences.putInt("color_Mars", planet_colors[3]);
  }
  //
  if (preferences.isKey("color_Jupiter")){
    planet_colors[4] = preferences.getInt("color_Jupiter");
  } else {
    preferences.putInt("color_Jupiter", planet_colors[4]);
  }
  //
  if (preferences.isKey("color_Saturn")){
    planet_colors[5] = preferences.getInt("color_Saturn");
  } else {
    preferences.putInt("color_Saturn", planet_colors[5]);
  }
  //
  if (preferences.isKey("color_Uranus")){
    planet_colors[6] = preferences.getInt("color_Uranus");
  } else {
    preferences.putInt("color_Uranus", planet_colors[6]);
  }
  //
  if (preferences.isKey("color_Neptune")){
    planet_colors[7] = preferences.getInt("color_Neptune");
  } else {
    preferences.putInt("color_Neptune", planet_colors[7]);
  }

  preferences.end();
}