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


void paint(void);												//Fill lcd screen
void paint_line(int, int, int, int);						//Paint line on display
void paint_straightLine(int, int, int, int);				//Paint straight line on display
void fill_rect(int, int, int, int);							//Paint filled rectangle on display	
void paint_rect(int, int, int, int);						//Paint hollow ractancle on display
void print_letter(int,int,char);							//Print letter on display
void print_string(int,int,char*);							//Print string on display
long int det(int, int, int, int, int, int, int, int, int); 	//Calculate determinant (used for display-touchscreen calibration)
int getX(int, int);											//Get calibrated X touchscreen coordinate
int getY(int, int);											//Get calibrated Y touchscreen coordinate
void calibrate(void);											//Calibrate display and touchscreen
void volumeUp(void); 							//Change volume up
void volumeDown(void); 							//Change volume down
/******* GLOBAL VARIABLES *******/

//Calibration variables
double alfax;
double betax;
double deltax;
double alfay;
double betay;
double deltay;

double volume = 1.0;						//Volume percent
char button_pressed = '-';			//Button currently pressed on touchscreen -> default (no button pressed) '-'
int temp1=0;						//Temporary value for x coordinate value
int temp2=0;						//Temporary value for y coordinate value
static int v=0;						//Value sent to DAC register
int timer_frequency = 20000;			//4778;////Wave frequency to me sent to DAC - default value 1000
int flag=0;
int flag_play=0;
char string_volume[46];


//Cosine values to be sent to DAC (5 deg steps)
short list[ ] = {1023,1022,1016,1006,993,976,954,931,904,874,841,806,768,728,
687,645,601,557,512,468,423,379,337,296,256,219,183,150,120,
93,69,48,31,18,8,2,0,2,8,18,31,48,69,93,120,150,183,219,256,
296,337,379,423,468,512,557,601,645,687,728,768,806,841,
874,904,931,954,976,993,1006,1016,1022,1023};

short temp[ ] = {1023,1022,1016,1006,993,976,954,931,904,874,841,806,768,728,
687,645,601,557,512,468,423,379,337,296,256,219,183,150,120,
93,69,48,31,18,8,2,0,2,8,18,31,48,69,93,120,150,183,219,256,
296,337,379,423,468,512,557,601,645,687,728,768,806,841,
874,904,931,954,976,993,1006,1016,1022,1023};

int notes[100]={0};
short period[100]={0};

int note_iterator=-1;

int first_time=0;
int second_time=0;


short list_zero[72] = {0};

volatile uint32_t msTicks = 0;  
volatile uint32_t counter = 0;  

void SysTick_Handler(void)  {                             
 /* msTicks++;	
	
	if(flag_play==1)
	{
		NVIC_EnableIRQ(TIMER0_IRQn);
		if(msTicks%10 != 0)
		{
		if(notes[counter]!=0)
			LPC_TIM0->MR0  = SystemCoreClock/4/notes[counter]; //18500;  ////// 240Hz
		
		counter++;
		counter%=100;
		
	
		}
		NVIC_DisableIRQ(TIMER0_IRQn);
	}
*/
}

//Sending value to DAC
void toDAC(short val)
{
	
	LPC_DAC->DACR=val<<6;
}

/* TIMER handler sending values to DAC*/
void TIMER0_IRQHandler(void)  {
	if(flag==1)			
	{
		
		toDAC( list[v]);
	}
		else
	{
		toDAC( 0 );
	}
	
	v++;
	v%=73;
	
	if(list[v] == 1023) 
		v=0;
    LPC_TIM0->IR = 1;
	
	
}


