#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"
int main(void) {
	
	sysclk_init();
	board_init();

	ioport_init();
	ioport_set_pin_dir(IOPORT_CREATE_PIN(PIOB, 2), IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(IOPORT_CREATE_PIN(PIOB, 3), IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(IOPORT_CREATE_PIN(PIOB, 2), IOPORT_PIN_LEVEL_HIGH);

	
	ioport_set_pin_level(IOPORT_CREATE_PIN(PIOB, 3), IOPORT_PIN_LEVEL_LOW);

	
	while (1) {
		
	}
}