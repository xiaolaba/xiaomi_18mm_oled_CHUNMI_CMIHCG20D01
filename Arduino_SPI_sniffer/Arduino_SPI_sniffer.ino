
// https://forum.arduino.cc/t/hacking-a-spi-lcd-display-bus-with-arduino/503154
// clone and modified the code, work with OLED of mijia Induction cooker, controller was not ST7920
// original code file name, LCD_ST7920_SPI_sniffer_v1.ino
// xioalaba, 2023-05-21


#include <SPI.h>
/*
 * LCD_ST7920_SPI_sniffer_v1.0
 *  SNIFFER on LCD Display 122x32dot (chip ST7920) on a SPI line (one-way Serial interface without SS)
 *  using standard SPI library and Arduino UNOv3 pin
 *  Keep on reading to choose the clock freq and number of char to be readen
 *  No need to manage SS because Arduino have to be always in silent read
 *  Alessandro Galli - Jan 19th 2018
 *  
 *  Use as you want + thanks to the Arduino community
 */


 /*  MS12232F lcd display using ST7920 chip  (5V TTL)
  *  Refer to ST7920 datasheet (I've used the v4 datasheet)
  *  The display was set to Half-width HCGROM fonts reading (page 11)
  *  Data read on the bus will drive the ST7920 chip to show the ASCII symbols ( table6 - page 14 is standard ASCII)
  *  Transmission protocol on page 26
  *  Each character showed on the display needs 3-bytes transmission
  *  1st byte is for syncro > you would not need it. It is always 0x11111010
  *  2nd & 3rd byte (just the Upper 4bits (bit 7,6,5,4) of each byte are used, throw away the lower bits)
  *  Upper 2nd-byte bits (from 0x0 to 0x7) sets the raw of the table6
  *  Upper 3nd-byte bits (from 0x0 to 0xF) sets the column of the table6
  *  
  *  In this sketch I read 33 bytes on the SPI bus (display in HCGROM mode shows 30 characters) 
  *  stored in a 33x3byte "volatile" array 
  *  I read 33 character against 30 displayed character because my SNIFFER is not syncronized 
  *  with the start of the package transmission between the controller & display
  *  My alghorithm will loose the first char. In order to find the first useful character I will parse the buffer
  *  in order to find the first 0b11111010 byte..I will start translatio from this point
  *  Anyway you can set it as you want ..try and error
  *  
  *  In my project the result is: "5000W Cnt: 1.5A T:23C Vol:104.3V"  ..with some error in ASCII conversion but not in the SNIFF job
  */
int dimbuffer=99; //each char on the display needs 3byte: 1st= syncro&setup 2nd&3rd=data
volatile byte c[99]; //this array dim have to be the same of dimbuffer but the "volatile" command gives me error msg using "c[dimbuffer]" version
int i=1;
int offset;
uint16_t codice;


// Arduino UNO / NANO, D10, D11, D12, D13 all reserved as hardware SPI pin
/* costants defined in the SPI library
 *  SS   =17 > pin10
 *  MOSI =16 > pin 11  This is the line to be SNIFFED (display SID line)
 *  MISO =14 > pin 12
 *  SCK  =15 > pin 13  
 *  
 *  Mind to share ground between Arduino and the Display
 *  
*/


void setup() {
  // Standard SPI pin are used so DON'T use pin13/12/11/10 for any other job
  pinMode(SCK,  INPUT); 
  pinMode(MOSI, INPUT); //it is the SNIFFING line
  pinMode(MISO, INPUT);
  pinMode(SS,   INPUT);  

  /* SPCR byte setup (as per SPI specifications): SPIE|SPE|DORD|MSTR|CPOL|CPHA|SPR1|SPR2
   *  bit8 SPIE= 1 enable SPI interrupt
   *  bit7 SPE= 1 enable SPI
   *  bit6 DORD= 1 LSB first - 0 MSB first
   *  bit5 MSTR= 1 set as Master - 0 set as SLAVE
   *  bit 4 CPOL= 1 clock idle HIGH - idle LOW
   *  bit3 CHPA= 1 sample data on FALLING - 0 on RISING
   *  bit2+1 SPR1+SPR2= 00 speed fastest - 11 slowest

   * Now different way to setup and start SPI . I choosed the direct register write within the main loop()
   * SPCR = (1 << SPE); // Enable SPI as slave.
   * SPI.beginTransaction(SPISettings(16000000,MSBFIRST,SPI_MODE0));
  */
  
  Serial.begin(115200); //that's is the serial.Monitor baud rate...you can choose
  Serial.println("SPI sniffer setup completed");
  Serial.println("uses oscope, to measure your SPI bus frequency (clock rate)");
  Serial.println("UNO/NANO 5V device, OLED 3v3 device, 100 ohm resistors for each SPI connection");
}


