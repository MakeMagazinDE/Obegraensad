 // THIS FILE IS A DEMO FOR IKEA OBERGRANSÄD KIT
 // AS INTERNET CLOCK

 // (C)2023 DR. ARMIN ZINK
 // parts inspired by 
 // By /u/frumperino
 // goodwires.org


#include <Arduino.h>
#include <pgmspace.h>
#include <ESP8266WiFi.h>

 
#define STASSID "SSID"
#define STAPSK  "PASS"
 

#define P_EN D5  // ORANGE
#define P_DI D6  // YELLO
#define P_CLK D7 // BLUE
#define P_CLA D8 // LILA

#define P_KEY D4 // ROT


// SET YOUR TIMEZONE HERE
#define MY_NTP_SERVER "pool.ntp.org" // set the best fitting NTP server (pool) for your location
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
 

long     mil;
int      brightness=100;
uint8_t  sec;
uint8_t  minute;
uint8_t  hour;
time_t   now;                         // this is the epoch
tm       tm;

#define TT 5                          // delay for digital IO

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// THIS IS SYSTEM FONT FOR 6x7 Bitmaps.with FAT NUMBERS !

static const uint8_t System6x7[] PROGMEM = {
  
    // Fixed width; 
    
    // font data
  0x00, 0x00, 0x00, 0x00, 0x00,0x00,// (space)
  0x00, 0x00, 0x5F, 0x00, 0x00,0x00,// !
  0x00, 0x07, 0x00, 0x07, 0x00,0x00,// "
  0x14, 0x7F, 0x14, 0x7F, 0x14,0x00,// #
  0x24, 0x2A, 0x7F, 0x2A, 0x12,0x00,// $
  0x23, 0x13, 0x08, 0x64, 0x62,0x00,// %
  0x36, 0x49, 0x55, 0x22, 0x50,0x00,// &
  0x00, 0x05, 0x03, 0x00, 0x00,0x00,// '
  0x00, 0x1C, 0x22, 0x41, 0x00,0x00,// (
  0x00, 0x41, 0x22, 0x1C, 0x00,0x00,// )
  0x08, 0x2A, 0x1C, 0x2A, 0x08,0x00,// *
  0x08, 0x08, 0x3E, 0x08, 0x08,0x00,// +
  0x00, 0x50, 0x30, 0x00, 0x00,0x00,// ,
  0x08, 0x08, 0x08, 0x08, 0x08,0x00,// -
  0x00, 0x60, 0x60, 0x00, 0x00,0x00,// .
  0x20, 0x10, 0x08, 0x04, 0x02,0x00,// /


  0b00111110, // 0
  0b01111111,
  0b01100011,
  0b01100011,
  0b01111111,
  0b00111110,

  0b00000000, // 1
  0b00000010,
  0b01111111,
  0b01111111,
  0b00000000,
  0b00000000,

  0b01100010, // 2
  0b01110011,
  0b01111011,
  0b01101011,
  0b01101111,
  0b01100110,

  0b00100010, // 3
  0b01101011,
  0b01101011,
  0b01101011,
  0b01111111,
  0b00111110,

  0b00000111, // 4
  0b00001111,
  0b00001100,
  0b00001100,
  0b01111111,
  0b01111111,


  0b01101111, // 5
  0b01101111,
  0b01101011,
  0b01101011,
  0b01111011,
  0b00110011,

  0b00111110, // 6
  0b01111111,
  0b01101011,
  0b01101011,
  0b01111011,
  0b00110010,
  
  0b00000011, // 7
  0b00000011,
  0b01111011,
  0b01111111,
  0b00000111,
  0b00000011,
  
  0b00110110, // 8
  0b01111111,
  0b01101011,
  0b01101011,
  0b01111111,
  0b00110110,
  
  0b00100110, // 9
  0b01101111,
  0b01101011,
  0b01101011,
  0b01111111,
  0b00111110,
  
  0x00, 0x36, 0x36, 0x00, 0x00,0x00,// :
  0x00, 0x56, 0x36, 0x00, 0x00,0x00,// ;
  0x00, 0x08, 0x14, 0x22, 0x41,0x00,// <
  0x14, 0x14, 0x14, 0x14, 0x14,0x00,// =
  0x41, 0x22, 0x14, 0x08, 0x00,0x00,// >
  0x02, 0x01, 0x51, 0x09, 0x06,0x00,// ?
  0x32, 0x49, 0x79, 0x41, 0x3E,0x00,// @
  0x7E, 0x11, 0x11, 0x11, 0x7E,0x00,// A
  0x7F, 0x49, 0x49, 0x49, 0x36,0x00,// B
  0x3E, 0x41, 0x41, 0x41, 0x22,0x00,// C
  0x7F, 0x41, 0x41, 0x22, 0x1C,0x00,// D
  0x7F, 0x49, 0x49, 0x49, 0x41,0x00,// E
  0x7F, 0x09, 0x09, 0x01, 0x01,0x00,// F
  0x3E, 0x41, 0x41, 0x51, 0x32,0x00,// G
  0x7F, 0x08, 0x08, 0x08, 0x7F,0x00,// H
  0x00, 0x41, 0x7F, 0x41, 0x00,0x00,// I
  0x20, 0x40, 0x41, 0x3F, 0x01,0x00,// J
  0x7F, 0x08, 0x14, 0x22, 0x41,0x00,// K
  0x7F, 0x40, 0x40, 0x40, 0x40,0x00,// L
  0x7F, 0x02, 0x04, 0x02, 0x7F,0x00,// M
  0x7F, 0x04, 0x08, 0x10, 0x7F,0x00,// N
  0x3E, 0x41, 0x41, 0x41, 0x3E,0x00,// O
  0x7F, 0x09, 0x09, 0x09, 0x06,0x00,// P
  0x3E, 0x41, 0x51, 0x21, 0x5E,0x00,// Q
  0x7F, 0x09, 0x19, 0x29, 0x46,0x00,// R
  0x46, 0x49, 0x49, 0x49, 0x31,0x00,// S
  0x01, 0x01, 0x7F, 0x01, 0x01,0x00,// T
  0x3F, 0x40, 0x40, 0x40, 0x3F,0x00,// U
  0x1F, 0x20, 0x40, 0x20, 0x1F,0x00,// V
  0x7F, 0x20, 0x18, 0x20, 0x7F,0x00,// W
  0x63, 0x14, 0x08, 0x14, 0x63,0x00,// X
  0x03, 0x04, 0x78, 0x04, 0x03,0x00,// Y
  0x61, 0x51, 0x49, 0x45, 0x43,0x00,// Z
  0x00, 0x00, 0x7F, 0x41, 0x41,0x00,// [
  0x02, 0x04, 0x08, 0x10, 0x20,0x00,// "\"
  0x41, 0x41, 0x7F, 0x00, 0x00,0x00,// ]
  0x04, 0x02, 0x01, 0x02, 0x04,0x00,// ^
  0x40, 0x40, 0x40, 0x40, 0x40,0x00,// _
  0x00, 0x01, 0x02, 0x04, 0x00,0x00,// `
  0x20, 0x54, 0x54, 0x54, 0x78,0x00,// a
  0x7F, 0x48, 0x44, 0x44, 0x38,0x00,// b
  0x38, 0x44, 0x44, 0x44, 0x20,0x00,// c
  0x38, 0x44, 0x44, 0x48, 0x7F,0x00,// d
  0x38, 0x54, 0x54, 0x54, 0x18,0x00,// e
  0x08, 0x7E, 0x09, 0x01, 0x02,0x00,// f
  0x08, 0x14, 0x54, 0x54, 0x3C,0x00,// g
  0x7F, 0x08, 0x04, 0x04, 0x78,0x00,// h
  0x00, 0x44, 0x7D, 0x40, 0x00,0x00,// i
  0x20, 0x40, 0x44, 0x3D, 0x00,0x00,// j
  0x00, 0x7F, 0x10, 0x28, 0x44,0x00,// k
  0x00, 0x41, 0x7F, 0x40, 0x00,0x00,// l
  0x7C, 0x04, 0x18, 0x04, 0x78,0x00,// m
  0x7C, 0x08, 0x04, 0x04, 0x78,0x00,// n
  0x38, 0x44, 0x44, 0x44, 0x38,0x00,// o
  0x7C, 0x14, 0x14, 0x14, 0x08,0x00,// p
  0x08, 0x14, 0x14, 0x18, 0x7C,0x00,// q
  0x7C, 0x08, 0x04, 0x04, 0x08,0x00,// r
  0x48, 0x54, 0x54, 0x54, 0x20,0x00,// s
  0x04, 0x3F, 0x44, 0x40, 0x20,0x00,// t
  0x3C, 0x40, 0x40, 0x20, 0x7C,0x00,// u
  0x1C, 0x20, 0x40, 0x20, 0x1C,0x00,// v
  0x3C, 0x40, 0x30, 0x40, 0x3C,0x00,// w
  0x44, 0x28, 0x10, 0x28, 0x44,0x00,// x
  0x0C, 0x50, 0x50, 0x50, 0x3C,0x00,// y
  0x44, 0x64, 0x54, 0x4C, 0x44,0x00,// z
  0x00, 0x08, 0x36, 0x41, 0x00,0x00,// {
  0x00, 0x00, 0x7F, 0x00, 0x00,0x00,// |
  0x00, 0x41, 0x36, 0x08, 0x00,0x00,// }
  0x08, 0x08, 0x2A, 0x1C, 0x08,0x00,// ->
  0x08, 0x1C, 0x2A, 0x08, 0x08, 0x00// <-
    
};


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------


