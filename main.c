#include <stdint.h>
#include <stdio.h>
#include "Board_LED.h" 
#include "LPC17xx.h"
#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include <math.h>
#include "asciiLib.h"
#include "TP_Open1768.h"
/*
	#define ADRX_RAM 0x20    //RAM address set X
	#define ADRY_RAM 0x21    //RAM address set Y
	#define DATA_RAM 0x0022 //RAM data
	#define LCDRed             0xF800
	LCD_MAX_Y  320
	LCD_MAX_X  240
*/

void paint();
void paint_line(int, int, int, int);
void paint_straightLine(int, int, int, int);
void fill_rect(int, int, int, int);
void paint_rect(int, int, int, int);
void paint_circle(int, int, int);//TODO
void print_letter(int,int,char);
void print_string(int,int,char*);



long int det(int A00, int A01, int A02, int A10, int A11, int A12, int A20, int A21, int A22){
    return A00*(A11*A22 - A21*A12) - A01 *(A10*A22 -A20*A12) + A02*(A10*A21 - A20*A11);
}

    double alfax;
    double betax;
    double deltax;

    double alfay;
    double betay;
    double deltay;



void calibrate()
{
    int x1 = 50;
    int y1 = 30;

    int x2 = 200;
    int y2 =30;

    int x3 = 120;
    int y3 = 250;

    int x1_p = 510;//678;//490;
    int y1_p = 1000;//2169;//10500;

    int x2_p = 510;//2807;//493;
    int y2_p = 3300;//1327;//33000;

    int x3_p = 3000;//2629;//2970;
    int y3_p = 2100;//3367;//2180;

    long int delta =  det(x1_p, y1_p, 1, x2_p, y2_p, 1, x3_p, y3_p, 1);
    long int dx1 =  det(x1, y1_p, 1, x2, y2_p, 1, x3, y3_p, 1);
    long int dx2 = det(x1_p, x1, 1, x2_p, x2, 1, x3_p, x3, 1);
    long int dx3 = det(x1_p, y1_p, x1, x2_p, y2_p, x2, x3_p, y3_p, x3);
    long int dy1 = det(y1, y1_p, 1, y2, y2_p, 1, y3, y3_p, 1);
    long int dy2 = det(x1_p, y1, 1, x2_p, y2, 1, x3_p, y3, 1);
    long int dy3 = det(x1_p, y1_p, y1, x2_p, y2_p, y2, x3_p, y3_p, y3);

    

    alfax = (double)dx1/delta;
    betax =  (double)dx2/delta;
    deltax = (double)dx3/delta;

    alfay = (double)dy1 / delta;
    betay = (double)dy2/delta;
    deltay = (double)dy3/delta;







}

int getX(int x,int y)
{
    return (alfax* x + betax* y + deltax)/1;
}


int getY(int x, int y)
{
    return (alfay* x + betay* y + deltay)/1;
}






