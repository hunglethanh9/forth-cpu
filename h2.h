#ifndef H2_H
#define H2_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**@note STK_SIZE is fixed to 32, but h2.vhd allows for the instantiation of
 * CPUs with different stack sizes (so long as they are a power of 2) */

#define MAX_CORE      (8192u)
#define STK_SIZE      (32u)
#define START_ADDR    (8u)

typedef struct {
	size_t length;
	uint16_t *points;
} break_point_t;

typedef struct {
	uint16_t core[MAX_CORE]; /**< main memory */
	uint16_t rstk[STK_SIZE]; /**< return stack */
	uint16_t dstk[STK_SIZE]; /**< variable stack */
	uint16_t pc;  /**< program counter */
	uint16_t tos; /**< top of stack */
	uint16_t rp;  /**< return stack pointer */
	uint16_t sp;  /**< variable stack pointer */
	bool     ie;  /**< interrupt enable */

	break_point_t bp;
} h2_t; /**< state of the H2 CPU */

typedef enum {
	SYMBOL_TYPE_LABEL,
	SYMBOL_TYPE_CALL,
	SYMBOL_TYPE_CONSTANT,
	SYMBOL_TYPE_VARIABLE,
} symbol_type_e;

typedef struct {
	symbol_type_e type;
	char *id;
	uint16_t value;
} symbol_t;

typedef struct {
	size_t length;
	symbol_t **symbols;
} symbol_table_t;


#define CLOCK_SPEED_HZ             (100000000ULL)
#define VGA_BUFFER_LENGTH          (1 << 13)

#define TIMER_ENABLE_BIT           (15)
#define TIMER_RESET_BIT            (14)
#define TIMER_INTERRUPT_ENABLE_BIT (13)
#define TIMER_ENABLE               (1 << TIMER_ENABLE_BIT)
#define TIMER_RESET                (1 << TIMER_RESET_BIT)
#define TIMER_INTERRUPT_ENABLE     (1 << TIMER_INTERRUPT_ENABLE_BIT)

#define UART_RX_FIFO_EMPTY_BIT     (8)
#define UART_RX_FIFO_FULL_BIT      (9)
#define UART_RX_RE_BIT             (10)
#define UART_TX_FIFO_EMPTY_BIT     (11)
#define UART_TX_FIFO_FULL_BIT      (12)
#define UART_TX_WE_BIT             (13)

#define UART_RX_FIFO_EMPTY         (1 << UART_RX_FIFO_EMPTY_BIT)
#define UART_RX_FIFO_FULL          (1 << UART_RX_FIFO_FULL_BIT)
#define UART_RX_RE                 (1 << UART_RX_RE_BIT)
#define UART_TX_FIFO_EMPTY         (1 << UART_TX_FIFO_EMPTY_BIT)
#define UART_TX_FIFO_FULL          (1 << UART_TX_FIFO_FULL_BIT)
#define UART_TX_WE                 (1 << UART_TX_WE_BIT)

#define PS2_NEW_CHAR_BIT           (8)
#define PS2_NEW_CHAR               (1 << PS2_NEW_CHAR_BIT)

typedef struct {
	uint8_t leds;
	uint16_t vga_cursor;
	uint16_t vga_control;
	uint16_t vga[VGA_BUFFER_LENGTH];

	uint16_t timer_control;
	uint16_t timer;

	uint16_t irc_mask;

	uint8_t uart_getchar_register;

	uint8_t led_8_segments;

	uint8_t switches;

	bool wait;
} h2_soc_state_t;

typedef uint16_t (*h2_io_get)(h2_soc_state_t *soc, uint16_t addr, bool *debug_on);
typedef void     (*h2_io_set)(h2_soc_state_t *soc, uint16_t addr, uint16_t value, bool *debug_on);
typedef void     (*h2_io_update)(h2_soc_state_t *soc);

typedef struct {
	h2_io_get in;
	h2_io_set out;
	h2_io_update update;
	h2_soc_state_t *soc;
} h2_io_t;

/**@todo used the enumerations h2_input_addr_t and h2_output_addr_t to
 * generate constants for use with the assembler */

typedef enum {
	iUart         = 0x6000,
	iSwitches     = 0x6001,
	iTimerCtrl    = 0x6002,
	iTimerDin     = 0x6003,
	iVgaTxtDout   = 0x6004,
	iPs2          = 0x6005,
} h2_input_addr_t;

typedef enum {
	oUart         = 0x6000,
	oLeds         = 0x6001,
	oTimerCtrl    = 0x6002,
	oVgaCursor    = 0x6003,
	oVgaCtrl      = 0x6004,
	o8SegLED      = 0x6005,
	oIrcMask      = 0x6006,
} h2_output_addr_t;

void *allocate_or_die(size_t length);
FILE *fopen_or_die(const char *file, const char *mode);

h2_t *h2_new(uint16_t start_address);
void h2_free(h2_t *h);
int h2_load(h2_t *h, FILE *hexfile);
int h2_save(h2_t *h, FILE *output, bool full);
int h2_run(h2_t *h, h2_io_t *io, FILE *output, unsigned steps, symbol_table_t *symbols, bool run_debugger);

void soc_print(FILE *out, h2_soc_state_t *soc, bool verbose);
h2_soc_state_t *h2_soc_state_new(void);
void h2_soc_state_free(h2_soc_state_t *soc);
h2_io_t *h2_io_new(void);
void h2_io_free(h2_io_t *io);

#endif