// THIS IS CLASS p_panel, for freqenz and OBERGRÄNSAD IKEA Panels 
int lut[16][16] = {
        {23, 22, 21, 20, 19, 18, 17, 16, 7, 6, 5, 4, 3, 2, 1, 0},
        {24, 25, 26, 27, 28, 29, 30, 31, 8, 9, 10, 11, 12, 13, 14, 15},
        {39, 38, 37, 36, 35, 34, 33, 32, 55, 54, 53, 52, 51, 50, 49, 48},
        {40, 41, 42, 43, 44, 45, 46, 47, 56, 57, 58, 59, 60, 61, 62, 63},
        {87, 86, 85, 84, 83, 82, 81, 80, 71, 70, 69, 68, 67, 66, 65, 64},
        {88, 89, 90, 91, 92, 93, 94, 95, 72, 73, 74, 75, 76, 77, 78, 79},
        {103, 102, 101, 100, 99, 98, 97, 96, 119, 118, 117, 116, 115, 114, 113, 112},
        {104, 105, 106, 107, 108, 109, 110, 111, 120, 121, 122, 123, 124, 125, 126, 127},
        {151, 150, 149, 148, 147, 146, 145, 144, 135, 134, 133, 132, 131, 130, 129, 128},
        {152, 153, 154, 155, 156, 157, 158, 159, 136, 137, 138, 139, 140, 141, 142, 143},
        {167, 166, 165, 164, 163, 162, 161, 160, 183, 182, 181, 180, 179, 178, 177, 176},
        {168, 169, 170, 171, 172, 173, 174, 175, 184, 185, 186, 187, 188, 189, 190, 191},
        {215, 214, 213, 212, 211, 210, 209, 208, 199, 198, 197, 196, 195, 194, 193, 192},
        {216, 217, 218, 219, 220, 221, 222, 223, 200, 201, 202, 203, 204, 205, 206, 207},
        {231, 230, 229, 228, 227, 226, 225, 224, 247, 246, 245, 244, 243, 242, 241, 240},
        {232, 233, 234, 235, 236, 237, 238, 239, 248, 249, 250, 251, 252, 253, 254, 255}};

 unsigned short _pLatch;
 unsigned short _pClock;
 unsigned short _pData;
 uint8_t p_buf[16*16];
 
