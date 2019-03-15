// ST7735TestMain.c
// Runs on TM4C123
// Modified graphics code to test line drawing and 3D rendering
// Heavily influenced by code written by Jonathan Valvano http://users.ece.utexas.edu/~valvano/


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

#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"

#define Perlin 1
#if Perlin == 1
	#define FZ			17
	#define SCALE		5
	#define ZSCALE	1
	#define XSTRIP	144/SCALE
	#define ZSTRIP	80/SCALE
int32_t terrain[ZSTRIP+1][XSTRIP];
#endif

#if Perlin == 1
static int SEED = 3;

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
                     185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
    int tmp = hash[(y + SEED) & 255];
    return hash[(tmp + x) & 255];
}

int32_t lin_inter(int32_t x, int32_t y, int32_t s)
{
    return x + s * (y-x);
}

int32_t smooth_inter(int32_t x, int32_t y, int32_t s)
{
    return lin_inter(x, y, s * s * (3-2*s));
}

int32_t noise2d(int32_t x, int32_t y)
{
    int x_int = x/100;
    int y_int = y/100;
    int32_t x_frac = x%100;
    int32_t y_frac = y%100;
	
    int s = noise2(x_int, y_int);
    int t = noise2(x_int+1, y_int);
    int u = noise2(x_int, y_int+1);
    int v = noise2(x_int+1, y_int+1);
    int32_t low = smooth_inter(s, t, x_frac);
    int32_t high = smooth_inter(u, v, x_frac);
    return smooth_inter(low, high, y_frac);
}

int32_t perlin2d(int32_t x, int32_t y, int32_t freq, int depth)
{
    int32_t xa = x*freq%832;
    int32_t ya = y*freq%832;
    int32_t amp = 10;
    int32_t fin = 0;
    int32_t div = 0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}

// Slightly modified Bresenham's algorithm
void makeLine(int8_t x, int8_t y, int8_t z, int8_t x1, int8_t y1, int8_t z1, uint16_t color) {			
	int16_t cx = 64 + (x*FZ/(z));
	int16_t cy = 55 + ((y+24)*FZ/(z));
	int16_t cx1 = 64 + (x1*FZ/(z1));
	int16_t cy1 = 55 + ((y1+24)*FZ/(z1));
	
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
			ST7735_DrawPixel(cx, cy, color);
			if (balance >= 0) {
				cy += incy;
				balance -= dx;
			}
			balance += dy;
			cx += incx;
		}
		ST7735_DrawPixel(cx, cy, color);
	}
	else {
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (cy != cy1) {
			ST7735_DrawPixel(cx, cy, color);
			if (balance >= 0) {
				cx += incx;
				balance -= dy;
			}
			balance += dx;
			cy += incy;
		} 
		ST7735_DrawPixel(cx, cy, color);
	}
}
#endif

#if Perlin == 1
void runTerrain(void) {
	int16_t x, z;
	int i, j;
	int ioff = ZSTRIP * 40;
	while(1) {
		int16_t z1;
		/*setAddrWindow(0, 40, 128, 160);
		for(z = 0; z < 128*120; z++){
			pushColor(0);
		}*/
		for(z = 2; z <= ZSTRIP; z++){
			uint16_t color = 0xFFFF - ((z-2)<<12) - ((z-2)<<7) - ((z-2)<<1);
			if(z > 30)
				color = 0x0000;
			for(x = -XSTRIP/2; x < XSTRIP/2 - 1; x++) {
				if((z * 2) <= ZSTRIP + 2){
					z1 = (z - 2) * 2;
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE, (x+1)*SCALE, terrain[z1][(x+1)+XSTRIP/2],     (z1+2)*ZSCALE, 0x0000);
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE,     x*SCALE,       terrain[z1+1][x+XSTRIP/2], (z1+3)*ZSCALE, 0x0000);
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE, (x+1)*SCALE,   terrain[z1+1][(x+1)+XSTRIP/2], (z1+3)*ZSCALE, 0x0000);	
					z1 += 1;
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE, (x+1)*SCALE, terrain[z1][(x+1)+XSTRIP/2],     (z1+2)*ZSCALE, 0x0000);
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE,     x*SCALE,       terrain[z1+1][x+XSTRIP/2], (z1+3)*ZSCALE, 0x0000);
					makeLine(x*SCALE, terrain[z1][x+XSTRIP/2], (z1+2)*ZSCALE, (x+1)*SCALE,   terrain[z1+1][(x+1)+XSTRIP/2], (z1+3)*ZSCALE, 0x0000);
				}
			  
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE, (x+1)*SCALE, terrain[z-1][(x+1)+XSTRIP/2],     z*ZSCALE, color);
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE,     x*SCALE,       terrain[z][x+XSTRIP/2], (z+1)*ZSCALE, color);
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE, (x+1)*SCALE,   terrain[z][(x+1)+XSTRIP/2], (z+1)*ZSCALE, color);
			}
		}
		/*for(x = -XSTRIP/2; x < XSTRIP/2 - 1; x++) {
			makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE, (x+1)*SCALE, terrain[z-1][(x+1)+XSTRIP/2], z*ZSCALE, 0xFFFF - ((ZSTRIP+1)<<12) - ((ZSTRIP+1)<<7) - ((ZSTRIP+1)<<1));
		}*/
		/*
		for(z = 2; z <= ZSTRIP; z++){
			for(x = -XSTRIP/2; x < XSTRIP/2 - 1; x++) {
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE, (x+1)*SCALE, terrain[z-1][(x+1)+XSTRIP/2],     z*ZSCALE, 0xFFFF);
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE,     x*SCALE,       terrain[z][x+XSTRIP/2], (z+1)*ZSCALE, 0xFFFF);
				makeLine(x*SCALE, terrain[z-1][x+XSTRIP/2], z*ZSCALE, (x+1)*SCALE,   terrain[z][(x+1)+XSTRIP/2], (z+1)*ZSCALE, 0xFFFF);
			}
		}*/
	
		for(i = 1; i <= ZSTRIP; i++){
			for(j = 0; j < XSTRIP; j++){
				terrain[i-1][j] = terrain[i][j];
			}
		}
		for(j = 0; j < XSTRIP; j++){
				terrain[ZSTRIP][j] = perlin2d(j * 20, ioff++, 3, 22)/90000;
		}
		//Delay1ms(100);
	
	}
}
#endif
int main(void){
  PLL_Init(Bus80MHz);                  // set system clock to 80 MHz
  ST7735_InitR(INITR_REDTAB);
	//Wait here then run demo
	#if Perlin == 1
		int i, j;
		for(i = 0; i <= ZSTRIP; i++){
			for(j = 0; j < XSTRIP; j++){
				terrain[i][j] = perlin2d(j * 20, i, 3, 22)/90000; //110000
			}
		}
		runTerrain();
	#endif
	return 1;
}

