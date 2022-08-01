/*
 CODE SPI MASTER
 
 */

#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)
#include <xc.h>
#include <p24FJ128GA006.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPI_soft.h"
#include "lcd.h"
#include "my_delay.h"
#include "dht11.h"

#define REG_NHIET_DO 0x01
#define REG_DO_AM 0x02
#define REG_OUTPUT 0x03
#define CMD_READ 0x15
#define CMD_WRITE 0x20

#define up PORTEbits.RE0
#define down PORTEbits.RE2
#define ok PORTEbits.RE1

#define on 0
#define off 1
#define lamp PORTEbits.RE4
#define fan PORTEbits.RE5

#define btn1 LATGbits.LATG2
#define btn2 LATGbits.LATG3
#define btn3 LATGbits.LATG6
#define btn4 LATGbits.LATG7

unsigned char nhiet_do=0;
unsigned char do_am=0;
unsigned int so_lan_bat_quat=0;
unsigned int so_lan_bat_den=0;

void set_humidity();
void set_temperature();
void check_info();
void refresh_menu(char __dem);
char menu();


int main(int argc,char ** argv)
{
    spi_soft_init();
    //nut nhan
    TRISEbits.TRISE0=1;
    TRISEbits.TRISE1=1;
    TRISEbits.TRISE2=1;
    
    TRISGbits.TRISG2=1;
    TRISGbits.TRISG3=1;
    TRISGbits.TRISG6=1;
    TRISGbits.TRISG7=1;
    
    //lamp & fan
    TRISEbits.TRISE4=0;
    TRISEbits.TRISE5=0; 
    lamp=off;
    fan=off;
    
    lcd_init();
    delay_ms(50);
    lcd_clear();
   
   while(1)
   {
        //hien thi nhiet do
        lcd_string(1,1,"NHIET DO:");
        lcd_char(1,11,nhiet_do/10+'0');
        lcd_char(1,12,nhiet_do%10+'0');
        lcd_char(1,13,0xDF);
        lcd_char(1,14,'c');

        //hien thi do am
        lcd_string(2,1,"DO AM:");
        lcd_char(2,8,do_am/10+'0');
        lcd_char(2,9,do_am%10+'0');
        lcd_char(2,10,'%');
       
        if(ok) menu();
        if(up) 
        {
            lamp=!lamp;
            while(up);
        }
        
        if(down) 
        {
            fan=!fan;
            while(down);
        }
        
        delay_ms(500);
        nhiet_do=spi_soft_write(REG_NHIET_DO);
        delay_ms(10);
        do_am = spi_soft_write(REG_DO_AM);
   }
    
   return (EXIT_SUCCESS); 
}


void set_humidity()
{
	char dem,dem_last;
	 while(ok);
	 //hmax=eep_read(add_hmax);
	 dem=dem_last=10;
	 lcd_clear();
	 lcd_string(1,1,"Setting Hmax="); lcd_char(1,16,'%');
	 lcd_char(1,14,dem/10+0x30);
	 lcd_char(1,15,dem%10+0x30);
	 while(1)
	 {
		 if(dem>99) dem=99;
		 if(dem!=dem_last)
		 {
			 lcd_char(1,14,dem/10+0x30);
			 lcd_char(1,15,dem%10+0x30);
			 dem_last=dem;
		 }
			if(up) {if(dem<99) dem++; else dem=1; delay_ms(100);}
			else if(down) {if(dem>1) dem--; else dem=1; delay_ms(100);}
			else if(ok) {break;}
	 }//while(1)
	dem=5;
	dem_last=0;
	while(ok);
}

void set_temperature()
{
	char dem,dem_last;
	 while(ok);
	 dem=dem_last=40;
	 lcd_clear();
	 lcd_string(1,1,"Setting Tmax="); lcd_char(1,16,'c');
	 lcd_char(1,14,dem/10+0x30);
	 lcd_char(1,15,dem%10+0x30);
	 while(1)
	 {
		 if(dem>99) dem=99;
		 if(dem!=dem_last)
		 {
			 lcd_char(1,14,dem/10+0x30);
			 lcd_char(1,15,dem%10+0x30);
			 dem_last=dem;
		 }
			if(up) {if(dem<99) dem++; else dem=1; delay_ms(100);}
			else if(down) {if(dem>1) dem--; else dem=1; delay_ms(100);}
			else if(ok) {break;}
	 }//while(1)
	dem=5;
	dem_last=0;
	while(ok);
}

