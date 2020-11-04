#include <stdlib.h>
#include <atmel_start.h>

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
	for (int i=1; i<=8; i++) {
		toggle_led(i);
		delay_ms(speed);
	}
}

// Progress LEDs right to left
void progress_rev (int speed) {
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

// Jaws close
void jaws_close (int speed) {
	for (int i=1; i<=4; i++) {
		toggle_led(i);
		toggle_led(9-i);
		delay_ms(speed);
	}
}

// Jaws open
void jaws_open (int speed) {
	for (int i=4; i>=1; i--) {
		toggle_led(i);
		toggle_led(9-i);
		delay_ms(speed);
	}
}

// Blink a given pattern
void blink_pattern(int pattern, int speed, int count) {
	for (int i=1; i<=count; i++) {
		switch(pattern) {
			case 0: // Double progress
				leds_off();
				progress(speed);
				delay_ms(speed);
				progress(speed);
				delay_ms(speed);
				break;
			case 1: // Reverse double progress
				leds_off();
				progress_rev(speed);
				delay_ms(speed);
				progress_rev(speed);
				delay_ms(speed);
				break;
			case 2: // Progress forward & back
				leds_off();
				progress(speed);
				delay_ms(speed);
				progress_rev(speed);
				delay_ms(speed);
				break;
			case 3: // Progress back & forward
				leds_off();
				progress_rev(speed);
				delay_ms(speed);
				progress(speed);
				delay_ms(speed);
				break;
			case 4: // Slow blink all
				blinkall(1, speed * 4);
				break;
			case 5: // Fast blink all
				blinkall(2, speed);
				break;
			case 6: // Jaws open/close
				leds_off();
				jaws_close(speed);
				jaws_open(speed);
				delay_ms(speed);
				break;
			case 7: // Marquee back & forth
				leds_off();
				marquee(speed);
				marquee_rev(speed);
				delay_ms(speed);
				break;
			case 8: // Fast marquee forward
				marquee(speed / 2);
				break;
			case 9: // Fast marquee reverse
				marquee_rev(speed / 2);
				break;
		}
	}
}

/* MAIN */
int main(void) {
	// Initialize MCU, drivers and middleware
	atmel_start_init();
	
	// Blink LEDs in random patterns
	int last = 0;
	int pattern = 0;
	int speed = 50;
	int count = 4;
	
	while (true) {
		while (pattern == last) {
			pattern = rand() % 9;
		}
		blink_pattern(pattern, speed, count);
		last = pattern;
		delay_ms(speed * count * 2);
	}
}
