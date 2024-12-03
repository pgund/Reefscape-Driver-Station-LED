#include <Adafruit_NeoPixel.h> // Adafruit_NeoPixel library

// Define pin for LED strip and number of LEDs
#define LED_PIN 6
#define NUM_LEDS 30

// TODO: update the variables above based on setup

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); // Create an instance of the Adafruit_NeoPixel library

enum LEDState { // States for different LED patterns
  SOLID,
  OFF,
  RAINBOW,
  THEATER_CHASE,
  SOLID_MIDDLE,
  STROBE,
  BREATH,
  WAVE,
  STRIPES,
  COLONIAL_SOLID,
  COLONIAL_STROBE,
  COLONIAL_WAVE,
  COLONIAL_STRIPE
};

LEDState currentState = OFF; // Default state


void setup() {
  leds.begin();  // Initialize LED strip
  leds.show();   // Turn off all LEDs initially
}

void loop() { // Main loop

}

void setSolidColor(uint8_t red, uint8_t green, uint8_t blue) { // Code to set all pixels a certain color
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, leds.Color(red, green, blue));
  }
  leds.show();
}

void turnOffLEDs() { // Turn off all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, 0);
  }
  leds.show();
}

void rainbowEffect() { // Create a rainbow effect across the LED strip
  for (int j = 0; j < 256; j++) { // Cycle through the rainbow
    for (int i = 0; i < NUM_LEDS; i++) {
      int pixelHue = (i * 256 / NUM_LEDS + j) & 255;
      leds.setPixelColor(i, leds.ColorHSV(pixelHue * 65536L / 256));
    }
    leds.show();
    delay(20);
  }
}


void theaterChase(uint8_t red, uint8_t green, uint8_t blue) { // Create a theater chase effect
  for (int q = 0; q < 3; q++) {
    for (int i = 0; i < NUM_LEDS; i += 3) {
      leds.setPixelColor(i + q, leds.Color(red, green, blue)); // Turn on every third LED
    }
    leds.show();
    delay(50);
    for (int i = 0; i < NUM_LEDS; i += 3) {
      leds.setPixelColor(i + q, 0); // Turn off every third LED
    }
  }
}

void solid(double percent, uint32_t color1, uint32_t color2) { // Create a solid color with percentage of two colors
  int color1Pixels = (int)(NUM_LEDS * percent);
  for (int i = 0; i < color1Pixels; i++) {
    leds.setPixelColor(i, color1);
  }
  for (int i = color1Pixels; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, color2);
  }
  leds.show();
}

void solidMiddle(double percent, uint32_t color) { // Create a solid middle effect with black borders
  int middlePixels = (int)(NUM_LEDS * percent);
  int border = (NUM_LEDS - middlePixels) / 2;
  for (int i = 0; i < border; i++) {
    leds.setPixelColor(i, 0);
  }
  for (int i = border; i < border + middlePixels; i++) {
    leds.setPixelColor(i, color);
  }
  for (int i = border + middlePixels; i < NUM_LEDS; i++) {
    leds.setPixelColor(i, 0);
  }
  leds.show();
}

void strobe(uint32_t color, int duration) { // Create a strobe effect
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if ((currentTime - lastTime) >= duration) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds.setPixelColor(i, leds.getPixelColor(i) == 0 ? color : 0);
    }
    leds.show();
    lastTime = currentTime;
  }
}

void breath(uint32_t color1, uint32_t color2, int duration) { // Create a breathing effect
  static int step = 0;
  int brightness = (sin((float)step / duration * 3.14159) + 1) * 127.5;
  uint8_t r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
  uint8_t r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;
  uint8_t r = r1 + (r2 - r1) * brightness / 255;
  uint8_t g = g1 + (g2 - g1) * brightness / 255;
  uint8_t b = b1 + (b2 - b1) * brightness / 255;
  setSolidColor(r, g, b);
  step = (step + 1) % duration;
}

void wave(uint32_t color1, uint32_t color2, float wavelength, int duration) {
  static float offset = 0;
  offset += 3.14159 / duration;
  for (int i = 0; i < NUM_LEDS; i++) {
    float wave = (sin((float)i / wavelength + offset) + 1) / 2;
    uint8_t r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
    uint8_t r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;
    uint8_t r = r1 + (r2 - r1) * wave;
    uint8_t g = g1 + (g2 - g1) * wave;
    uint8_t b = b1 + (b2 - b1) * wave;
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}

void COLONIAL_WAVE(){
  wave(leds.Color(32, 42, 68), leds.Color(192, 180, 91), 10.0, 2000);
}

void COLONIAL_STRIPE(){
  strobe(leds.Color(32, 42, 68), leds.Color(192, 180, 91), 3);
}

void COLONIAL_BREATH(){
  breath(leds.Color(32, 42, 68), leds.Color(192, 180, 91), 1000);
}