void check_info()
{
   while(ok);
   lcd_clear();
   lcd_string(1,1,"FAN ON:");
   lcd_string(2,1,"LAMP ON:");
   while(1)
   {
       lcd_char(1,10,so_lan_bat_quat/1000%10+'0');
       lcd_char(1,11,so_lan_bat_quat/100%10+'0');
       lcd_char(1,12,so_lan_bat_quat/10%10+'0');
       lcd_char(1,13,so_lan_bat_quat%10+'0');
       
       lcd_char(2,10,so_lan_bat_den/1000%10+'0');
       lcd_char(2,11,so_lan_bat_den/100%10+'0');
       lcd_char(2,12,so_lan_bat_den/10%10+'0');
       lcd_char(2,13,so_lan_bat_den%10+'0');
       if(ok) break;
   }
   while(ok);
   lcd_clear();
}


char menu()
{
  char _Exit=0;
  char dem,dem_last;
  //---------------------giao dien menu--------------------
  lcd_clear();
  dem=4;
  dem_last=!dem;
  lcd_char(1,1,0x7e);  
  lcd_string(1,2,"Setting Tmax");
  lcd_string(2,2,"Setting Hmax");
  while(ok);
  while(1)
  {
    //kiem tra su kien nhan nut up
    if(up)
    {
      if(dem<4) dem++; 
      else dem=1; 
      while(up);
    }

    //kiem tra su kien nhan nut down
    if(down)
    {
      if(dem>1) dem--; 
      else dem=4; 
      while(down);
    }    

    //neu co su kien nhan up hoac down thi hien thi cho phu hop
    if(dem!=dem_last)
    {
      lcd_clear();
      dem_last=dem;
      switch(dem)
      {
         
         case 4:
				lcd_char(1,1,0x7e);
                lcd_char(2,1,' ');
				lcd_string(1,2,"Setting Tmax");
				lcd_string(2,2,"Setting Hmax");
         break;       
				 
         case 3:
                lcd_char(1,1,' ');
				lcd_char(2,1,0x7e);
				lcd_string(1,2,"Setting Tmax");
				lcd_string(2,2,"Setting Hmax");
         break;      				 
				 
         case 2:
                 lcd_char(1,1,0x7e);
                 lcd_char(2,1,' ');
				 lcd_string(1,2,"Check info");
				 lcd_string(2,2,"Exit");
         break;   	

         case 1:
                 lcd_char(1,1,' ');
                 lcd_char(2,1,0x7e);
				 lcd_string(1,2,"Check info");
				 lcd_string(2,2,"Exit");
         break;   					 
         
      }//switch(dem)
    }//if(dem!=dem_last)
//-------------------------------------------------------------------------------------
  if(ok)
  {
      switch(dem)
      {
        case 4:  set_temperature(); dem=4; refresh_menu(dem); break;
        case 3:  set_humidity(); dem=3; refresh_menu(dem); break;
		case 2:  check_info(); dem=2; refresh_menu(dem); break;
        case 1: _Exit=1; break;
      }//switch(dem)
  }//if(ok)

  if(_Exit) break;
//-------------------------------------------------------------------------------------
  }//while(1)

   while(ok);
   lcd_clear();
   return 0;
}//char menu()

void refresh_menu(char __dem)
{
    lcd_clear();
    if(__dem==4 || __dem==3)
    {
        lcd_string(1,2,"Setting Tmax");
        lcd_string(2,2,"Setting Hmax");
    }
    else if(__dem==2 || __dem==1) 
    {
        lcd_string(1,2,"Check info");
        lcd_string(2,2,"Exit");
    }
	 
   switch(__dem)
   {
      case 4: lcd_char(1,1,0x7e); break;
      case 3: lcd_char(2,1,0x7e); break;
      case 2: lcd_char(1,1,0x7e); break;
      case 1: lcd_char(2,1,0x7e); break;
      default: lcd_char(1,1,0x7e); break;
   }
}






























































