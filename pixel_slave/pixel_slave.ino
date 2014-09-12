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

#define SERIAL_DEBUG 0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(111, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

boolean resultState = false;         // the current state
boolean abortState = true;
boolean endedShow = false;
boolean blockButton = false;
boolean showAll = false;
boolean hideAll = false;
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
long resultStateTime = 0;
int stripDelay = 22;

uint32_t color;
uint32_t initColor = 0;
uint16_t activePixels = 0;

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
#endif
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
  
  // wait for pins to settle
  delay(22);
  
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
  
  // join i2c using calculated address
#ifdef SERIAL_DEBUG
  Serial.print("Joining i2c as ");
  Serial.print(address, HEX);
  Serial.print("\n");
#endif
  Wire.begin(address);
  // register event callback for i2c requests
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  // set up initial color
  switch(random(10)) {
    case 1:
      initColor = strip.Color(255, 0, 0);
    break;
    case 2:
      // reddish green
      initColor = strip.Color(16, 255, 0);
    break;
    case 3:
      // nicely tuned blue
      initColor = strip.Color(0, 32, 255);
    break;
    case 4:
      initColor = strip.Color(128, 4, 128);
    break;
    case 5:
      initColor = strip.Color(0, 200, 180);
    break;
    case 7:
      initColor = strip.Color(124, 74, 14);
    break;
    case 8:
      initColor = strip.Color(255, 255, 0);
    break;
    case 9:
      initColor = strip.Color(255, 128, 0);
    break;
    case 10:
      initColor = strip.Color(102, 0, 0);
    break;
  }
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

    // if the button state has changed we do stuff
    if (!blockButton && reading != buttonState) {
      buttonState = reading;

      // only toggle the resultState if the new button state is HIGH
      if (buttonState == LOW) {
#ifdef SERIAL_DEBUG
        Serial.println("Button pressed");
#endif
        abortState = true;
        endedShow = false;
        
        if (color != 0) {
          initColor = color;
        } else {
          color = initColor;
        }
#ifdef SERIAL_DEBUG
        Serial.print("color: ");
        Serial.println(color);
#endif

        // set all the pixels to the new color value on each change:
        // this first for loops through the available pixel so we can turn them on one at a time
        for(uint16_t i = 0; i <= strip.numPixels(); i++) {
          // the second loop is used for toggling all pixel on/off
          for(uint16_t j = i-1; j <= strip.numPixels(); j++) {
            // we only toogle pixels on thathave been reached in the outer loop
            if (i >= j) {
              strip.setPixelColor(j, color);
            } else {
              strip.setPixelColor(j, 0, 0, 0);
            }
          }
          
          if (i == strip.numPixels()) {
              resultState = true;          
              resultStateTime = millis();
#ifdef SERIAL_DEBUG
        Serial.println("setting resultState");
#endif
          }
          // push newly lighted pixel to the strip and wait some before doing next pixel
          strip.show();
          // @todo debounce this digitalRead (no high prio but still not very nice
          if (digitalRead(BUTTON_PIN) == HIGH) {
            i = strip.numPixels();
            abortState = false;
          } else {
            delay(stripDelay);
          }
        }
      } else {
        //resultState = false;
        //abortState == false;
      }
      if (abortState == false) {
        for(uint16_t j = 0; j < strip.numPixels(); j++) {
          strip.setPixelColor(j, 0, 0, 0);
        }
        strip.show();
        resultState = false;
      }
#ifdef SERIAL_DEBUG
      Serial.print("\n");
#endif
    }
    
    if (!blockButton && digitalRead(BUTTON_PIN) == HIGH) {
      resultState = false;
      abortState = false;
      color = 0;
    }
  }
#ifdef SERIAL_DEBUG
    // Serial.print(resultStateTime);
    // Serial.print(" ");
    // Serial.print(millis());
    // Serial.print("\n");
#endif
  if (color == 0 && !endedShow) {
#ifdef SERIAL_DEBUG
    Serial.println("Ending show");
#endif
    for(uint16_t j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, color);
    }
    strip.show();
    color = initColor;
    endedShow = true;
  }
#ifdef SERIAL_DEBUG
  // Serial.print(".");
#endif
  if (blockButton) {
    resultState = false;
  }
  if (showAll) {
#ifdef SERIAL_DEBUG
    Serial.print("Showing all ");
    Serial.print(initColor);
    Serial.print("\n");
#endif
    for(uint16_t j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, initColor);
    }
    strip.show();
    showAll = false;
  }
  if (hideAll) {
#ifdef SERIAL_DEBUG
    Serial.println("Hiding all");
#endif
    for(uint16_t j = 0; j < strip.numPixels(); j++) {
      strip.setPixelColor(j, 0);
    }
    strip.show();
    hideAll = false;
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

/**
 * handle i2c request event by sending 0 or 1 depending on state
 */
void requestEvent() {
  if (resultState) {
    Wire.write(0x01);
  } else {
    Wire.write(0x00);
  }
}

void receiveEvent(int numBytes) {
  // get register byte
  int i2cRegister = Wire.read();
  if (i2cRegister == 0) {
    // next 3 bytes must be color
    color = strip.Color(Wire.read(), Wire.read(), Wire.read());
    if (color > 0) {
      initColor = color;
    }
  } else if (i2cRegister == 1) {
    strip.setBrightness(Wire.read());
  } else if (i2cRegister == 2) {
    blockButton = Wire.read() == 1;
  } else if (i2cRegister == 3) {
    stripDelay = Wire.read();
  } else if (i2cRegister == 4) {
    showAll = true;
  } else if (i2cRegister == 5) {
    hideAll = true;
  }
  
#ifdef SERIAL_DEBUG
  Serial.println(color);
#endif
  while (0 < Wire.available()) {
#ifdef SERIAL_DEBUG
    Serial.println(":(");
#endif
    // get rid of any remaining bytes
    Wire.read();
  }
}

#ifdef SERIAL_DEBUG
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void serialEvent() {
  resetFunc();
}
#endif
