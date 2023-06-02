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

//#define MEM_SIZE (1<<4)
#define MEM_SIZE (1<<8)

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

#define matrix0 (*(CTRL_ADDR+6))
#define matrix1 (*(CTRL_ADDR+7))
#define matrix2 (*(CTRL_ADDR+8))
#define matrix3 (*(CTRL_ADDR+9))

#define matrix4 (*(CTRL_ADDR+10))
#define matrix5 (*(CTRL_ADDR+11))
#define matrix6 (*(CTRL_ADDR+12))
#define matrix7 (*(CTRL_ADDR+13))
#define matrix8 (*(CTRL_ADDR+14))
#define matrix9 (*(CTRL_ADDR+15))

#define matrix10 (*(CTRL_ADDR+16))
#define matrix11 (*(CTRL_ADDR+17))
#define matrix12 (*(CTRL_ADDR+18))
#define matrix13 (*(CTRL_ADDR+19))
#define matrix14 (*(CTRL_ADDR+20))
#define matrix15 (*(CTRL_ADDR+21))
#define matrix16 (*(CTRL_ADDR+22))

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

    // test square

#if 0
    // Matrix A 2x2
    matrix0 = -3;
    matrix1 = -15;
    matrix2 = -6;
    matrix3 = 7;

    // Matrix B 2x2
    matrix4 = 9;
    matrix5 = -15;
    matrix6 = -2;
    matrix7 = -5;

    kiop_operation = 1;
    // A 2 x 2
    kiop_width_a = 2;
    kiop_height_a = 2;
    // B 2 x 2
    kiop_width_b = 2;
    kiop_height_b = 2;
#endif

    // test non square
    /*
     * -3 * -12 +  -15 * -6 = 126
     * 
     * 
     */

#if 0
   /*  
   Matrix A 3x2
   -3 -15 -7
    8 -15 -5

   Matrix B 2x3
   -12  13
    -6  5
     7  12
   */
    // A 3 x 2
    kiop_width_a = 3;
    kiop_height_a = 2;
    // B 2 x 3
    kiop_width_b = 2;
    kiop_height_b = 3;
#endif
   /*  
    Matrix A 2x3
    -3 -15
    -7   8
    -15 -5

   Matrix B 3x2
   -12 -13  -6
     5   7  12
   */

    // Matrix A
    matrix0 = -3;
    matrix1 = -15;
    matrix2 = -7;
    matrix3 = 8;
    matrix4 = -15;
    matrix5 = -5;

    // Matrix B
    matrix6 =  -12;
    matrix7 =  -13;
    matrix8 =  -6;
    matrix9 =  5;
    matrix10 =  7;
    matrix11 =  12;

    kiop_operation = 1; // set config registers r/w
    // A 3 x 2
    kiop_width_a = 2;
    kiop_height_a = 3;
    // B 2 x 3
    kiop_width_b = 3;
    kiop_height_b = 2;

    kiop_status = -1; // shoot and go

    reg_mprj_datal = matrix12;
    reg_mprj_datal = matrix13;
    reg_mprj_datal = matrix14;
    reg_mprj_datal = matrix15;
    reg_mprj_datal = matrix16;

    reg_mprj_datal = 0xAB610000;

}