/* IRQ interrupt (touchscreen) handler */
void EINT3_IRQHandler(void)
{

	
	int time = 0;
	first_time = 0;
	second_time = 0;
		NVIC_DisableIRQ(EINT3_IRQn);

	
		NVIC_EnableIRQ(TIMER0_IRQn);
		
		int x;
		int y;
	
		for(int i=0;i<10000;i++){}
	
		char string1[46];

		touchpanelGetXY(&x, &y);
		
		
		for(int i=0;i<1000;i++){}
						
		
						
		sprintf(string1,"play");
			
		print_string(100,260,string1);
		temp1=getX(x,y);
		temp2=getY(x,y);
		x=temp1;
		y=temp2;
		
		if(x>100 && x<160 && y>260 && y< 280 )
		{
			flag_play=1;
		}
			
		if(y<=230&&y>=0)
		{
			
			flag=1;
			first_time=msTicks;
			note_iterator=note_iterator+1;
			if(x<=30&&x>=0)
			{
				print_string(100,300,"1");
				LPC_TIM0->MR0  = SystemCoreClock/4/(17520); //18500;  ////// 240Hz
				if(note_iterator<100)
				notes[note_iterator]=17520;
			}
			else if(x<=60)
			{
				print_string(100,300,"2");
				
				LPC_TIM0->MR0  = SystemCoreClock/4/(19710);  ////// 270Hz
				if(note_iterator<100)
				notes[note_iterator]=19710;
			}
			else if(x<=90)
			{
				print_string(100,300,"3");
				LPC_TIM0->MR0  = SystemCoreClock/4/(21900);  ////// 300Hz
				if(note_iterator<100)
				notes[note_iterator]=21900;
			}
			else if(x<=120)
			{
				print_string(100,300,"4");
				LPC_TIM0->MR0  = SystemCoreClock/4/(23360);  ////// 320Hz
				if(note_iterator<100)
				notes[note_iterator]=23360;
			}
			else if(x<=150)
			{
				print_string(100,300,"5");
				LPC_TIM0->MR0  = SystemCoreClock/4/(26280);  ////// 360Hz
				if(note_iterator<100)
				notes[note_iterator]=26280;
			}
			else if(x<=180)
			{
				print_string(100,300,"6"); 
				LPC_TIM0->MR0  = SystemCoreClock/4/(29200);  ////// 400Hz
				if(note_iterator<100)
				notes[note_iterator]=29200;
			}
			else if(x<=210)
			{
				print_string(100,300,"7");
				LPC_TIM0->MR0  = SystemCoreClock/4/(32850);  ////// 450Hz
				if(note_iterator<100)
				notes[note_iterator]=32850;
			}
			else if(x<=240)
			{
				print_string(100,300,"8");
				LPC_TIM0->MR0  = SystemCoreClock/4/(35040);  ////// 480Hz
				notes[note_iterator]=35040;
			}
				
				
		}
		else
		{
			second_time=msTicks;
			time = second_time - first_time;
			if(x>=30&&x<=70 && y>= 260 && y<= 280)
			{	
					volumeDown();
			}
			else if(x>=168&&x<=205 && y>= 260 && y<= 289)
			{
					volumeUp();
			}
			print_string(100,300,"-");
			flag=0;
			
		}
	
		
		
	
		temp1=-1;
		temp2=-1;
		x=-10;
		y=-100;
		
		LPC_GPIOINT->IO0IntClr=(1<<19);
		
		
		
		
		if(note_iterator<100 && note_iterator>=0){
			period[note_iterator]=second_time-first_time;
		}
		char s1[11]; 
		sprintf(s1,"%d",  time);
	
		
		
		print_string(10,280,s1);
		
		NVIC_EnableIRQ(EINT3_IRQn);
	
}



int main(void)
{
	
	
	lcdConfiguration();
	init_ILI9325();

	paint();
	
	/* Three points for display-touchscreen calibration*/
	/*
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
	
	//Display buttons (one octave)
	fill_rect(0, 0,30,230);
	fill_rect(31, 0,60,230);
	fill_rect(61, 0,90,230);
	fill_rect(91, 0,120,230);
	fill_rect(121, 0,150,230);
	fill_rect(151, 0,180,230);
	fill_rect(181, 0,210,230);
	fill_rect(211, 0,240,230);
	
	///plus
	fill_rect(190, 260,195,290);
	fill_rect(181, 273,205,277);
	
	///minus
	fill_rect(31, 273,61,277);
	
	

  LPC_PINCON->PINSEL1=2<<20;
	LPC_PINCON->PINMODE1=1<<20;
	
	LPC_SC->PCLKSEL0=1<<22;

	
	/*PINS*/
	uint32_t _pin = PIN_Configure(0, 19, 0, 2, 0);
	uint32_t _pin1 = PIN_Configure(0, 2, 1, 2, 0);
	
	
	//GPIO interrupts - touchscreen          


	LPC_GPIOINT->IO0IntEnF=(1<<19);
	LPC_GPIOINT->IO0IntEnR=(1<<19);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	
	
	
	
	//TIMER sending signal to DAC
	LPC_TIM0->PR = 0;
	LPC_TIM0->MCR  = (1<<1) | (1<<0);   
	
	//LPC_TIM0->MR0  = SystemCoreClock/4/timer_frequency;  
	LPC_TIM0->TCR=1;     
	
  
  SysTick_Config(SystemCoreClock / 10); // przerwanie co 0.1s
	
	while(1){
		sprintf(string_volume,"Volume: %.1f",volume);
		print_string(140, 300,string_volume);
	}
	

	
	
	
	return 0;
}




