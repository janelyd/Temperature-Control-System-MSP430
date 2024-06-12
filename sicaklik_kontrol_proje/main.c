#include <msp430.h> 
#include "LCD.h"

unsigned int adc_value = 0; // ADC degerini saklamak icin degisken
int pot_value = 0;
float derece; // Dereceyi saklamak i�in de�i�ken
unsigned char dizi[2]; // ekrana yaz�lacak degeri saklamak icin dizi

int i,j; // D�ng� icin sayac

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Watchdog timer'� durdur
    BCSCTL1 = CALBC1_1MHZ;      // DCO'yu 1MHz'e ayarla
    DCOCTL = CALDCO_1MHZ; // DCO kontrol register'�n� 1MHz'e ayarla
    LCD_Ayarla(); // Ba�lang��ta LCD ayarlar�n� yap

    P1SEL |= BIT0; // A0 pini olarak ayarla
    P1DIR |= BIT1 + BIT2 + BIT3; // P1.1, P1.2 ve P1.3 c�k�s olarak ayarlandi

    // ****************  ADC ayarlar� ************* //
    ADC10CTL1 = INCH_0; // A0 pini i�in ADC kanal�n� se�
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON; // Referans seviyesi besleme ve toprak arasinda, �rnekleme s�resi ve ADC'yi etkinlestir
    ADC10AE0 |= BIT0; // A0 pini i�in analog giris etkinlestir

    while(1){
        ADC10CTL0 &= ~ENC; // D�n�s�m bitini s�f�rla
        while (ADC10CTL1 & BUSY); // E�er ADC mesgulse bekle
        ADC10CTL0 |= ENC + ADC10SC; // D�n�s�m� baslat
        while (ADC10CTL1 & BUSY); // D�n�s�m tamamlanana kadar bekle

        adc_value = ADC10MEM; // ADC sonucunu oku

        // ADC degerini 0-40 aras�nda bir say�ya d�n�st�r
        pot_value = (unsigned int)(adc_value);
        derece = (pot_value * 3.3) / 1023; // ADC de�erini voltaja d�n�st�r
        derece = derece / 0.0392; // Voltaj� dereceye d�n�st�r (burada gercek volt degeri kullan�lmad�)
        int derece_int = (int)derece; // Dereceyi int t�r�ne d�n�st�r
        int x = 1;

        for(i = 0; i <= 1; i++){
            dizi[i] = ((derece_int / x) % 10) + 48; // Dereceyi karakter dizisine d�n�st�r
            x = x * 10;
        }

        LCD_Temizle(); // Ekran� temizle
        LCD_Git_XY(2, 1); // Kurs�r� 2.sat�r 1.s�tuna g�t�r
        LCD_Yazi_Yaz("derece="); // Ekrana "derece=" yazd�r
        for(i = 0; i <= 1; i++){
            LCD_Karakter_Yaz(dizi[1 - i]); // Dereceyi ekrana yazd�r
        }

        if (derece < 28.00) {
            P1OUT |= BIT1; // P1.1'i y�ksek seviyeye ayarla
            P1OUT &= ~(BIT2 + BIT3); // P1.2 ve P1.3'� d���k seviyeye ayarla
            LCD_Git_XY(1, 1); // Kurs�r� 1.sat�r 1.s�tuna g�t�r
            LCD_Yazi_Yaz("hava soguk"); // Ekrana "hava soguk" yazd�r
        } else if(derece > 28.00 && derece < 35.00) {
            LCD_Git_XY(1, 1); // Kurs�r� 1.sat�r 1.s�tuna g�t�r
            LCD_Yazi_Yaz("hava orta sicak"); // Ekrana "hava orta sicak" yazd�r
            P1OUT &= ~(BIT1 + BIT3); // P1.1 ve P1.3'� d���k seviyeye ayarla
            P1OUT |= BIT2; // P1.2'yi y�ksek seviyeye ayarla
        } else {
            LCD_Git_XY(1, 1); // Kurs�r� 1.sat�r 1.s�tuna g�t�r
            LCD_Yazi_Yaz("hava cok sicak"); // Ekrana "hava cok sicak" yazd�r
            P1OUT ^= BIT3; // P1.3'� toggle yap

            // Bekleme d�ng�s�n� 17 kez tekrar ediyoruz (yakla��k 17 saniye)
            for (j = 0; j < 17; j++) {
                // Yakla��k 1 saniyelik bekleme d�ng�s�
                for(i = 4000000; i > 0; i--); // yakla��k 1 saniye bekle
            }
            P1OUT &= ~(BIT1 + BIT2); // Hem P1.1 hem de P1.2'yi d���k seviyeye ayarla
        }

        // pot_value de�i�kenini kullanarak yapmak istedi�iniz i�lemi ger�ekle�tirin

        __delay_cycles(250); // Bekleme s�resi (250 clock cycle)
    }
}
