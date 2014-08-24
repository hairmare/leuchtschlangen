#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define LED_DATA_PIN 11
#define BUTTON_PIN 2

#define ADR_PIN1 3
#define ADR_PIN2 4
#define ADR_PIN3 5
#define ADR_PIN4 7
#define ADR_PIN5 8
#define ADR_PIN6 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(149, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

boolean resultState = false;         // the current state
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

uint32_t color;
uint16_t activePixels = 0;

void setup() {
  //Serial.begin(9600);
  strip.begin();
  strip.setBrightness(64);
  
  // activate internal pull up resitors on all button and address select pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ADR_PIN1, INPUT_PULLUP);
  pinMode(ADR_PIN2, INPUT_PULLUP);
  pinMode(ADR_PIN3, INPUT_PULLUP);
  pinMode(ADR_PIN4, INPUT_PULLUP);
  pinMode(ADR_PIN5, INPUT_PULLUP);
  pinMode(ADR_PIN6, INPUT_PULLUP);
  
  // find i2c address based on adress select pin states
  int address = 0;
  if (!digitalRead(ADR_PIN1)) {
    address += 1;
  }
  if (!digitalRead(ADR_PIN2)) {
    address += 2;
  }
  if (!digitalRead(ADR_PIN3)) {
    address += 4;
  }
  if (!digitalRead(ADR_PIN4)) {
    address += 8;
  }
  if (!digitalRead(ADR_PIN5)) {
    address += 16;
  }
  if (!digitalRead(ADR_PIN6)) {
    address += 32;
  }
  // join i2c
  Wire.begin(address);
  // register event callback for i2c requests
  Wire.onRequest(requestEvent);
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the resultState if the new button state is HIGH
      if (buttonState == LOW) {
        //Serial.println("Button pressed");
        resultState = true;
        
        color = strip.Color(random(255), random(255), random(255));
        
        switch(random(8)) {
          case 1:
            color = strip.Color(255, 0, 0);
          break;
          case 2:
            // reddish green
            color = strip.Color(16, 255, 0);
          break;
          case 3:
            // nicely tuned blue
            color = strip.Color(0, 32, 255);
          break;
          case 4:
            color = strip.Color(128, 4, 128);
          break;
          case 5:
            color = strip.Color(0, 200, 180);
          break;
          case 7:
            color = strip.Color(124, 74, 14);
          break;
          case 8:
            color = strip.Color(255, 255, 0);
          break;
        }
        //    color = strip.Color(255, 255, 0);
        //color = strip.Color(110, 4, 255);
        //Serial.print("color: ");
        //Serial.println(color);
       
        // set all the pixels to the new color value on each change:
        //Serial.println(strip.numPixels());
        for(uint16_t i = 0; i < strip.numPixels(); i++) {
          for(uint16_t j = 0; j < strip.numPixels(); j++) {
            if (i > j) {
              strip.setPixelColor(j, color);
            } else {
              strip.setPixelColor(j, 0, 0, 0);
            }
          }
          strip.show();
          delay(50);
          if (digitalRead(BUTTON_PIN) == HIGH) {
            i = strip.numPixels();
          }
        }
      } else {
        for(uint16_t j = 0; j < strip.numPixels(); j++) {
          //Serial.print(".");
          strip.setPixelColor(j, 0, 0, 0);
          //Serial.print("color (inner): ");
          //Serial.println(color);
        }
        strip.show();
        resultState = false;
      }
      //Serial.print("\n");
    }
  }
  if (resultState == true) {
  } else {
  }
    

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

/**
 * handle i2c request event by sending current state as boolean
 */
void requestEvent() {
  Wire.write(resultState);
}
