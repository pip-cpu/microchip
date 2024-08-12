#include <asf.h>
#include <string.h>
#include <stdio.h>
#define AES_KEY_SIZE 128 // Key size in bits
#define AES_BLOCK_SIZE 16 // AES block size in bytes
static uint32_t aes_key[AES_KEY_SIZE / 32] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c};
static uint8_t input_data[AES_BLOCK_SIZE];
static uint8_t encrypted_data[AES_BLOCK_SIZE];
static uint8_t decrypted_data[AES_BLOCK_SIZE];
void configure_usart(void);
void configure_aes(void);
void encrypt_data(void);
void decrypt_data(void);
void print_hex(const uint8_t *data, size_t length);
int main(void) {
	// Initialize the system and board
	sysclk_init();
	board_init();
	configure_usart();
	configure_aes();
	while (1) {
		// Input the text to encrypt
		printf("Enter text to encrypt (16 characters max): ");
		scanf("%16s", (char*)input_data);
		// Display the entered text
		printf("You entered: %s\n", (char*)input_data);
		// Pad input_data if necessary
		size_t len = strlen((char*)input_data);
		if (len < AES_BLOCK_SIZE) {
			memset(input_data + len, 0, AES_BLOCK_SIZE - len);
		}
		// Encrypt data
		encrypt_data();
		printf("Encrypted data:\n");
		print_hex(encrypted_data, AES_BLOCK_SIZE);
		// Decrypt data
		decrypt_data();
		printf("Decrypted text: %s\n", (char*)decrypted_data);
	}
}
void configure_usart(void) {
	const usart_serial_options_t usart_options = {
		.baudrate = 115200,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};
	stdio_serial_init(CONF_UART, &usart_options);
}
void configure_aes(void) {
	struct aes_config aes_cfg;
	aes_get_config_defaults(&aes_cfg);
	aes_cfg.encrypt_mode = AES_ENCRYPTION;
	aes_cfg.key_size = AES_KEY_SIZE_128;
	aes_cfg.opmode = AES_ECB_MODE;
	aes_cfg.start_mode = AES_MANUAL_START;
	aes_init(AES, &aes_cfg);
	aes_write_key(AES, aes_key);
}
void encrypt_data(void) {
	aes_write_input_data(AES, (uint32_t*)input_data);
	aes_start(AES);
	aes_read_output_data(AES, (uint32_t*)encrypted_data);
}
void decrypt_data(void) {
	aes_write_input_data(AES, (uint32_t*)encrypted_data);
	aes_start(AES);
	aes_read_output_data(AES, (uint32_t*)decrypted_data);
}
void print_hex(const uint8_t *data, size_t length) {
	for (size_t i = 0; i < length; i++) {
		printf("%02x", data[i]);
		if ((i + 1) % 16 == 0) { // Print a new line after every 16 bytes
			printf("\n");
			} else if ((i + 1) % 4 == 0) { // Print a space every 4 bytes
			printf(" ");
		}
	}
	if (length % 16 != 0) { // Ensure a new line at the end if not already added
		printf("\n");
	}
}