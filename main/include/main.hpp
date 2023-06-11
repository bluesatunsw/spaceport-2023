// telem.h
// Definitions for main spaceport telem module
// Matt Rossouw (04/2023)

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

// Main function
void obc_main(void);

// System modes
void offload(void);
void diagnostic(void);
void mission(bool test);