void loop() {
  //Serial.print("\n..start sniff");

  /* 
   * SPI setup 
   * we need to know the clock freq & idle level > I have used an oscilloscope
   * My master2display transmission uses: 
   * > a 1,4microSec period clock >>> 714kHz (measured with a oscilloscope)
   * > idle LOW (if in doubt just try...you have 50% chance)
   * > sample on FALLING edge (as generally used. If in doubt use this)
   * > Each byte was sent with MSBfirst (...so from bit 7 to bit 0) (if you need you will find here a BitReverse() function)
   * 
   * You need to choose the nearest clock speed as possible according to your Hacking SPI HDW
   * SPI standard divisor values are f/2..4..8..16..32..64..128  refer to SPI SPCR+SPSR register reference
   * I have found useful info on http://maxembedded.com/2013/11/the-spi-of-the-avr/ and official Arduino/SPI reference
   * With 16MHz Arduino UNO I needed a 16 divisor in order to achieve something similar to 714kHz
   */

  // here we Start the SPI
  SPCR = 0b01000101;  //setup and start the SPI writing directly the SPCR register
  //SPSR = SPSR & 0b1111110; //bit 0 of SPSR register along with SPCR register bit 1 & 0 (0bxxxxxx01) set Arduino clock divisor to 16

  //SPSR = SPSR & 0b1111110; //bit 0 of SPSR register along with SPCR register bit 1 & 0 (0bxxxxxx01) set Arduino clock divisor to 16

  
    // this loop is the real SPI reading byte per byte , as fast as possible using a "volatile" c[] array
    // I suggest not to perform any other job in this loop in order to avoid to miss any data 
    for (i=0;i<dimbuffer;i++) {
      c[i]= SPI.transfer(0); //0x00 is fake, you can write whatever but just don't connect the MISO line to avoid transmission
    }
      SPCR = 0b00000000;  // we stop the SPI to let Arduino elaborate readen data. Alternatively we can even write LOW just the bit6

    // we have already finished the SNIFF job, the buffer is loaded with raw data
    // now we have time to compute and translate data according to ST7920 protocol

    /* this loop just write down the array
    for (i=0;i<dimbuffer;i++) {
      Serial.print(i); Serial.print(":"); Serial.println(c[i],BIN); 
    }
    */

    //parse the buffer to find the first useful data , read this sketch introduction
    for (i=0;i<dimbuffer;i++) {  
      if (c[i]==0b11111010) break; 
    }
    offset=i;  // i is the offset starting index within the buffer
    Serial.print("\n");

    // according to the ST7920 datasheet we need to translate the data readen
    for (i=offset;i<dimbuffer;) {
      interpreta2(i);  //this function read 3bytes from the buffer + translate + print character
      i=i+3;  // we need to increment the reading index 3by3
    }
// that's it ... simple! ;-)))
}





void interpreta2(int offset) {

    // offset is the index recevered by the parsing job
    codice=0;  //it is a global var so we need to reset each time
    
    /*
    // if you need you can "reverse" the readen data. It's not for me now
    //Serial.print("\n c[0]   verse:");  Serial.print(c[0+offset],BIN);
    //c[1+offset]=Bit_Reverse(c[1+offset]);    
    //Serial.print("\n c[0] reverse:");  Serial.print(c[0+offset],BIN);
    */

    //now we start elaborating the 2&3 byte merging them into the "codice" byte
    //Upper part of the 2nd byte will become bit7..4
    //Upper part of the 3rd byte will become bit3..0
    
    codice = codice | c[1+offset];    //Serial.print("\ncod1:");    Serial.print(codice,BIN);
    codice= codice & 0b11110000;
    c[2+offset]=c[2+offset] >>4;     //Serial.print("\ncod2:");    Serial.print(codice,BIN);
    codice = codice | c[2+offset];    //Serial.print("\ncod3:");    Serial.print(codice,BIN);
    //Serial.print("\nFinal check: 0x"); Serial.print(codice,HEX);
    //Serial.print("\nTranslation: ");
    Serial.write(codice);   //write down the ASCII value of the codice variable
}

// if you need to reverse MSB 2 LSB first you can use this function found on some Arduino forum...
// I apologize I've lost the source link...
unsigned char Bit_Reverse( unsigned char x ) 
{ 
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa); 
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc); 
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0); 
    return x;    
} 

