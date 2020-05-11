#include <Arduino.h>

#include <FastLED.h>
#include <EEPROM.h>
#include <TimerOne.h>

#include <Button.h>

#include "config.h"
#include "modes.h"

uint32_t timer1;

CHSV hsv;
uint8_t opMode;
uint8_t settingId;

// Buttons
Button button_1(PIN_BUTTON_1);
Button button_2(PIN_BUTTON_2);
Button button_3(PIN_BUTTON_3);
Button button_4(PIN_BUTTON_4);

// FastLED RGB strip
CRGB strip[NUM_LED];

// ISR for debouncing and buzzer timer
void timerIsr()
{
	button_1.update();
	button_2.update();
	button_3.update();
	button_4.update();
}

// print helper
void print_hsv(void) {
	Serial.print(F("H:"));
	Serial.print(hsv.hue);
	Serial.print(F(" S:"));
	Serial.print(hsv.sat);
	Serial.print(F(" V:"));
	Serial.print(hsv.val);
	Serial.println();
}

// load preset from EEPROM
void load_preset(uint16_t eeAddr) {
	opMode = MODE_PRESET;
	EEPROM.get(eeAddr, hsv);
#ifdef DEBUG
	Serial.print(F("Load preset - "));
	print_hsv();
#endif
	fill_solid(strip, NUM_LED, hsv);
	FastLED.show();
}

// save preset to EEPROM
void save_preset(uint16_t eeAddr) {
	EEPROM.put(eeAddr, hsv);
#ifdef DEBUG
	Serial.print(F("Save preset - "));
	print_hsv();
#endif
	opMode = MODE_SAVED;
	fill_solid(strip, NUM_LED, CRGB::Black);
	FastLED.show();
}

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println(F("*\n* LED strip controller\n*"));
#endif

	button_1.begin();
	button_2.begin();
	button_3.begin();
	button_4.begin();

	// Input processing and debouncing
	Timer1.initialize(5000); // 5ms
	Timer1.attachInterrupt(timerIsr);

	FastLED.addLeds<NEOPIXEL, PIN_LED>(strip, NUM_LED);

	timer1 = 0;
	opMode = MODE_MANUAL;
	settingId = 0;
}

void loop() {
	uint8_t bState1 = button_1.getState();
	uint8_t bState2 = button_2.getState();
	uint8_t bState3 = button_3.getState();
	uint8_t bState4 = button_4.getState();


	if (bState1 == Button::Clicked) {
		opMode = MODE_MANUAL;
#ifdef DEBUG
		Serial.println(F("Manual mode"));
#endif
	}
	else {
		if (bState2 == Button::Clicked) {
			load_preset(EE_ADDR_00);
		}
		if (bState3 == Button::Clicked) {
			load_preset(EE_ADDR_01);
		}
		if (bState4 == Button::Clicked) {
			load_preset(EE_ADDR_02);
		}
	}

	if (bState1 == Button::DoubleClicked) {
		opMode = MODE_RAINBOW;
#ifdef DEBUG
		Serial.println(F("*** RAINBOW MODE !!! ***"));
#endif
	}

	uint8_t rainbowHue;

	switch(opMode) {
	case MODE_MANUAL:
		if ((millis() - timer1) > REFRESH_RATE) {
			timer1 = millis();

			// shift values 2 bits right since we only code on 8 bits
			hsv.hue = (uint8_t)(analogRead(PIN_POT_HUE) >> 2);
			hsv.sat = (uint8_t)(analogRead(PIN_POT_SAT) >> 2);
			hsv.val = (uint8_t)(analogRead(PIN_POT_VAL) >> 2);

			fill_solid(strip, NUM_LED, hsv);
			FastLED.show();
		}
		// no break here
	case MODE_PRESET:
		if (bState2 == Button::Held) {
			save_preset(EE_ADDR_00);
		}
		if (bState3 == Button::Held) {
			save_preset(EE_ADDR_01);
		}
		if (bState4 == Button::Held) {
			save_preset(EE_ADDR_02);
		}
		break;
	
	case MODE_SAVED:
		// wait until user releases the button
		if ((bState2 == Button::Released) || (bState3 == Button::Released) || (bState4 == Button::Released)) {
			fill_solid(strip, NUM_LED, hsv);
			FastLED.show();
			opMode = MODE_PRESET;
		}
		break;

	case MODE_RAINBOW:
		rainbowHue = beatsin8(10, 0, 255);
		fill_rainbow(strip, NUM_LED, rainbowHue, 5);
		FastLED.delay(50); // calls show()
		break;
	
	default:
		// we should not be here
#ifdef DEBUG
		Serial.println(F("Error - Fallback to Manual mode"));
#endif
		opMode = MODE_MANUAL;
	}

}