/**************** FUNCTIONS ********************/
/**********************************************/
/**********************************************/
/**********************************************/
/**********************************************/
/**********************************************/
/**********************************************/
/**********************************************/
void volumeUp()
{
	
	
	if(volume<2.0)
	{
		volume+=0.1;
		for(int i=0;i<73;i++)
		{
			list[i]=temp[i];
			list[i] *= volume / 1;
		}
		
	}
	

}


void volumeDown()
{
	
	
	if(volume>0)
	{
		volume-=0.1;
		for(int i=0;i<73;i++)
		{
			list[i]=temp[i];
			list[i] *= volume / 1;
		}
		
	}
	

}



void calibrate()
{
	//Three point display-touchscreen calibration

	//1. point on display
    int x1 = 50;
    int y1 = 30;

	//2. point on display
    int x2 = 200;
    int y2 =30;

	//3. point on display
    int x3 = 120;
    int y3 = 250;


	//1. point coordinates on touchscreen
    int x1_p = 510;
    int y1_p = 1000;

	//2. point coordinates on touchscreen
    int x2_p = 510;
    int y2_p = 3300;

	//3. point coordinates on touchscreen
    int x3_p = 3000;
    int y3_p = 2100;

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


void paint(){
	
	lcdSetCursor(0, 0);
	for(uint16_t x=0; x<LCD_MAX_X; x++){
		for(uint16_t y=0; y<LCD_MAX_Y; y++){
			lcdWriteReg(DATA_RAM,LCDBlack);		//Change color here
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
		
		for( j=7,j2=0;j>=0;j--,j2++)
		{
			lcdSetCursor(x+j2,y+i);
			
			if(pBuffer[i] & (1<<j))
			{
				lcdWriteReg(DATA_RAM,0xF800);		//Change letter color here
			}
			else
			{
				lcdWriteReg(DATA_RAM,LCDGreen);		//Change bg color here
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
			lcdWriteReg(DATA_RAM,0xF800);		//Change color here
		}
	}
	else{
		for(uint16_t x=x1; x<dx; x++){
			lcdSetCursor(x, y1);
			lcdWriteReg(DATA_RAM,0xF800);		//Change color here
		}
	}

}


void paint_rect(int x1, int y1, int x2, int y2){
	
	paint_line(x1, y1, x1, y2);
	paint_line(x2, y1, x2, y2);
	paint_line(x1, y1, x2, y1);
	paint_line(x1, y2, x2, y2);
	
}


void fill_rect(int x1, int y1, int x2, int y2){
	
	lcdSetCursor(x1, y1);
	for(uint16_t x=x1; x<x2; x++){
		for(uint16_t y=y1; y<y2; y++){
			lcdSetCursor(x, y);
			lcdWriteReg(DATA_RAM,LCDWhite);		//Change fill color here
		}
	}
	
	
}

void paint_line(int x1, int y1, int x2, int y2){
	
     int d, dx, dy, ai, bi, xi, yi;
     int x = x1, y = y1;
	
     
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
     
    lcdSetCursor(x, y);
	lcdWriteReg(DATA_RAM,LCDBlack );		//Change color here..
     
     if (dx > dy)
     {
         ai = (dy - dx) * 2;
         bi = dy * 2;
         d = bi - dx;
         
         while (x != x2)
         {
             
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
     
     else
     {
         ai = ( dx - dy ) * 2;
         bi = dx * 2;
         d = bi - dy;
         
         while (y != y2)
         {
           
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
			lcdWriteReg(DATA_RAM,LCDBlack ); 	//.. and here
         }
     }
}

long int det(int A00, int A01, int A02, int A10, int A11, int A12, int A20, int A21, int A22){
    return A00*(A11*A22 - A21*A12) - A01 *(A10*A22 -A20*A12) + A02*(A10*A21 - A20*A11);
} 