void p_init(int p_latch, int p_clock, int p_data)//, int bitDepth, int numPanels)
{
 
    _pLatch = p_latch;
    _pClock = p_clock;
    _pData = p_data;
    pinMode(_pLatch, OUTPUT);
    pinMode(_pClock, OUTPUT);
    pinMode(_pData, OUTPUT);
 
  }


// Clear the Panel Buffer
void p_clear()
{
    for (int i = 0; i < 256; i++)  {
        p_buf[i] = 0;
    }
}

// SCAN DISPLAY, output Bytes to Serial
void p_scan()
{
    
    digitalWrite(P_EN,255);    
    uint8_t w=0;
    uint8_t w2=0;
     for (int i = 0; i< 256 ; i++)  {
        digitalWrite(_pData, 1 & p_buf[w++]); // make count faster
        digitalWrite(_pClock, HIGH);
        delayMicroseconds(TT);
        digitalWrite(_pClock, LOW);
        w2++;
        if(w2 >15) { 
            w2=0;    

            digitalWrite(_pLatch, HIGH);
            delayMicroseconds(TT);
            digitalWrite(_pLatch, LOW);
         }
      }  
    digitalWrite(P_EN,0);    
}

// SET a  Pixel in Panel Buffer, calculate position from LUT
void p_drawPixel(int8_t x, int8_t y, uint8_t color)
{ 
    p_buf[lut[y][x]] = color;
}


