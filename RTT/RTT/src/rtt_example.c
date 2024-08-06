#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

#define STRING_HEADER "-- RTT Timer and Alarm Setup --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --\r\n"

volatile uint8_t g_uc_alarmed;

/**
 * \brief Refresh display on terminal.
 *
 * Update the terminal display to show the current time and the alarm status.
 */
static void refresh_display(void)
{
	printf("%c[2J\r", 27);
	printf("Time: %u\n\r", (unsigned int)rtt_read_timer_value(RTT));

	/* Display alarm */
	if (g_uc_alarmed) {
		puts("!!! ALARM !!!\r");
		g_uc_alarmed = 0;  // Reset the alarm status after displaying
	}
}

/**
 * \brief RTT configuration function.
 *
 * Configure the RTT to generate a one second tick, which triggers the RTTINC
 * interrupt.
 */
static void configure_rtt(void)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
#if SAM4N || SAM4S || SAM4E || SAM4C || SAM4CP || SAM4CM || SAMV71 || SAMV70 || SAME70 || SAMS70
	rtt_sel_source(RTT, false);
#endif
	rtt_init(RTT, 32768);  // 32768 Hz = 1 tick per second

	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
}

/**
 * \brief Configure the console UART.
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
 * \brief Interrupt handler for the RTT.
 *
 * Display the current time on the terminal.
 */
void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* Time has changed, refresh display */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		refresh_display();
	}

	/* Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		g_uc_alarmed = 1;
		/* Set the next alarm for 5 seconds later */
		rtt_write_alarm_time(RTT, rtt_read_timer_value(RTT) + 5);
		refresh_display();
	}
}

/**
 * \brief Application entry point for RTT example.
 *
 * Initialize the RTT, display the current time, and set an alarm for 5 seconds.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Configure console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure RTT */
	configure_rtt();

	/* Set initial alarm for 5 seconds */
	rtt_write_alarm_time(RTT, rtt_read_timer_value(RTT) + 5);

	/* Initialize alarm state */
	g_uc_alarmed = 0;
	refresh_display();

	/* Main loop */
	while (1) {
		/* Just wait for the alarm to trigger */
	}
}
