#define WIFI_STATION_MODE

#include "main.hpp"
#include "sscweb.hpp"

// What HW is actually connected? We will ignore the HW if declared disconnected.
bool rtc_available=true;
bool led_available=true;

// Initialize the RTC
RTC_DS1307 rtc;

enum ClockMode clock_mode=LIVE;

// Create a webserver to host the control panel
AsyncWebServer server(80);

// Global variables to be accessed in multiple places
double days_since_j2k_epoch=0;
DateTime last_frame_time;
double playback_speedup=100000;
double planet_longitudes[NUM_PLANETS] = {0.,0.,0.,0.,0.,0.,0.,0.};

bool verbose=true;

void setup() {
  // Serial monitor
  Serial.begin(115200);
  Serial.println("Started serial");

  // WiFi
  Serial.print("Connecting to WiFi...");
  IPAddress myIP;
  #ifdef WIFI_STATION_MODE
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password, WIFI_CHANNEL);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
    }
    myIP = WiFi.localIP();
  #else
    Serial.print("(AP mode)... ");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password, WIFI_CHANNEL);
    myIP = WiFi.softAPIP();
  #endif
  Serial.println(" Connected!");
  Serial.print("IP Address: ");
  Serial.println(myIP);

  // Clock
  if (rtc_available){
    Wire.begin();
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
  
  if (rtc_available &&  !rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  // This line sets the RTC with an explicit date & time, for example to set
  // January 1, 2024 at 00:00 you would call:
  // rtc.adjust(DateTime(2024, 1, 1, 0, 0, 0));
  // And this sets the datetime to the binary compilation datetime.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
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

  server.on("/mode", HTTP_GET, getMode);

  server.on("/setMode", HTTP_POST, setMode);

  server.on("/longitudes", HTTP_GET, getLongitudes);

  server.onNotFound(notFound);

  server.begin();
}

void loop() {
  // Get time
  // If RTC is not available, default to J2K
  DateTime now(946728000);
  if (rtc_available){
    DateTime now = rtc.now();
    char clock_buffer[] = "DDD, DD MMM YYYY hh:mm:ss";
    if(verbose){
      Serial.println(now.toString(clock_buffer));
    }
  } 

  if (clock_mode==LIVE){  
    uint32_t seconds_since_unix_epoch = now.unixtime();
    uint32_t seconds_since_j2k_epoch = seconds_since_unix_epoch-UNIX_TO_J2K_OFFSET_S;
    days_since_j2k_epoch = seconds_since_j2k_epoch/(60.*60.*24.);
  } else {
    double playback_clock_increment_days = playback_speedup/(60*60*24);
    if (rtc_available){
      playback_clock_increment_days = (now-last_frame_time).totalseconds()*playback_speedup/(60*60*24);
      last_frame_time = now;
    }
    // Increment the time by time since last frame, multiplied by playback speedup, in days.
    days_since_j2k_epoch += playback_clock_increment_days;
    // days_since_j2k_epoch += 0.0001*playback_speedup/(60*60*24);
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
    leds[ring_offset+ring_planet_id] = getPlanetColor(planet_id);
    ring_offset += count;
    
  }
  // Color the Sun
  leds[NUM_LEDS-1] = 0xf28322; //getPlanetColor(0);

  if (led_available){
    FastLED.show();
  }
  sleep(1);
}

// Planet colors, inspired by https://i.imgur.com/3pL0vJq.png
CRGB getPlanetColor(int planetIndex) {
  switch (planetIndex) {
    // case 0: return 0xf28322; //CRGB::Yellow;
    case 0: return 0xBFBEBD; //CRGB::Orange;   // Mercury
    case 1: return 0xD9B391; //CRGB::Green;    // Venus
    case 2: return 0xBACBD9; //CRGB::Blue;     // Earth
    case 3: return 0xF27A5E; //CRGB::Red;      // Mars
    case 4: return 0xBF8136; //CRGB::Purple;   // Jupiter
    case 5: return 0xF2CD88; //CRGB::Cyan;     // Saturn
    case 6: return 0xCEF2F2; //CRGB::Magenta;  // Uranus
    case 7: return 0x789EBF; //CRGB::White;    // Neptune
    default: return CRGB::Black;   // Default to off
  }
}

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