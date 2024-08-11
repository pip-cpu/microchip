#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

#define BUFFER_SIZE 100

static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = 115200,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT,
	};

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

int main(void)
{
	char buffer[BUFFER_SIZE];

	/* Initialize system and board */
	sysclk_init();
	board_init();

	/* Configure UART for console output */
	configure_console();

	while (1) {
		/* Prompt user to enter a number */
		printf("\r\nEnter a number: "); // Ensure a new line with \r\n

		scanf("%s", buffer);

		/* Print received number */
		printf("\r\nYou entered the number: %s\r\n", buffer); // Ensure a new line with \r\n
	}
}
