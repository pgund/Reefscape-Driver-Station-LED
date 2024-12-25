#include <FastLED.h>

/*             DEFINITIONS             */
#define NUM_LEDS 123 // total number of LEDs on the strip
#define LED_PIN 7    // pin connected to DATA line to control the LEDs
CRGB leds[NUM_LEDS]; // array to store LED colors

#define CLK 2        // channel A
#define DT 3         // channel B
#define SW 4         // push-button

volatile int encoderPosition = 0; // tracks encoder position
#define MAX_ENCODER_POSITION 20
int positionCase = 0;
int lastCLK; // last state of CLK pin

bool manual = true; // whether or not we are connected to the driverstation -> if true NOT connected

/*                SETUP                */
void setup() {
    Serial.begin(9600); // Start serial communication
    setupEncoderPins(); // Call setting up encoder pins
    lastCLK = digitalRead(CLK); // Initialize lastCLK state

    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // Setup LEDs on WS2812B strip
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500); // Set power limit of LED strip to 5V, 2500mA
    FastLED.clear();
}

/*                LOOP                 */
void loop() {
    trackEncoderRotation(); // track encoder rotation and update encoder position variable
    switch (manual) {
        case false: // connected to driverstation; mirroring robot state // TODO write functionality (based off of game)
            if (checkEncoderButton()) {
                manual = true;
            }
            break;

        case true: // not connected to driverstation -> have complete control over LEDs
            if (checkEncoderButton() && recievingData()) {
                manual = false;
            }
            switch (positionCase) {
                case 1:
                    Serial.println("Case 1: Range 0-4, TWINKLE");
                    COMtwinkle();
                    break;
                case 2:
                    Serial.println("Case 2: Range 4-8, COMET");
                    comet(CRGB::Blue);
                    comet(CRGB::Yellow);
                    break;
                case 3:
                    Serial.println("Case 3: Range 8-12, STROBE");
                    breathe(CRGB::Blue, CRGB::Yellow)
                    break;
                case 4:
                    Serial.println("Case 4: Range 12-16, STRIPS");
                    stripes();
                    break;
                case 5:
                    Serial.println("Case 5: Range 16-20, RAINBOW");
                    rainbow();
                    break;
                default:
                    Serial.println("Default case: Out of range: RAINBOW");
                    rainbow();
                    break;
            }
            break;
    }
}

/*          HELPER FUNCTIONS          */
void setupEncoderPins() {
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP); // enable internal pull-up for switch
}

bool recievingData() { //TODO write functionality for this
    clearLEDs();
    if (true) {
        setLEDs(CRGB(0, 255, 0));
    } else {
        setLEDs(CRGB(255, 0, 0));
    }
    FastLED.show();
}

/*          ENCODER FUNCTIONS         */
void trackEncoderRotation() { // https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/
    int currentCLK = digitalRead(CLK); 
    if (currentCLK != lastCLK) {
        if (digitalRead(DT) != currentCLK) { // clockwise rotation
            encoderPosition++;
            if (encoderPosition > MAX_ENCODER_POSITION) { // wrap around if exceeding max encoder position
                encoderPosition = 0;
            }
        } else { // counterclockwise rotation
            encoderPosition--;
            if (encoderPosition < 0) { // wrap around if below 0
                encoderPosition = MAX_ENCODER_POSITION;
            }
        }

        Serial.print("Encoder Position: ");
        Serial.println(encoderPosition);
    }

    lastCLK = currentCLK;

    if (encoderPosition >= 0 && encoderPosition < 5) {
        positionCase = 1;
    } else if (encoderPosition >= 4 && encoderPosition < 8) {
        positionCase = 2;
    } else if (encoderPosition >= 8 && encoderPosition < 12) {
        positionCase = 3;
    } else if (encoderPosition >= 12 && encoderPosition <= 16) {
        positionCase = 4;
    } else if (encoderPosition >= 16 && encoderPosition <= 20) {
        positionCase = 5;
    } else {
        positionCase = 0;
    }
}


bool checkEncoderButton() {
    if (digitalRead(SW) == LOW) { // checks for signal
        return true;
    }
    return false;
}

void resetEncoderPosition() {
    encoderPosition = 0; // reset encoder position to 0
    Serial.println("Encoder position reset to 0");
}

/*         ENCODER ANIMATIONS         */
void rainbow() {
    static int hue = 0;
    fill_rainbow(leds, NUM_LEDS, hue, 5); // increment hue per LED -> this makes the leds seem to move
    FastLED.show();
    hue = hue + 7;
}

void setLEDs(CRGB color) { // puts all the leds to a certain color
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
}

void clearLEDs() {
    FastLED.clear();
    FastLED.show();
}

void strobe(CRGB color, int interval) { // flashes a color, interval is the time between the flashes
    static bool isOn = false;                  
    static unsigned long lastUpdate = 0;       

    unsigned long currentMillis = millis();    
    if (currentMillis - lastUpdate >= interval) {
        lastUpdate = currentMillis;            
        isOn = !isOn;                          

        if (isOn) {
            setLEDs(color); 
        } else {
            setLEDs(CRGB(0,0,0));
        }
        FastLED.show();                        
    }
}

