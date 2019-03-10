// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Daniel Valvano
// March 30, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// hardware connections
// **********ST7735 TFT and SDC*******************
// ST7735
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// **********wide.hk ST7735R with ADXL345 accelerometer *******************
// Silkscreen Label (SDC side up; LCD side down) - Connection
// VCC  - +3.3 V
// GND  - Ground
// !SCL - PA2 Sclk SPI clock from microcontroller to TFT or SDC
// !SDA - PA5 MOSI SPI data from microcontroller to TFT or SDC
// DC   - PA6 TFT data/command
// RES  - PA7 TFT reset
// CS   - PA3 TFT_CS, active low to enable TFT
// *CS  - (NC) SDC_CS, active low to enable SDC
// MISO - (NC) MISO SPI data from SDC to microcontroller
// SDA  – (NC) I2C data for ADXL345 accelerometer
// SCL  – (NC) I2C clock for ADXL345 accelerometer
// SDO  – (NC) I2C alternate address for ADXL345 accelerometer
// Backlight + - Light, backlight connected to +3.3 V

// **********wide.hk ST7735R with ADXL335 accelerometer *******************
// Silkscreen Label (SDC side up; LCD side down) - Connection
// VCC  - +3.3 V
// GND  - Ground
// !SCL - PA2 Sclk SPI clock from microcontroller to TFT or SDC
// !SDA - PA5 MOSI SPI data from microcontroller to TFT or SDC
// DC   - PA6 TFT data/command
// RES  - PA7 TFT reset
// CS   - PA3 TFT_CS, active low to enable TFT
// *CS  - (NC) SDC_CS, active low to enable SDC
// MISO - (NC) MISO SPI data from SDC to microcontroller
// X– (NC) analog input X-axis from ADXL335 accelerometer
// Y– (NC) analog input Y-axis from ADXL335 accelerometer
// Z– (NC) analog input Z-axis from ADXL335 accelerometer
// Backlight + - Light, backlight connected to +3.3 V

#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"

#define Sphere 1
#define Cube 0
#if Sphere == 1
	#define DIMX 	12
	#define DIMY 	14
	#define FZS		15
#else 
	#define DIMYR 	5
	#define DIMZ 	5
	#define FZ 		12
#endif

// Optimizations make the arrays powers of 2
// Use only bitwise anding so no need for modulus

#if Cube == 1
const static int8_t wave[] = {0,2,4,6,7,7,7,6,4,2,0,-2,-4,-6,-7,-7,-7,-6,-4,-2};
// Only needed for z rotations
const static uint8_t zwave[] = {10,10,10,9,8,8,7,6,5,5,5,5,5,6,7,8,8,9,10,10};	
#endif

