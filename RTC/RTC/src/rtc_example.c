#include "asf.h"
#include "stdio_serial.h"
#include "conf_clock.h"
#include "conf_board.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_EOL    "\r"
#define STRING_HEADER "-- RTC Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define TARGET_COUNT 10 

/* Week string */
static uint8_t gs_uc_day_names[7][4] =
		{ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

/* Interrupt counter */
static volatile uint32_t ul_interrupt_count = 0;

/**
 *  Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Refresh display data on the terminal.
 */
static void refresh_display(void)
{
	uint32_t ul_hour, ul_minute, ul_second;
	uint32_t ul_year, ul_month, ul_day, ul_week;

	/* Retrieve date and time */
	rtc_get_time(RTC, &ul_hour, &ul_minute, &ul_second);
	rtc_get_date(RTC, &ul_year, &ul_month, &ul_day, &ul_week);

	/* Clear screen and print new data */
	printf("\033[H\033[J");  // ANSI escape codes to clear the screen
	printf("Current time: %02u:%02u:%02u\n", (unsigned int)ul_hour,
			(unsigned int)ul_minute, (unsigned int)ul_second);

	/* Display the current date */
	printf("Current date: %02u/%02u/%04u %s\n",
			(unsigned int)ul_month, (unsigned int)ul_day,
			(unsigned int)ul_year, (char const *)gs_uc_day_names[ul_week - 1]);
}

/**
 * \brief RTC interrupt handler.
 */
void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);

	/* Second increment interrupt */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		/* Clear the interrupt */
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);
		/* Refresh display */
		refresh_display();

		/* Increment the interrupt counter */
		ul_interrupt_count++;

		/* Check if counter has reached the target count */
		if (ul_interrupt_count >= TARGET_COUNT) {
			printf("Interrupt: Counter has reached %d!\n", TARGET_COUNT);
			/* Reset the counter */
			ul_interrupt_count = 0;
		}
	}
}

/**
 * \brief Application entry point for RTC example.
 * Initialize the system, UART console, RTC, and display the time and date.
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the UART console */
	configure_console();
	puts(STRING_HEADER);

	/* Enable the RTC peripheral clock */
	pmc_enable_periph_clk(ID_RTC);

	/* Configure RTC for 24-hour mode and enable second interrupt */
	rtc_set_hour_mode(RTC, 0);
	rtc_enable_interrupt(RTC, RTC_IER_SECEN);

	/* Enable RTC interrupt in NVIC */
	NVIC_EnableIRQ(RTC_IRQn);

	while (1) {
		/* Enter sleep mode, waiting for RTC interrupt to wake up */
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