void wave(CRGB color, int waveLength, int speed) {
    static unsigned long lastUpdate = 0;      
    static int wavePosition = 0;              

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= speed) {
        lastUpdate = currentMillis;           
        wavePosition++;                       // increment the wave position -> makes it move
        if (wavePosition >= 256) {            // wrap wavePosition to avoid overflow
            wavePosition = 0;
        }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        int brightness = (sin8((i * 256 / waveLength) + wavePosition)); // Calculate brightness
        leds[i] = color;
        leds[i].fadeToBlackBy(255 - brightness); // Adjust brightness
    }
    FastLED.show();                           // Show the updated state
}

void comet(CRGB color) { // https://github.com/davepl/DavesGarageLEDSeries/blob/master/LED%20Episode%2006/src/comet.h
    const byte fadeAmount = 225;
    const int cometSize = 10;
    static int position = 0;
    static int direction = 1;
    static unsigned long lastUpdate = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 1) {
        lastUpdate = currentMillis;

        FastLED.clear();

        for (int i = 0; i < cometSize; i++) {
            if (position + i < NUM_LEDS) {
                leds[position + i] = color;
            }
        }

        for (int j = 0; j < NUM_LEDS; j++) {
            if (random(10) > 5) {
                leds[j].fadeToBlackBy(fadeAmount);
            }
        }

        FastLED.show();

        position += direction;

        if (position <= 0 || position >= NUM_LEDS - cometSize) {
            direction *= -1;
        }

        if (position == 0 && direction == 1) {
            return;
        }
    }
}

void bounce(CRGB color) {
    
}

void twinkle() { // https://github.com/Electriangle/Twinkle_Pixels_Main/blob/main/Twinkle_Pixels_Main/Twinkle_Pixels_Main.ino
    static unsigned long lastUpdate = 0;  // to control the timing of the twinkle effect

    int pixelVolume = 20;  // set the volume of pixels that twinkle
    int fadeAmount = 20;   // set the fade amount for LEDs

    unsigned long currentMillis = millis(); // get the current time
    if (currentMillis - lastUpdate >= 50) {  // adjust the frequency of the effect
        lastUpdate = currentMillis;

        for (int i = 0; i < NUM_LEDS; i++) {
            // Ccance for a pixel to twinkle
            if (random(pixelVolume) < 2) {
                CRGB color;
                // alternate between yellow and blue for the twinkle effect
                if (random(2) == 0) {
                    color = CRGB::Yellow; // yellow
                } else {
                    color = CRGB::Blue; // blue
                }

                uint8_t intensity = random(50, 255);  // set random intensity for each twinkle
                leds[i] = color;
                leds[i].fadeToBlackBy(255 - intensity);  // apply twinkle effect with intensity
            }

            // fade the LEDs gradually
            if (leds[i].getAverageLight() > 0) {
                leds[i].fadeToBlackBy(fadeAmount);  // fade pixel to black
            }
        }
        FastLED.show();  // update the LEDs
    }
}

void COMtwinkle() { // basically the exact same as twinkle() but with two colors now
    static unsigned long lastUpdate = 0; 

    int pixelVolume = 20; 
    int fadeAmount = 20;

    unsigned long currentMillis = millis(); 
    if (currentMillis - lastUpdate >= 50) {  
        lastUpdate = currentMillis;

        for (int i = 0; i < NUM_LEDS; i++) {
            if (random(pixelVolume) < 2) {
                CRGB color;
              
                if (random(2) == 0) {
                    color = CRGB::Yellow;
                } else {
                    color = CRGB::Blue; 
                }

                uint8_t intensity = random(50, 255);
                leds[i] = color;
                leds[i].fadeToBlackBy(255 - intensity);
            }

            if (leds[i].getAverageLight() > 0) {
                leds[i].fadeToBlackBy(fadeAmount); 
            }
        }
        FastLED.show();  
    }
}

void stripes() {
    static int offset = 0; 

    for (int i = 0; i < NUM_LEDS; i++) {
        if ((i + (offset/2)) % 6 == 0) {
            leds[i] = CRGB::Blue;
            leds[i-1] = CRGB::Blue; 
            leds[i-2] = CRGB::Blue; 
        } else {
            leds[i] = CRGB::Yellow; 
    }

    offset++;
    if (offset >= NUM_LEDS) {
        offset = 0;
    }

    FastLED.show(); 
}

void breathe(CRGB color1, CRGB color2) {
    static unsigned long lastUpdate = 0; 
    static float progress = 0.0;         
    static int direction = 1;           // 1 for increasing, -1 for decreasing

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= 10) {
        lastUpdate = currentMillis;

        progress += direction * (1.0 / (3000 / 10.0)); // 3000 is three seconds
        if (progress >= 1.0 || progress <= 0.0) {
            direction *= -1;
        }

        CRGB currentColor = blend(color1, color2, progress * 255);
        fill_solid(leds, NUM_LEDS, currentColor);
        FastLED.show();
    }
}
