// telem.h
// Definitions for main spaceport telem module
// Matt Rossouw (04/2023)

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// Main function
void obc_main(void);

// System modes
void offload(void);
void diagnostic(void);
void mission(bool test);