#include "lcd.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

void Tulostus(char sana[],int rivi,int x){
	int i, len;
	len = strlen(sana);
	switch(rivi){
		case 1:
			lcd_write_ctrl(LCD_DDRAM|(0x00+x));
			break;
		case 2:
			lcd_write_ctrl(LCD_DDRAM|(0x40+x));
			break;
	}
	for(i=0; i<len; i++){
		lcd_write_data(sana[i]);
		}
		
}
