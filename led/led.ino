/* This code is adapted from Electrictriangle: https://github.com/Electriangle*/

#include <FastLED.h>

// led defs
#define NUM_LEDS      123     // Enter the total number of LEDs on the strip
#define LED_PIN       1       // The pin connected to DATA line to control the LEDs
CRGB leds[NUM_LEDS];

// rotary defs
#define CLK 2    // Clock pin (Channel A)
#define DT 3     // Data pin (Channel B)
#define SW 4     // Switch pin (push-button)

volatile int position = 0;  // Tracks encoder position
int lastCLK;                // Last state of CLK pin
bool resetFlag = false;     // Flag for reset condition

void setup() {
  // encoder setup
  Serial.begin(9600);         // Start serial communication
  setupEncoderPins();
  lastCLK = digitalRead(CLK); // Initialize lastCLK state
  

  // LED setup
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500);    // Set power limit of LED strip to 5V, 1500mA
  FastLED.clear();                                    // Initialize all LEDs to "OFF"


}

void loop() {
  // encoder 
  trackRotation(); // Handle rotary encoder rotation
  checkButton();   // Handle button press to reset position

 /* put in all the animations you want to run */
 switch (position / 5) { // Divide by 10 to create ranges: 0-10, 10-20, etc.
    case 0:
      // Position is between 0 and 9
      Serial.println("Position is between 0 and 10");
      COMTwinklePixels();
      break;
    case 1:
      // Position is between 10 and 19
      Serial.println("Position is between 10 and 20");
      COMKnightRider();
      break;
    case 2:
      // Position is between 20 and 29
      Serial.println("Position is between 20 and 30");
      rainbowCycle(5);
      break;
    case 3:
      // Position is between 30 and 39
      Serial.println("Position is between 30 and 40");
      COMFireYellow();
      break;
    default:
      // Position is greater than or equal to 40 or less than 0
      Serial.println("Position is outside the expected range");
      COMBlue();
      break;
  }

}

void setupEncoderPins() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP); // Enable internal pull-up for switch
}

// Track rotation of the encoder
void trackRotation() {
  int currentCLK = digitalRead(CLK); // Read current state of CLK
  
  // Check if the CLK pin state has changed (rotation detected)
  if (currentCLK != lastCLK) {
    // Determine rotation direction based on DT pin
    if (digitalRead(DT) != currentCLK) {
      position++; // Clockwise
    } else {
      position--; // Counterclockwise
    }

    // Output current position to the Serial Monitor
    Serial.print("Position: ");
    Serial.println(position);
  }

  lastCLK = currentCLK; // Update lastCLK state
}

void checkButton() {
  if (digitalRead(SW) == LOW) { // Button is pressed
    if (!resetFlag) { // Prevent multiple resets during a single press
      resetPosition();
      resetFlag = true;
    }
  } else {
    resetFlag = false; // Clear reset flag when button is released
  }
}

// Reset the position to 0
void resetPosition() {
  position = 0; // Reset position to 0
  Serial.println("Position reset to 0");
}

// LED CODE 

void off() {
  /* This function will turn OFF all the LEDs. */
  FastLED.clear();
  FastLED.show();
}

void setColor(int red, int green, int blue) {
  /* This function will turn ON all the LEDs to a solid color. */
  fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));
  FastLED.show();
}

void fadeOn(int red, int green, int blue) {
  /* This function will turn ON all LEDs with a fade animation. */
  float r, g, b;
  for(int i=0; i<=255; i++) {
    r = (i/256.0)*red;
    g = (i/256.0)*green;
    b = (i/256.0)*blue;
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(2);
  }
}

void fadeOff(int red, int green, int blue) {
  /* This function will turn OFF all LEDs with a fade animation. NOTE: Color values must be provided as an initial starting point for the animation. */
  float r, g, b;
  for(int i=255; i>=0; i--) {
    r = (i/256.0)*red;
    g = (i/256.0)*green;
    b = (i/256.0)*blue;
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(2);
  }
}