#if Sphere == 1
void drawOctant(int x0, int y0, int z0, uint16_t color){
	
	int8_t x = 64 + ((x0+5)*FZS/(z0));
	int8_t y = 80 + ((y0+20)*FZS/(z0));
	
	setAddrWindow(x,y,x,y);
	pushColor(color);
}	
void drawcircle(int x1, int y1, int z1, int radius, uint16_t color, uint8_t plane)
{
		int8_t x0 = x1;//(x1*FZ/(z1));
		int8_t y0 = y1;//((y1+10)*FZ/(z1));
		int8_t z0 = z1;
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
				if(plane == 0){
					drawOctant(x0+x, y0+y, z0, color);
					drawOctant(x0+x, y0-y, z0, color);
					drawOctant(x0-x, y0+y, z0, color);
					drawOctant(x0-x, y0-y, z0, color);
					drawOctant(x0+y, y0+x, z0, color);
					drawOctant(x0+y, y0-x, z0, color);
					drawOctant(x0-y, y0+x, z0, color);
					drawOctant(x0-y, y0-x, z0, color);
				}
				else if (plane == 1){
					drawOctant(x0, y0+y, z0+(x/4), color);
					drawOctant(x0, y0-y, z0+(x/4), color);
					drawOctant(x0, y0+y, z0-(x/4), color);
					drawOctant(x0, y0-y, z0-(x/4), color);
					drawOctant(x0, y0+x, z0+(y/4), color);
					drawOctant(x0, y0-x, z0+(y/4), color);
					drawOctant(x0, y0+x, z0-(y/4), color);
					drawOctant(x0, y0-x, z0-(y/4), color);
				}
				else{
					drawOctant(x0+y, y0, z0+(x/4), color);
					drawOctant(x0-y, y0, z0+(x/4), color);
					drawOctant(x0+y, y0, z0-(x/4), color);
					drawOctant(x0-y, y0, z0-(x/4), color);
					drawOctant(x0+x, y0, z0+(y/4), color);
					drawOctant(x0-x, y0, z0+(y/4), color);
					drawOctant(x0+x, y0, z0-(y/4), color);
					drawOctant(x0-x, y0, z0-(y/4), color);
				}
        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
#else
void makeLine(int8_t x, int8_t y, int8_t z, int8_t x1, int8_t y1, int8_t z1, uint16_t color) {			
	int8_t cx = 64 + (x*FZ/(z));
	int8_t cy = 80 + ((y+10)*FZ/(z));
	int8_t cx1 = 64 + (x1*FZ/(z1));
	int8_t cy1 = 80 + ((y1+10)*FZ/(z1));
	
	int8_t	dx, dy;
	int8_t	incx, incy;
	int8_t	balance;

	if (cx1 >= cx) {
		dx = cx1 - cx;
		incx = 1;
	} 
	else {
		dx = cx - cx1;
		incx = -1;
	}

	if (cy1 >= cy) {
		dy = cy1 - cy;
		incy = 1;
	}
	else {
		dy = cy - cy1;
		incy = -1;
	}

	if (dx >= dy) {
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (cx != cx1) {
			setAddrWindow(cx,cy,cx,cy);
			pushColor(color);
			if (balance >= 0) {
				cy += incy;
				balance -= dx;
			}
			balance += dy;
			cx += incx;
		}
		setAddrWindow(cx,cy,cx,cy);
		pushColor(color);
	}
	else {
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (cy != cy1) {
			setAddrWindow(cx,cy,cx,cy);
			pushColor(color);
			if (balance >= 0) {
				cx += incx;
				balance -= dy;
			}
			balance += dx;
			cy += incy;
		} 
		setAddrWindow(cx,cy,cx,cy);
		pushColor(color);
	}
}
#endif
#if Cube == 1
/***** Fun little rotation in xy plane ******
void makeBox(int8_t i, uint16_t color){
	uint8_t z = 5;
	
	int8_t a = wave[i];
	int8_t b = wave[(i+10)%20];
	
	int8_t c = wave[(i+5)%20];
	int8_t d = wave[(i+15)%20];
	
	if(i < 0 && color == 0x0000)
		return;
	//Front Face
	makeLine(a, c, z, c, b, z, color);
	makeLine(a, c, z, d, a, z, color);
	makeLine(b, d, z, c, b, z, color);
	makeLine(b, d, z, d, a, z, color);
	//Back Face
	makeLine(a, c, z+DIMZ, c, b, z+DIMZ, color);
	makeLine(a, c, z+DIMZ, d, a, z+DIMZ, color);
	makeLine(b, d, z+DIMZ, c, b, z+DIMZ, color);
	makeLine(b, d, z+DIMZ, d, a, z+DIMZ, color);
	//Connectors
	makeLine(a, c, z, a, c, z+DIMZ, color);
	makeLine(c, b, z, c, b, z+DIMZ, color);
	makeLine(b, d, z, b, d, z+DIMZ, color);
	makeLine(d, a, z, d, a, z+DIMZ, color);
	
}*/
/***** Fun little rotation in xz plane ******/
void makeBox(int8_t i, uint16_t color){
	uint8_t z = zwave[i];
	uint8_t z1 = zwave[(i+5)%20];
	uint8_t z2 = zwave[(i+10)%20];
	uint8_t z3 = zwave[(i+15)%20];
	
	int8_t a = wave[i];
	int8_t b = wave[(i+10)%20];
	
	int8_t c = wave[(i+5)%20];
	int8_t d = wave[(i+15)%20];
	
	if(i < 0 && color == 0x0000)
		return;
	// Try to make this a loop
	//uint8_t j = 0;
	//for(j = 0; j < 20; j +=5){
		// Do something in here using i and j
	//	makeLine(a, 7, z, c, 7, z1, color);
	//	makeLine(a, -7, z, c, -7, z1, color);
	//	makeLine(a, 7, z, a, -7, z, color);
	//}
	//
	//Top Face
	makeLine(a, DIMYR, z,  c, DIMYR, z1, color);
	makeLine(a, DIMYR, z,  d, DIMYR, z3, color);
	makeLine(b, DIMYR, z2, c, DIMYR, z1, color);
	makeLine(b, DIMYR, z2, d, DIMYR, z3, color);
	//Bottom Face
	makeLine(a, -DIMYR, z, c, -DIMYR, z1, color);
	makeLine(a, -DIMYR, z, d, -DIMYR, z3, color);
	makeLine(b, -DIMYR, z2, c, -DIMYR, z1, color);
	makeLine(b, -DIMYR, z2, d, -DIMYR, z3, color);
	//Connectors
	makeLine(a, DIMYR, z, a, -DIMYR, z, color);
	makeLine(c, DIMYR, z1, c, -DIMYR, z1, color);
	makeLine(b, DIMYR, z2, b, -DIMYR, z2, color);
	makeLine(d, DIMYR, z3, d, -DIMYR, z3, color);
}
void runCube(void) {
	int8_t i;
	int8_t last = -1;
	while(1) {
		for(i = 0; i < 20; i++) {
			makeBox(last, 0x0000);
			makeBox(i, 0xFFFF);
			Delay1ms(250);
			last = i;
		}
		last = 19;
	}
}
#elif Sphere == 1
void makeSphere(int8_t x, int8_t y, uint16_t color){
	drawcircle(x, y, 15, 20, color, 0);
	drawcircle(x, y, 15, 18, color, 1);
	drawcircle(x, y, 15, 18, color, 2);
}
void runSphere(void)	{
	int i, j, k = -1;
	int8_t lasti = -100;
	int8_t lastj = -100;
	for(j = -30; j < 30; j++){
		for(i = -30; i < 30; i++){
			if(lasti != -100){
				makeSphere(lasti*k, lastj, 0x0000);
			}
			makeSphere(i*k, j, 0xFFFF);
			Delay1ms(150);
			lasti = i;
			lastj = j;
		}
		makeSphere(lasti*k, lastj, 0x0000);
		k = k * -1;
	}
}
#endif
int main(void){
  PLL_Init(Bus80MHz);                  // set system clock to 80 MHz
  ST7735_InitR(INITR_REDTAB);
	//Wait here then run demo
	#if Sphere == 1
		runSphere();
	#elif Cube == 1
		runCube();
	#endif
}

