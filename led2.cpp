#include <FastLED.h>

/*             DEFINITIONS             */
#define NUM_LEDS 123 // Enter the total number of LEDs on the strip
#define LED_PIN 7    // The pin connected to DATA line to control the LEDs
CRGB leds[NUM_LEDS]; // Array to store LED colors

#define CLK 2        // Clock pin for encoder (Channel A)
#define DT 3         // Data pin for encoder (Channel B)
#define SW 4         // Switch pin for encoder (push-button)

volatile int encoderPosition = 0; // Tracks encoder position
#define MAX_ENCODER_POSITION 20
int positionCase = 0;
int lastCLK; // Last state of CLK pin

bool manual = true; // Whether or not we are connected to the driverstation

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
    trackEncoderRotation(); // Track encoder rotation and update encoder position variable
    switch (manual) {
        case false: // Connected to driverstation; mirroring robot state
            if (checkEncoderButton()) {
                manual = true;
            }
            break;

        case true: // Not connected to driverstation, have complete control over LEDs
            if (checkEncoderButton() && recievingData()) {
                manual = false;
            }
            switch (positionCase) {
                case 1:
                    Serial.println("Case 1: Range 0-5");
                    strobe(CRGB::Red, 100);
                    break;
                case 2:
                    Serial.println("Case 2: Range 5-10");
                    wave(CRGB::Blue, 20, 30);
                    break;
                case 3:
                    Serial.println("Case 3: Range 10-15");
                    strobe(CRGB::Green, 150);
                    break;
                case 4:
                    Serial.println("Case 4: Range 15-20");
                    rainbow();
                    break;
                default:
                    Serial.println("Default case: Out of range");
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
    pinMode(SW, INPUT_PULLUP); // Enable internal pull-up for switch
}

bool recievingData() {
    if (true) {
        setLEDs(CRGB(0, 255, 0));
    } else {
        setLEDs(CRGB(255, 0, 0));
    }
}

/*          ENCODER FUNCTIONS         */
void trackEncoderRotation() {
    int currentCLK = digitalRead(CLK); // Read current state of CLK
    if (currentCLK != lastCLK) { // Check if the CLK pin state has changed (rotation detected)
        if (digitalRead(DT) == HIGH) { // Determine rotation direction based on DT pin
            encoderPosition++; // Clockwise
            if (encoderPosition > MAX_ENCODER_POSITION) { // Wrap around if exceeding MAX_POSITION
                encoderPosition = 0;
            }
        } else {
            encoderPosition--; // Counterclockwise
            if (encoderPosition < 0) { // Wrap around if below 0
                encoderPosition = MAX_ENCODER_POSITION;
            }
        }
        Serial.print("Encoder Position: ");
        Serial.println(encoderPosition); // Print encoder position on a new line

        lastCLK = currentCLK; // Update lastCLK state
    }

    if (encoderPosition >= 0 && encoderPosition < 5) {
        positionCase = 1;
    } else if (encoderPosition >= 5 && encoderPosition < 10) {
        positionCase = 2;
    } else if (encoderPosition >= 10 && encoderPosition < 15) {
        positionCase = 3;
    } else if (encoderPosition >= 15 && encoderPosition <= 20) {
        positionCase = 4; 
    } else {
        positionCase = 0; 
    }
}

bool checkEncoderButton() {
    if (digitalRead(SW) == LOW) {
        return true;
    }
    return false;
}

void resetEncoderPosition() {
    encoderPosition = 0; // Reset encoder position to 0
    Serial.println("Encoder position reset to 0");
}

/*         ENCODER ANIMATIONS         */
void rainbow() {
    static int hue = 0;
    fill_rainbow(leds, NUM_LEDS, hue, 5); // Increment hue per LED
    FastLED.show();
    hue++;
}

void setLEDs(CRGB color) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
}

void clearLEDs() {
    FastLED.clear();
    FastLED.show();
}

void strobe(CRGB color, int interval) {
    static bool isOn = false;                   // Tracks whether the LEDs are ON or OFF
    static unsigned long lastUpdate = 0;       // Tracks the last time the strobe toggled

    unsigned long currentMillis = millis();    // Get current time
    if (currentMillis - lastUpdate >= interval) {
        lastUpdate = currentMillis;            // Update the last toggle time
        isOn = !isOn;                          // Toggle the ON/OFF state

        if (isOn) {
            setLEDs(color); 
        } else {
            setLEDs(CRGB(0,0,0));
        }
        FastLED.show();                        
    }
}

void wave(CRGB color, int waveLength, int speed) {
    static unsigned long lastUpdate = 0;      // Tracks the last time the wave moved
    static int wavePosition = 0;              // Tracks the current position of the wave

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= speed) {
        lastUpdate = currentMillis;           // Update the last update time
        wavePosition++;                       // Increment the wave position
        if (wavePosition >= 256) {            // Wrap wavePosition to avoid overflow
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
    const byte fadeAmount = 128;
    const int cometSize = 5;
    static int position = 0;
    static int direction = 1;
    static unsigned long lastUpdate = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 50) {
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