// Draw a Char Glyph into Panel Buffer
void p_printChar(uint8_t xs, uint8_t ys, char ch) {
    uint8_t d;
 
    for (uint8_t x=0;x<6;x++) { 
      // get Data from Memory
      d = pgm_read_byte_near((ch-32)*6+x +System6x7);
     // check which pixel to draw
     if ((d&1)  == 1)  p_drawPixel(x+xs, 0+ys, 0xff); else p_drawPixel(x+xs, 0+ys, 0x00);
     if ((d&2)  == 2)  p_drawPixel(x+xs, 1+ys, 0xff); else p_drawPixel(x+xs, 1+ys, 0x00);
     if ((d&4)  == 4)  p_drawPixel(x+xs, 2+ys, 0xff); else p_drawPixel(x+xs, 2+ys, 0x00);
     if ((d&8)  == 8)  p_drawPixel(x+xs, 3+ys, 0xff); else p_drawPixel(x+xs, 3+ys, 0x00);
     if ((d&16) == 16) p_drawPixel(x+xs, 4+ys, 0xff); else p_drawPixel(x+xs, 4+ys, 0x00);
     if ((d&32) == 32) p_drawPixel(x+xs, 5+ys, 0xff); else p_drawPixel(x+xs, 5+ys, 0x00);
     if ((d&64) == 64) p_drawPixel(x+xs, 6+ys, 0xff); else p_drawPixel(x+xs, 6+ys, 0x00);
   
    }
 }  
  
// --------------------------------------------------------------------------
// ------------------------ NTP ---------------------------------------------
// --------------------------------------------------------------------------



const char* getTimeString(void) {
  static char   acTimeString[32];
  time_t now = time(nullptr);
  ctime_r(&now, acTimeString);
  size_t    stLength;
  while (((stLength = strlen(acTimeString))) &&
         ('\n' == acTimeString[stLength - 1])) {
    acTimeString[stLength - 1] = 0; // Remove trailing line break...
  }
  return acTimeString;
}



 
 void set_clock(void) {
 
  //configTime(((MY_TIMEZ) * 3600), (DST_OFFSET * 3600), "pool.ntp.org", "time.nist.gov", "time.windows.com");
  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

 

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);   // Secs since 01.01.1970 (when uninitialized starts with (8 * 3600 = 28800)
  while (now < 8 * 3600 * 2) {  // Wait for realistic value
 
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  Serial.printf("Current time: %s\n", getTimeString());
}


void set_clock_from_tm() {
            time(&now);                       // read the current time
            localtime_r(&now, &tm);           // update the structure tm with the current time  
          
          
           // update time from struct
           minute = tm.tm_min;   
           hour   = tm.tm_hour;  
 
         
}



// --------------------------------------------------------------------------
// ----------------------SETUP ----------------------------------------------
// --------------------------------------------------------------------------




void setup() {

//  vars_init();
pinMode(P_EN, OUTPUT);
  pinMode(P_CLK, OUTPUT);
  pinMode(P_CLA, OUTPUT);
  pinMode(P_DI, OUTPUT);
  
  p_init(P_CLA, P_CLK, P_DI);
  
  Serial.begin(74880);

  
  
  pinMode(P_KEY, INPUT_PULLUP);
   
  analogWrite(P_EN, brightness); // full brightness
  p_clear();
  p_scan();
  
  

// start network
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
   //if you get here you have connected to the WiFi   
   Serial.println("Wifi connected!");
   // Sync clock
   set_clock();
   set_clock_from_tm() ;
   
}



// --------------------------------------------------------------------------
// ---------------------- LOOP ----------------------------------------------
// --------------------------------------------------------------------------

void loop() {

  // JEDE SEKUNDE
  if (millis()>mil+1000)
  {
    mil = millis();
    // PRINT THE TIME
 
    p_printChar(2,0,(hour/10) +48);
    p_printChar(9,0,(hour % 10) +48);
    p_printChar(2,9,(minute/10) +48);
    p_printChar(9,9,(minute%10) +48);
    p_scan(); // refreshes display

    // JEDE MINUTE
    sec ++;
    // every minute set the time
    if (sec>60) {
        sec = 0;
        set_clock_from_tm() ;
        set_clock();
      }
       Serial.printf("Current time: %s\n", getTimeString());
  }
  
  // TASTE
  if (digitalRead(P_KEY)==0) {
    brightness+=50; if(brightness>200) brightness =0;
    analogWrite(P_EN, brightness); // full brightness
    delay(500);
   }
  
 
}
