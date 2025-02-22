#ifndef SSC_TYPES_HPP
#define SSC_TYPES_HPP

// LED configuration
#define LED_PIN     5
#define NUM_LEDS    241
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB


// Time configuration: offset between UNIX epoch and J2K epoch
#define UNIX_TO_J2K_OFFSET_S 946728000

#define NUM_PLANETS 8

// Clock mode
// In live mode, clock only looks at RTC.
// In playback mode, it advances at a given rate.
enum ClockMode{
    LIVE=0,
    PLAYBACK
  };
  


#endif