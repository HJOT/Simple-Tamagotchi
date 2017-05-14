#define F_CPU 16000000UL //16 MHz
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Tulostus.c"
#include "Valikko.c"

/*Näppäinten tilat*/
#define BUTTON1_DOWN !(PINA & (1 << PA0))
#define BUTTON2_DOWN !(PINA & (1 << PA1))
#define BUTTON3_DOWN !(PINA & (1 << PA2))
#define BUTTON4_DOWN !(PINA & (1 << PA3))
#define BUTTON5_DOWN !(PINA & (1 << PA4))

/*Ledin tilat*/
#define LED_ON (PORTA |=  (1 << PA6))
#define LED_OFF (PORTA &= ~(1 << PA6))

void init(void) {

   		/* estetään kaikki keskeytykset */
		cli();

        /* kaiutin pinnit ulostuloksi */
        DDRE  |=  (1 << PE4) | (1 << PE5);
        /* pinni PE4 nollataan */
        PORTE &= ~(1 << PE4);
        /* pinni PE5 asetetaan */
        PORTE |=  (1 << PE5);   
        
        /* ajastin nollautuu, kun sen ja OCR1A rekisterin arvot ovat samat */
        /*TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
        TCCR1B |=    (1 << WGM12);
        TCCR1B &=   ~(1 << WGM13);*/

        /* salli keskeytys, jos ajastimen ja OCR1A rekisterin arvot ovat samat */
        /*TIMSK |= (1 << OCIE1A);*/

        /* asetetaan OCR1A rekisterin arvoksi 0x3e (~250hz) */
        /*OCR1AH = 0x00;
        OCR1AL = 0x3e;*/

        /* käynnistä ajastin ja käytä kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* näppäin pinnit sisääntuloksi */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* rele/led pinni ulostuloksi */
		DDRA |= (1 << PA6);

		/* lcd-näytön alustaminen */
		lcd_init();
		lcd_write_ctrl(LCD_ON);
		lcd_write_ctrl(LCD_CLEAR);

}
		char EEMEM eeprom_nimet[10][6];
		int EEMEM eeprom_top_ajat[10];
		char nimet[10][6]={}, toppi[16];
		int top_ajat[10]={};
		uint8_t EEMEM eetesti;

