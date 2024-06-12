#include <msp430.h> 
#include "LCD.h"

unsigned int adc_value = 0; // ADC degerini saklamak icin degisken
int pot_value = 0;
float derece; // Dereceyi saklamak için deðiþken
unsigned char dizi[2]; // ekrana yazýlacak degeri saklamak icin dizi

int i,j; // Döngü icin sayac

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Watchdog timer'ý durdur
    BCSCTL1 = CALBC1_1MHZ;      // DCO'yu 1MHz'e ayarla
    DCOCTL = CALDCO_1MHZ; // DCO kontrol register'ýný 1MHz'e ayarla
    LCD_Ayarla(); // Baþlangýçta LCD ayarlarýný yap

    P1SEL |= BIT0; // A0 pini olarak ayarla
    P1DIR |= BIT1 + BIT2 + BIT3; // P1.1, P1.2 ve P1.3 cýkýs olarak ayarlandi

    // ****************  ADC ayarlarý ************* //
    ADC10CTL1 = INCH_0; // A0 pini için ADC kanalýný seç
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON; // Referans seviyesi besleme ve toprak arasinda, örnekleme süresi ve ADC'yi etkinlestir
    ADC10AE0 |= BIT0; // A0 pini için analog giris etkinlestir

    while(1){
        ADC10CTL0 &= ~ENC; // Dönüsüm bitini sýfýrla
        while (ADC10CTL1 & BUSY); // Eðer ADC mesgulse bekle
        ADC10CTL0 |= ENC + ADC10SC; // Dönüsümü baslat
        while (ADC10CTL1 & BUSY); // Dönüsüm tamamlanana kadar bekle

        adc_value = ADC10MEM; // ADC sonucunu oku

        // ADC degerini 0-40 arasýnda bir sayýya dönüstür
        pot_value = (unsigned int)(adc_value);
        derece = (pot_value * 3.3) / 1023; // ADC deðerini voltaja dönüstür
        derece = derece / 0.0392; // Voltajý dereceye dönüstür (burada gercek volt degeri kullanýlmadý)
        int derece_int = (int)derece; // Dereceyi int türüne dönüstür
        int x = 1;

        for(i = 0; i <= 1; i++){
            dizi[i] = ((derece_int / x) % 10) + 48; // Dereceyi karakter dizisine dönüstür
            x = x * 10;
        }

        LCD_Temizle(); // Ekraný temizle
        LCD_Git_XY(2, 1); // Kursörü 2.satýr 1.sütuna götür
        LCD_Yazi_Yaz("derece="); // Ekrana "derece=" yazdýr
        for(i = 0; i <= 1; i++){
            LCD_Karakter_Yaz(dizi[1 - i]); // Dereceyi ekrana yazdýr
        }

        if (derece < 28.00) {
            P1OUT |= BIT1; // P1.1'i yüksek seviyeye ayarla
            P1OUT &= ~(BIT2 + BIT3); // P1.2 ve P1.3'ü düþük seviyeye ayarla
            LCD_Git_XY(1, 1); // Kursörü 1.satýr 1.sütuna götür
            LCD_Yazi_Yaz("hava soguk"); // Ekrana "hava soguk" yazdýr
        } else if(derece > 28.00 && derece < 35.00) {
            LCD_Git_XY(1, 1); // Kursörü 1.satýr 1.sütuna götür
            LCD_Yazi_Yaz("hava orta sicak"); // Ekrana "hava orta sicak" yazdýr
            P1OUT &= ~(BIT1 + BIT3); // P1.1 ve P1.3'ü düþük seviyeye ayarla
            P1OUT |= BIT2; // P1.2'yi yüksek seviyeye ayarla
        } else {
            LCD_Git_XY(1, 1); // Kursörü 1.satýr 1.sütuna götür
            LCD_Yazi_Yaz("hava cok sicak"); // Ekrana "hava cok sicak" yazdýr
            P1OUT ^= BIT3; // P1.3'ü toggle yap

            // Bekleme döngüsünü 17 kez tekrar ediyoruz (yaklaþýk 17 saniye)
            for (j = 0; j < 17; j++) {
                // Yaklaþýk 1 saniyelik bekleme döngüsü
                for(i = 4000000; i > 0; i--); // yaklaþýk 1 saniye bekle
            }
            P1OUT &= ~(BIT1 + BIT2); // Hem P1.1 hem de P1.2'yi düþük seviyeye ayarla
        }

        // pot_value deðiþkenini kullanarak yapmak istediðiniz iþlemi gerçekleþtirin

        __delay_cycles(250); // Bekleme süresi (250 clock cycle)
    }
}