void swipeOn(int red, int green, int blue) {
  /* This function will turn ON all LEDs with a line swipe animation. */
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(red, green, blue);
    FastLED.show();
    delay(50);
  }
}

void swipeOff(int red, int green, int blue) {
  /* This function will turn OFF all LEDs with a line swipe animation. NOTE: Try switching `NUM_LEDS` and `0` for i in the for loop for a different effect. */
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
    delay(50);
  }
}

void twinklePixels(int Color, int ColorSaturation, int PixelVolume, int FadeAmount, int DelayDuration) {
  for (int i = 0; i < NUM_LEDS; i++) {
    // Draw twinkling pixels
    if (random(PixelVolume) < 2) {     // Chance for pixel to twinkle
      uint8_t intensity = random(100, 255);     // Configure random intensity
      CRGB set_color = CHSV(Color, ColorSaturation, intensity);     // Configure color with max saturation and variable value (intensity)
      leds[i] = set_color;    // Set the pixel color
    }

    // Fade LEDs
    if (leds[i].r > 0 || leds[i].g > 0 || leds[i].b > 0) {    // Check if pixel is lit
      leds[i].fadeToBlackBy(FadeAmount);    // Fade pixel to black
    }
  }

  FastLED.show();
  delay(DelayDuration);
}

void fire(int Color, int Cooling, int Sparks, int DelayDuration, bool ReverseDirection) {
  static byte heat[NUM_LEDS];   // Array of temperature readings at each simulation cell
  int cooldown;
  
  // 1) Slight cool down for each cell
  for (int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown > heat[i]) {
      heat[i] = 0;
    }
    else {
      heat[i] = heat[i] - cooldown;
    }
  }
  
  // 2) Heat from each cell drifts up and diffuses slightly
  for (int k = (NUM_LEDS - 1); k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  
  // 3) Randomly ignite new Sparks near bottom of the flame
  if (random(255) < Sparks) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
  }
  
  // 4) Convert heat cells to LED colors
  for (int n = 0; n < NUM_LEDS; n++) {
    // Temperature ranges from 0 (black/cold) to 255 (white/hot)
    // Rescale heat from 0-255 to 0-191
    byte temperature = heat[n];
    byte t192 = round((temperature / 255.0) * 191);
    
    // Calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0...63
    heatramp <<= 2; // scale up to 0...252

    int BottomColor[8][3] = {   // Array of Bottom flame colors (for spark)
      {heatramp/2, heatramp/2, 255},      // 0 - blue sparks on white flame
      {255, 255, heatramp},               // 1 - white/yellow sparks on red flame
      {255, heatramp, heatramp},          // 2 - white/red sparks on yellow flame
      {heatramp, heatramp, 255},          // 3 - white/blue sparks on green flame
      {heatramp, heatramp, 255},          // 4 - white/blue sparks on cyan flame
      {255, 255, heatramp},               // 5 - white/yellow sparks on blue flame
      {255, heatramp, heatramp},          // 6 - white/red sparks on purple flame
      {255, heatramp, heatramp},          // 7 - white/red sparks on pink flame
    };
    int MiddleColor[8][3] = {   // Array of Middle flame colors
      {heatramp/2, heatramp/2, heatramp},   // 0 - white/blue
      {255, heatramp, 0},                   // 1 - red/yellow
      {heatramp, heatramp, 0},              // 2 - yellow
      {0, 255, heatramp/2},                 // 3 - green/blue
      {0, heatramp, heatramp},              // 4 - cyan
      {0, heatramp, 255},                   // 5 - blue/green
      {heatramp/3, 0, heatramp/2},          // 6 - purple
      {heatramp, heatramp/4, heatramp*2/3}, // 7 - pink
    };
    int TopColor[8][3] = {      // Array of Top flame colors
      {heatramp, heatramp, heatramp},       // 0 - white
      {heatramp, 0, 0},                     // 1 - red
      {heatramp, heatramp, 0},              // 2 - yellow
      {0, heatramp, 0},                     // 3 - green
      {0, heatramp, heatramp},              // 4 - cyan
      {0, 0, heatramp},                     // 5 - blue
      {heatramp/3, 0, heatramp/2},          // 6 - purple
      {heatramp, heatramp/4, heatramp*2/3}, // 7 - pink
    };

    int Pixel = n;
    if (ReverseDirection) {Pixel = (NUM_LEDS - 1) - n;}

    // Set Pixels according to the three regions of the flame:
    if (t192 > 0x80) {                    // hottest (bottom of flame, heatramp between yellow and white)
      leds[Pixel].setRGB(round(BottomColor[Color][0]), round(BottomColor[Color][1]), round(BottomColor[Color][2]));
    }
    else if (t192 > 0x40) {               // middle (midde of flame, heatramp with analogous Color)
      leds[Pixel].setRGB(round(MiddleColor[Color][0]), round(MiddleColor[Color][1]), round(MiddleColor[Color][2]));
    }
    else {                                // coolest (top of flame, heatramp heatramp with monochromatic Color)
      leds[Pixel].setRGB(round(TopColor[Color][0]), round(TopColor[Color][1]), round(TopColor[Color][2]));
    }

  }
  
  FastLED.show();
  delay(DelayDuration);
}

