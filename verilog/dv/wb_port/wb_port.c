/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.c>

#define MEM_SIZE (1<<4)

#define WB_BASE 0x32000000
//#define WB_BASE 0x30100000
//#define WB_BASE 0x30000000

#define CTRL_ADDR ((volatile uint32_t*)WB_BASE)
#define MATRIX_ADDR ((volatile uint32_t*)(WB_BASE+6))
#define RESULT_ADDR ((volatile uint32_t*)(WB_BASE+2*(MEM_SIZE*MEM_SIZE)+6))

// Knight Co Processor registers
#define kiop_operation (*(CTRL_ADDR+0)) // what to do? (multiplication: 1, ...)
#define kiop_width_a (*(CTRL_ADDR+1)) // width of matrix A
#define kiop_height_a (*(CTRL_ADDR+2)) // height of matrix A
#define kiop_width_b (*(CTRL_ADDR+3)) // width of matrix B
#define kiop_height_b (*(CTRL_ADDR+4)) // height of matrix B
#define kiop_status (*(CTRL_ADDR+5)) // write -1 to start, check later for error codes

// Matrix A values
#define matrix_a0 (*(CTRL_ADDR+6))
#define matrix_a1 (*(CTRL_ADDR+7))
#define matrix_a2 (*(CTRL_ADDR+8))
#define matrix_a3 (*(CTRL_ADDR+9))

// Matrix B values
#define matrix_b0 (*(CTRL_ADDR+10))
#define matrix_b1 (*(CTRL_ADDR+11))
#define matrix_b2 (*(CTRL_ADDR+12))
#define matrix_b3 (*(CTRL_ADDR+13))

// Matrix C values
#define matrix_c0 (*(CTRL_ADDR+2*(MEM_SIZE*MEM_SIZE)+6+0))
#define matrix_c1 (*(CTRL_ADDR+2*(MEM_SIZE*MEM_SIZE)+6+1))
#define matrix_c2 (*(CTRL_ADDR+2*(MEM_SIZE*MEM_SIZE)+6+2))
#define matrix_c3 (*(CTRL_ADDR+2*(MEM_SIZE*MEM_SIZE)+6+3))

/*
  Wishbone Test:
    - Configures MPRJ lower 8-IO pins as outputs
    - Checks counter value through the wishbone port
*/

void main()
{
    /*
    IO Control Registers
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |
    Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |

    Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
    | DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
    | 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |
    */

    /*
     * Set up the housekeeping SPI to be connected internally so
     * that external pin changes don't affect it.
     */

    reg_wb_enable = 1;
    reg_spi_enable = 1;
    //reg_spimaster_config = 0xa002;	// Enable, prescaler = 2,
                                          // connect to housekeeping SPI

    // All GPIO pins are configured to be output
    // Used to flad the start/end of a test
    reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_28 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_27 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_26 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_25 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_24 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_23 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_22 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_21 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_20 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_19 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_18 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_17 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_16 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_6 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_0  = GPIO_MODE_USER_STD_OUTPUT;

     /* Apply configuration */
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);

    reg_la2_oenb = reg_la2_iena = 0x00000000;    // [95:64]

    // Flag start of the test
    reg_mprj_datal = 0xAB600000;

    /*
    DIM==2:

    -3  -15  x  9  -15 =     3   120
    -6   7      -2 -5      -68    55

    -3*9 + -15*-2 = -27 + 30 = 3
    -3*-15 + -15*-5 = 45 + 75 = 120
    -6*9 + 7*-2 = -54 + -14 = -68
    -6*-15  + 7*-5 = 90 + -35 = 55

    Correct!
    */

    // Matrix A
    matrix_a0 = -3;
    matrix_a1 = -15;
    matrix_a2 = -6;
    matrix_a3 = 7;

    // Matrix B
    matrix_b0 = 9;
    matrix_b1 = -15;
    matrix_b2 = -2;
    matrix_b3 = -5;

    kiop_operation = 1;
    kiop_width_a = 2;
    kiop_height_a = 2;
    kiop_width_b = 2;
    kiop_height_b = 2;
    kiop_status = -1; // shoot and go

    reg_mprj_datal = matrix_c0;
    reg_mprj_datal = matrix_c1;
    reg_mprj_datal = matrix_c2;
    reg_mprj_datal = matrix_c3;

    /*
    // Matrix B
    INPUT_DEST(10) = 9;
    INPUT_DEST(11) = -15;
    INPUT_DEST(12) = -2;
    INPUT_DEST(13) = -5;

    // Op and dimensions
    INPUT_DEST(0) = 1;
    INPUT_DEST(1) = 2;
    INPUT_DEST(2) = 2;
    INPUT_DEST(3) = 2;
    INPUT_DEST(4) = 2;
    INPUT_DEST(5) = -1; // shoot and go

    buf = OUTPUT_DEST(0);
    buf = OUTPUT_DEST(1);
    buf = OUTPUT_DEST(2);
    buf = OUTPUT_DEST(3);*/

    reg_mprj_datal = 0xAB610000;

}
