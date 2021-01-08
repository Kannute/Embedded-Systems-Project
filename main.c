#include <stdint.h>
#include <stdio.h>
#include "Board_LED.h" 
#include "LPC17xx.h"
#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include <math.h>
#include "asciiLib.h"
#include "TP_Open1768.h"
#include "PIN_LPC17xx.h" 
/*
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

		char button_pressed = '-';
		int temp1=0;
		int temp2=0;
		static int v=0;

		short list[ ] = {1023,1022,1016,1006,993,976,954,931,904,874,841,806,768,728,
687,645,601,557,512,468,423,379,337,296,256,219,183,150,120,
93,69,48,31,18,8,2,0,2,8,18,31,48,69,93,120,150,183,219,256,
296,337,379,423,468,512,557,601,645,687,728,768,806,841,
874,904,931,954,976,993,1006,1016,1022,1023};


void calibrate()
{
    int x1 = 50;
    int y1 = 30;

    int x2 = 200;
    int y2 =30;

    int x3 = 120;
    int y3 = 250;

    int x1_p = 510;
    int y1_p = 1000;

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
    return ((alfax* x + betax* y + deltax)/1);
}


int getY(int x, int y)
{
    return (alfay* x + betay* y + deltay)/1;
}

void toDAC(short val)
{
	LPC_DAC->DACR=val<<6;
}


void TIMER0_IRQHandler(void)  {
				
				toDAC( list[v]);
				v++;
				v%=72;
		
			if(list[v] == 1023) 
				v=0;
      LPC_TIM0->IR = 1;
}

void EINT3_IRQHandler(void)
{

		NVIC_DisableIRQ(EINT3_IRQn);
		
		int x;
		int y;
		char string1[46];

		touchpanelGetXY(&x, &y);
		sprintf(string1,"%d %d",getX(x,y), getY(x,y));

		print_string(100,260,string1);
		int temp1=getX(x,y);
		int temp2=getY(x,y);
		x=temp1;
		y=temp2;
	
		if(y<=230&&y>=0)
		{
			if(x<=30&&x>=0)
			{
				print_string(100,300,"1");
				
			}
			else if(x<=60)
			{
				print_string(100,300,"2");
			}
			else if(x<=90)
			{
				print_string(100,300,"3");
			}
			else if(x<=120)
			{
				print_string(100,300,"4");
			}
			else if(x<=150)
			{
				print_string(100,300,"5");
			}
			else if(x<=180)
			{
				print_string(100,300,"6");
			}
			else if(x<=210)
			{
				print_string(100,300,"7");
			}
			else if(x<=240)
			{
				print_string(100,300,"8");
			}	
		}
		else
		{
			print_string(100,300,"-");
		}
	
		
	
		temp1=-1;
		temp2=-1;
		LPC_GPIOINT->IO0IntClr=(1<<19);
		
		NVIC_EnableIRQ(EINT3_IRQn);
	
}

int main(void)
{
	////do zrobienia TIMER(do zmiany czestotliwosci sygnalow) lub SysTick - oba powinny dzialac  oraz przerwania
	
	lcdConfiguration();
	init_ILI9325();

	paint();
	
	/* sprawdzanie kalibracja
	
	lcdWriteReg(ADRX_RAM, 50 );
  lcdWriteReg(ADRY_RAM, 30 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	
	lcdWriteReg(ADRX_RAM, 200 );
  lcdWriteReg(ADRY_RAM, 30 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	
	
	lcdWriteReg(ADRX_RAM, 120 );
  lcdWriteReg(ADRY_RAM, 250 );
	lcdWriteReg(DATA_RAM, LCDGreen );
	*/

	
	 calibrate();
	
	
	
touchpanelInit();
	

fill_rect(0, 0,30,230);
fill_rect(31, 0,60,230);
fill_rect(61, 0,90,230);
fill_rect(91, 0,120,230);
fill_rect(121, 0,150,230);
fill_rect(151, 0,180,230);
fill_rect(181, 0,210,230);
fill_rect(211, 0,240,230);



  LPC_PINCON->PINSEL1=2<<20;
	LPC_PINCON->PINMODE1=1<<20;
	
	LPC_SC->PCLKSEL0=1<<22;

	
	
	//PINY
	uint32_t _pin = PIN_Configure(0, 19, 0, 2, 0);
	uint32_t _pin1 = PIN_Configure(0, 2, 1, 2, 0);
	uint32_t _pin2 = PIN_Configure(0, 3, 1, 2, 0);
	
	//Przerwania z GPIO             
	LPC_GPIOINT->IO0IntEnF=(1<<19);
	NVIC_EnableIRQ(EINT3_IRQn);
	
	//TIMER
	LPC_TIM0->PR = 0;
	LPC_TIM0->MCR  = (1<<1) | (1<<0);   
	LPC_TIM0->MR0  = SystemCoreClock/4/1000;  
	LPC_TIM0->TCR=1;     
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	while(1){}
	


	return 0;
}

//Wypelnij tlo
void paint(){
	

	lcdSetCursor(0, 0);
	for(uint16_t x=0; x<LCD_MAX_X; x++){
		for(uint16_t y=0; y<LCD_MAX_Y; y++){
			lcdWriteReg(DATA_RAM,LCDBlack);
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
	
	lcdSetCursor(x1, y1);
	for(uint16_t x=x1; x<x2; x++){
		for(uint16_t y=y1; y<y2; y++){
			lcdSetCursor(x, y);
			lcdWriteReg(DATA_RAM,LCDWhite);
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