void knightRider(int red, int green, int blue, int ridingWidth, int delayDuration, int returnDelayDuration){
  for(int i = 0; i < (NUM_LEDS - ridingWidth - 2); i++) {
    FastLED.clear();
    FastLED.show();
    leds[i].setRGB(red/10, green/10, blue/10);
    for(int j = 1; j <= ridingWidth; j++) {
      leds[i + j].setRGB(red, green, blue);
    }
    leds[i + ridingWidth + 1].setRGB(red/10, green/10, blue/10);
    FastLED.show();
    delay(delayDuration);
  }

  delay(returnDelayDuration);

  for(int i = (NUM_LEDS - ridingWidth - 2); i > 0; i--) {
    FastLED.clear();
    FastLED.show();
    leds[i].setRGB(red/10, green/10, blue/10);
    for(int j = 1; j <= ridingWidth; j++) {
      leds[i + j].setRGB(red, green, blue);
    }
    leds[i + ridingWidth + 1].setRGB(red/10, green/10, blue/10);
    FastLED.show();
    delay(delayDuration);
  }
 
  delay(returnDelayDuration);
}

void rainbowCycle(int DelayDuration) {
  byte *c;
  uint16_t i, j;

  for(j=0; j < 256; j++) {
    for(i=0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      leds[NUM_LEDS - 1 - i].setRGB(*c, *(c+1), *(c+2));
    }
    FastLED.show();
    delay(DelayDuration);
  }
}

byte *Wheel(byte WheelPosition) {
  static byte c[3];
 
  if(WheelPosition < 85) {
   c[0] = WheelPosition * 3;
   c[1] = 255 - WheelPosition * 3;
   c[2] = 0;
  }
  else if(WheelPosition < 170) {
   WheelPosition -= 85;
   c[0] = 255 - WheelPosition * 3;
   c[1] = 0;
   c[2] = WheelPosition * 3;
  }
  else {
   WheelPosition -= 170;
   c[0] = 0;
   c[1] = WheelPosition * 3;
   c[2] = 255 - WheelPosition * 3;
  }

  return c;
}

void COMTwinklePixels() {
  twinklePixels(42, 255, 20, 50, 50);
  twinklePixels(170, 255, 20, 50, 50);
}

void COMFireBlue() {
  fire(2, 50, 20, 10, false);
}

void COMFireYellow() {
  fire(5, 50, 20, 10, false);
}

void COMBlue() {
  setColor(0, 255, 0);
}

void COMYellow() {
  setColor(255, 255, 0);
}

void COMKnightRider() {
  knightRider(255, 0, 0, 5, 10, 50);
  knightRider(255, 255, 0, 5, 10, 50);
}