#include <atmel_start.h>
#include "app/application.h"

// Turn all LEDs off
void leds_off () {
	for (int i=1; i<=8; i++)
		gpio_set_pin_level(GPIO(GPIO_PORTA, i+1), false);
}

// Turn all LEDs on
void leds_on () {
	for (int i=1; i<=8; i++)
		gpio_set_pin_level(GPIO(GPIO_PORTA, i+1), true);
}

// Toggle an LED
void toggle_led (int led) {
	gpio_toggle_pin_level(GPIO(GPIO_PORTA, led+1));
}

// Blink an LED
void blink(int led, int ms) {
	toggle_led(led);
	delay_ms(ms);
	toggle_led(led);
}

// Blink all LEDs
void blinkall (int count, int ms) {
	for (int x=0; x<count; x++) {
		leds_on();
		delay_ms(ms);
		leds_off();
		delay_ms(ms);
	}
}

// Progress LEDs left to right
void progress (int speed) {
	leds_off();
	for (int i=1; i<=8; i++) {
		toggle_led(i);
		delay_ms(speed);
	}
}

// Progress LEDs right to left
void progress_rev (int speed) {
	leds_off();
	for (int i=8; i>=1; i--) {
		toggle_led(i);
		delay_ms(speed);
	}
}

// Marquee LEDs left to right
void marquee (int speed) {
	leds_off();
	for (int i=1; i<=8; i++) {
		blink(i, speed);
		delay_ms(speed);
	}
}

// Marquee LEDs right to left
void marquee_rev (int speed) {
	leds_off();
	for (int i=8; i>=1; i--) {
		blink(i, speed);
		delay_ms(speed);
	}
}

// Test an init result and give feedback with LEDs
void test (uint8_t val) {
	if (val == 1) {
		// Failed - blink LEDs
		blinkall(6, 500);
	} else {
		// Succeeded! blink a marquee
		for(int i=0; i<6; i++) {
			marquee(20);
			delay_ms(200);
		}
	}
}

/* MAIN */
int main (void) {
	// Initialize MCU, drivers and middleware
	atmel_start_init();
	millis_init();
	
	// Init USB CDC serial
	//while (serial_init() != 0);
	
	/*
	// Test USB serial output
	static const char *banner = "Serial out!\r\n";
	progress(500);
	test(serial_writeString(banner));
	*/
	
	// Init CR95HF
	progress(100);
	test(CR95HF_init());
	
	// Look for tags
	/*while (true) {
		CR95HF_Card tag = CR95HF_tagHunter();
		if (tag != CR95HF_CARD_NONE) {
			// Found a valid tag - blink!
			marquee(40);
		}
	}*/
}