int main(void) 
{		
		/* Lataa TOP10-listan EEPROM-muistista */
		int8_t testi = eeprom_read_byte(&eetesti);
		
		if(testi!=2){
			eeprom_write_block((const void *)nimet, (void *)eeprom_nimet, sizeof(eeprom_nimet));
			eeprom_write_block((const void *)top_ajat, (void *)eeprom_top_ajat, sizeof(eeprom_top_ajat));
			eeprom_update_byte(&eetesti, 2);
		}
		else{
			eeprom_read_block((void *)nimet, (const void *)eeprom_nimet, sizeof(eeprom_nimet));
			eeprom_read_block((void *)top_ajat, (const void *)eeprom_top_ajat, sizeof(eeprom_top_ajat));
		}
		
        /* ikuinen silmukka */
        while (1){
			
			/* Alkuvalikko */
			init();	/* alusta laitteen komponentit */
			int k=0;
			while(1){
				Tulostus("B3: Uusi peli",1,0);
				Tulostus("B5: TOP-lista",2,0);
				if(BUTTON3_DOWN){
					lcd_write_ctrl(LCD_CLEAR);
					_delay_ms(1000);
					break;
				}
				if(BUTTON5_DOWN){
					lcd_write_ctrl(LCD_CLEAR);
					_delay_ms(500);
					while(!BUTTON3_DOWN){
						Tulostus("     TOP10:     ",1,0);
						sprintf(toppi,"%d. %s %d s",k+1,nimet[k],top_ajat[k]);
						Tulostus(toppi,2,0);
						if(BUTTON1_DOWN){
							lcd_write_ctrl(LCD_CLEAR);
							if(k==0){
								k=9;
							}
							else{
								k--;
							}
						}
						_delay_ms(100);
						if(BUTTON5_DOWN){
							lcd_write_ctrl(LCD_CLEAR);
							if(k==9){
								k=0;
							}
							else{
								k++;
							}
						}
						_delay_ms(100);
					}
					_delay_ms(300);
				}
			}
			
			/* Alustaa laitteen muuttujat */
			char nimi[6]="     \0", luvut[16], profiili[16], kirjaimet[28]=" ABCDEFGHIJKLMNOPQRSTUVWXYZ\0";
			int y=0, x=0, sek=0, sekka=0, min=0, val1=1, val2=0, Ajat[4]={0,0,0,0}, energia=100, tila=0, i,j=0; /* sek=kokonaispeliaika, Ajat on nälkäaika, väsymysaika, tylsistymisaika ja sairausaika */
			
			/* Nimen kysely ja kirjoittaminen */
			while(!BUTTON3_DOWN){
				Tulostus("Anna nimi:",1,0);
				lcd_write_ctrl(LCD_DDRAM | (0x40+x));
				lcd_write_ctrl(LCD_ON | 0x01);
				_delay_ms(50);
				if(BUTTON1_DOWN){
					if(y==0){
						y=26;
					}
					else{
						y--;
					}
				_delay_ms(100);
				}
				if(BUTTON5_DOWN){
					if(y==26){
						y=0;
					}
					else{
						y++;
					}
				_delay_ms(100);
				}
				if(BUTTON2_DOWN){
					if(x==0){
						x=4;
					}
					else{
						x--;
					}
				_delay_ms(100);
				}
				if(BUTTON4_DOWN){
					y=0;
					if(x==4){
						x=0;
					}
					else{
						x++;
					}
				_delay_ms(100);
				}
				nimi[x]=kirjaimet[y];
				Tulostus(nimi,2,0);
			}
			lcd_write_ctrl(LCD_CLEAR);
			Tulostus("---TAMAGOTCHI---",1,0);
			_delay_ms(3000);
			lcd_write_ctrl(LCD_CLEAR);
			Tulostus("Nimi: ",1,0);
			Tulostus(nimi,1,6);
			Tulostus("Peli alkaa...",2,0);
			_delay_ms(5000);
			lcd_write_ctrl(LCD_CLEAR);
			srand(TCNT1);
			Ajat[3] = (rand()%340+60);
			TCNT1=0;
			
			/* Varsinainen peli */
			lcd_write_ctrl(LCD_ON);
			lcd_write_ctrl(LCD_CLEAR);
			while(1){
				/* Kello */
				if(TCNT1>=15625){
					lcd_write_ctrl(LCD_CLEAR);
					if(sek<60){
						sprintf(luvut, "Aika: %ds",sek);
					}
					else{
						min=sek/60;
						sekka=sek-60*min;
						sprintf(luvut, "Aika: %dmin %ds",min,sekka);
					}
					TCNT1=0;
					sek++;
					sekka=sek;
					Ajat[0]++;
					if(tila==5){
						if(Ajat[1]>0){
							Ajat[1]=Ajat[1]-2;
						}
						energia = energia + 2;
						Ajat[0]=0;
						Ajat[2]=0;
					}
					else if(tila==4){
						energia = energia-2;
					}
					else{
						Ajat[1]++;
						Ajat[2]++;
					}
					Ajat[3]--;
					energia--;
				}
				
				/* Keskinappi eli enter */
				if(BUTTON3_DOWN){
					if(val2==0){
						val2 = 1;
					}
					else{
						val2=0;
					}
					_delay_ms(200);
					lcd_write_ctrl(LCD_CLEAR);
				}
				
				/* Valikon rakenne */
				switch(val2){
					case 0:
					
						/* Tilailmoitus */
						switch(tila){
							case 1:
								sprintf(profiili,"Nalka!    ");
								Tulostus(profiili,1,0);
								sprintf(profiili,"E: %d%%",energia);
								Tulostus(profiili,1,9);
								break;
							case 2:
								sprintf(profiili,"Vasy!     ");
								Tulostus(profiili,1,0);
								sprintf(profiili,"E: %d%%",energia);
								Tulostus(profiili,1,9);
								break;
							case 3:
								sprintf(profiili,"Tylsa!    ");
								Tulostus(profiili,1,0);
								sprintf(profiili,"E: %d%%",energia);
								Tulostus(profiili,1,9);
								break;
							case 4:
								sprintf(profiili,"Sairas!   ");
								Tulostus(profiili,1,0);
								sprintf(profiili,"E: %d%%",energia);
								Tulostus(profiili,1,9);
								break;
							case 5:
								Tulostus("Zzz...",1,0);
								break;
							default:
								Tulostus("--OK--        ",1,0);
								sprintf(profiili,"E: %d%%",energia);
								Tulostus(profiili,1,9);								
								break;
						}
					
						/* Valikossa liikkuminen, MENU */
						if(BUTTON1_DOWN){
							if(val1==1){
								val1=5;
							}
							else{
								val1--;
							}
						_delay_ms(200);
						}
						if(BUTTON5_DOWN){
							if(val1==5){
								val1=1;
							}
							else{
								val1++;
							}
						_delay_ms(200);
						}
						Valikko(val1);
						break;
						
						
					case 1:
						switch(val1){
							case 1:
								if(tila==5){
									val2=0;
								}
								else{
									Ajat[0]=0;
									energia = energia + 60;
									LED_OFF;
									val2=0;
									tila=0;
								}
								break;
							case 2:
								if(tila==5){
									lcd_write_ctrl(LCD_CLEAR);	
									Tulostus("Herailee...",2,0);
									_delay_ms(4000);
									tila=1;
									val2=0;
									Ajat[0]=40;
								}
								else{
									tila=5;
									val2=0;
								}
								break;
							case 3:
								if(tila==5 || tila==4){
									val2=0;
								}
								else{
									Ajat[2]=0;
									energia = energia - 10;
									LED_OFF;
									val2=0;
									tila=0;
								}
								break;
							case 4:
								if(tila==5){
									val2=0;
								}
								else{
									Ajat[3]=rand()%340+60;
									LED_OFF;
									val2=0;
									tila=0;
								}
								break;
							case 5:
								/*lcd_write_ctrl(LCD_CLEAR);*/
								Tulostus(luvut,1,0);
								Tulostus("Nimi: ",2,0);
								Tulostus(nimi,2,6);
								break;							
						}
						break;
					}
					
				/* LED-Hälytys */	
				if(Ajat[0]>=40 || Ajat[1]>=150 || Ajat[2]>=30 || Ajat[3]<=60){
					LED_ON;
				}
				else{
					LED_OFF;
				}
				
				/* Tilat */
				if(tila!=5){
					if(Ajat[0]>=40){
						tila=1;
					}
					if(Ajat[1]>=150){
						tila=2;
					}
					if(Ajat[1]>=200){
						tila=4;
					}
					if(Ajat[2]>=30){
						tila=3;
					}
					if(Ajat[2]>=60){
						tila=4;
						Ajat[3]=60;
					}
					if(Ajat[3]<=60){
						tila=4;
					}
				}
				
				/* Kuolemat */
				if(energia>=222){
					lcd_write_ctrl(LCD_CLEAR);
					_delay_ms(1000);
					Tulostus("  GAME OVER!!!  ",1,0);
					Tulostus("Syy: Laskeily   ",2,0);
					_delay_ms(5000);
					lcd_write_ctrl(LCD_CLEAR);
					LED_OFF;
					break;
				}
				if(energia<=0){
					lcd_write_ctrl(LCD_CLEAR);
					_delay_ms(1000);
					Tulostus("  GAME OVER!!!  ",1,0);
					Tulostus("Syy: Kuihtuminen",2,0);
					_delay_ms(5000);
					lcd_write_ctrl(LCD_CLEAR);
					LED_OFF;
					break;
				}
				if(Ajat[3]<=0){
					lcd_write_ctrl(LCD_CLEAR);
					_delay_ms(1000);
					Tulostus("  GAME OVER!!!  ",1,0);
					Tulostus("Syy: Sairaus    ",2,0);
					_delay_ms(5000);
					lcd_write_ctrl(LCD_CLEAR);
					LED_OFF;
					break;
				}

			}
			
			/* TOP-lista */
			for(i=0; i<10; i++){
				if(sek<top_ajat[i]){
					j++;
				}
			}
			if(j<10){
				for(i=9; i-1>=j ;i--){
					strcpy(nimet[i],nimet[i-1]);
					top_ajat[i]=top_ajat[i-1];
				}
			strcpy(nimet[j],nimi);
			top_ajat[j]=sek;	
			}
			eeprom_write_block((const void *)nimet, (void *)eeprom_nimet, sizeof(eeprom_nimet));
			eeprom_write_block((const void *)top_ajat, (void *)eeprom_top_ajat, sizeof(eeprom_top_ajat));
			
		}

}
/*ISR(TIMER1_COMPA_vect) {

	 vaihdetaan kaiutin pinnien tilat XOR operaatiolla
 	PORTE ^= (1 << PE4) | (1 << PE5); 
}*/