int main(void)
{
	//Zadanie - Zamazac caly ekran LCD
	
	lcdConfiguration();
	init_ILI9325();
//	lcdWriteReg(ADRX_RAM, 100 );
//  lcdWriteReg(ADRY_RAM, 100 );
//	lcdWriteReg(DATA_RAM, LCDGreen );
	paint();
	
	lcdWriteReg(ADRX_RAM, 50 );
  lcdWriteReg(ADRY_RAM, 30 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	
	lcdWriteReg(ADRX_RAM, 200 );
  lcdWriteReg(ADRY_RAM, 30 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	
	
	lcdWriteReg(ADRX_RAM, 120 );
  lcdWriteReg(ADRY_RAM, 250 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	

	
	 calibrate();
	
	//Rysowanie ksztaltow
	//paint();
	//paint_line(10,10,150,180);
	//paint_straightLine(10,10,150,10);
	//paint_rect(10, 10, 100, 100); 
	//fill_rect(10, 10, 100, 100); 
	//paint_circle(10, 100, 100);//TODO
//	print_letter(30,30,'A');
	//print_string(30,30,"ABCDEFGHIJK");
/*	
	//Sprwadzenie ukladu sterujacego
	
	LED_Initialize();
	if(lcdReadReg(0x00) == 0x8989)
		LED_SetOut(0);
	else if (lcdReadReg(0x00) == 0x9325 || lcdReadReg(0x00) == 0x9328)
		LED_SetOut(3);
	*/
	
	touchpanelInit();
	
//	print_letter(130,130, 'R');




  LPC_PINCON->PINSEL1=2<<20;
	LPC_PINCON->PINMODE1=1<<20;
	
	LPC_SC->PCLKSEL0=1<<22;


	LPC_DAC->DACR=1023<<6;
	
	while(1){
		int x;
		int y;
		char string1[46];

		touchpanelGetXY(&x, &y);
		sprintf(string1,"%d %d",getX(x,y), getY(x,y));

		print_string(100,30,string1);
	}
	


	return 0;
}

//Wypelnij tlo
void paint(){
	

	lcdSetCursor(0, 0);
	for(uint16_t x=0; x<LCD_MAX_X; x++){
		for(uint16_t y=0; y<LCD_MAX_Y; y++){
			lcdWriteReg(DATA_RAM,0xF800);
		}
	}
	
}
void print_letter(int x, int y, char c){
	
	unsigned char pBuffer[16];
	GetASCIICode(0, pBuffer, c);
	lcdSetCursor(x,y);
	int x0=x;
	int y0=y;
	int j;
	int j2;
	for(int i=0;i<16;i++)
	{
		
	//	unsigned char temp;
		
		
		
		for( j=7,j2=0;j>=0;j--,j2++)
		{
			lcdSetCursor(x+j2,y+i);
			
			if(pBuffer[i] & (1<<j))
			{
				lcdWriteReg(DATA_RAM,0xF800);
			}
			else
			{
				lcdWriteReg(DATA_RAM,LCDGreen);
			}
			
			y0++;
		}
		x0++;
	}
	

	
}


void print_string(int x, int y,char* s)
{
	int i=0;
	int x0 = x;

	while(s[i]!='\0')
	{
		print_letter(x0,y,s[i]);
		i++;
		x0+=8;
	}
	
}



//Rysuj prosta linie
void paint_straightLine(int x1, int y1, int x2, int y2){
	
	lcdSetCursor(x1, y1);
	
	int dx, dy;
	
	if (x1 < x2)   
		dx = x2 - x1;
    
  else
		dx = x1 - x2;
     

  if (y1 < y2)
    dy = y2 - y1;
     
  else
		dy = y1 - y2;
	
	
	if(x1==x2){
		for(uint16_t y=y1; y<dy; y++){
			lcdSetCursor(x1, y);
			lcdWriteReg(DATA_RAM,0xF800);
		}
	}
	else{
		for(uint16_t x=x1; x<dx; x++){
			lcdSetCursor(x, y1);
			lcdWriteReg(DATA_RAM,0xF800);
		}
	}

}

//Rysowanie kola TODO
void paint_circle(int r, int x, int y){
	
	
	//int y0 = y;
	int value1=0;
	for(int x0 = x - r;x0<=x+r;x0++){
		
		if(r>x0)
			value1=sqrt(r*r - x0*x0);
		else
			value1=sqrt(x0*x0 - r*r);
		
		lcdSetCursor(x0, value1);
		lcdWriteReg(DATA_RAM,0xF800);
		
	}
	//lineFrom(x0 - x, y0 + y, x0 + x, y0 + y);
}

//Rysuj prostokat
void paint_rect(int x1, int y1, int x2, int y2){
	
	paint_line(x1, y1, x1, y2);
	paint_line(x2, y1, x2, y2);
	paint_line(x1, y1, x2, y1);
	paint_line(x1, y2, x2, y2);
	
}
//Wypelnij prostokat
void fill_rect(int x1, int y1, int x2, int y2){
	
	int dx, dy;
	
	if (x1 < x2)   
		dx = x2 - x1;
    
  else
		dx = x1 - x2;
     

  if (y1 < y2)
    dy = y2 - y1;
     
  else
		dy = y1 - y2;

	lcdSetCursor(x1, y1);
	for(uint16_t x=x1; x<dx; x++){
		for(uint16_t y=y1; y<dy; y++){
			lcdSetCursor(x, y);
			lcdWriteReg(DATA_RAM,0xF800);
		}
	}
	
	
}
//Rysuj linie
void paint_line(int x1, int y1, int x2, int y2){
	
     int d, dx, dy, ai, bi, xi, yi;
     int x = x1, y = y1;
	
     // ustalenie kierunku rysowania
     if (x1 < x2)
     {
         xi = 1;
         dx = x2 - x1;
     }
     else
     {
         xi = -1;
         dx = x1 - x2;
     }
     // ustalenie kierunku rysowania
     if (y1 < y2)
     {
         yi = 1;
         dy = y2 - y1;
     }
     else
     {
         yi = -1;
         dy = y1 - y2;
     }
     // pierwszy piksel
     lcdSetCursor(x, y);
		 lcdWriteReg(DATA_RAM,LCDBlack );
     // os wiodaca OX
     if (dx > dy)
     {
         ai = (dy - dx) * 2;
         bi = dy * 2;
         d = bi - dx;
         // petla po kolejnych x
         while (x != x2)
         {
             // test wspólczynnika
             if (d >= 0)
             {
                 x += xi;
                 y += yi;
                 d += ai;
             }
             else
             {
                 d += bi;
                 x += xi;
             }
             lcdSetCursor(x, y);
         }
     }
     // os wiodaca OY
     else
     {
         ai = ( dx - dy ) * 2;
         bi = dx * 2;
         d = bi - dy;
         // petla po kolejnych y
         while (y != y2)
         {
             // test wspólczynnika
             if (d >= 0)
             {
                 x += xi;
                 y += yi;
                 d += ai;
             }
             else
             {
                 d += bi;
                 y += yi;
             }
             lcdSetCursor(x, y);
						 lcdWriteReg(DATA_RAM,LCDBlack );
         }
     }
}
