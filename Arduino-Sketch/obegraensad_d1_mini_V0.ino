// THIS FILE IS A DEMO FOR IKEA OBERGRANSÄD/Frequenz KIT WITH LDR 
 // AS INTERNET CLOCK
 // 1.11.23 increased accuracy by Martin
 // simplified by Pixelkoenig - (C)2023 DR. ARMIN ZINK
 // parts borrowed from
 // By /u/frumperino
 // goodwires.org

 // Waiting Time for Shift Cycle, can go downto 1 to speed up update of display.
#define TT 5

// FOR FREQUENZ     H_FREKVENS
// FOR OBERGRÄNSAD  H_OBEGRANSAD
#define H_OBEGRANSAD

#include <Arduino.h>
#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

 
// COLORS VALID FOR OBEGRÄNSAD ARICLE
#define P_EN D5  // ORANGE
#define P_DI D6  // YELLO
#define P_CLK D7 // BLUE
#define P_CLA D8 // LILA

#define P_KEY D4 // ROT
#define P_KEY_YELLOW D2 // YELLOW BUTTON

#define P_LDR  A0   


// SET YOUR TIMEZONE HERE
#define MY_NTP_SERVER "pool.ntp.org" // set the best fitting NTP server (pool) for your location
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

long     mil;
int      brightness=220;
uint8_t  sec;
uint8_t  minute;
uint8_t  hour;
time_t   now;                         // this is the epoch
tm       tm;



static const uint8_t System6x7[] PROGMEM = {
  
    // Fixed width; char width table not used !!!!
    // FIRST 32 Characters omitted
    
    // font data
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (space)
  0x00, 0x00, 0x4f, 0x4f, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 
  0x00, 0x0a, 0x3f, 0x0a, 0x3f, 0x0a, 
  0x24, 0x2a, 0x7f, 0x2a, 0x7f, 0x12, 
  0x23, 0x33, 0x18, 0x0c, 0x66, 0x62, 
  0x3e, 0x3f, 0x6d, 0x6a, 0x30, 0x48, 
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
  0x00, 0x1c, 0x3e, 0x63, 0x63, 0x00,
  0x00, 0x00, 0x63, 0x63, 0x3e, 0x1c, 
  0x2a, 0x1c, 0x7f, 0x7f, 0x1c, 0x2a, 
  0x08, 0x08, 0x3e, 0x3e, 0x08, 0x08, 
  0x00, 0x00, 0xc0, 0xe0, 0x60, 0x00, 
  0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 
  0x20, 0x30, 0x18, 0x0c, 0x06, 0x02, 
  0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x3e, // 0
  0x00, 0x02, 0x7f, 0x7f, 0x00, 0x00, // 1
  0x62, 0x73, 0x7b, 0x6b, 0x6f, 0x66, // 2
  0x22, 0x63, 0x6b, 0x6b, 0x7f, 0x36, // 3
  0x0f, 0x0f, 0x08, 0x08, 0x7f, 0x7f, // 4
  0x2f, 0x6f, 0x6b, 0x6b, 0x7b, 0x3b, // 5
  0x3e, 0x7f, 0x6b, 0x6b, 0x7b, 0x3a, // 6
  0x03, 0x03, 0x7b, 0x7b, 0x0f, 0x07, // 7
  0x36, 0x7f, 0x6b, 0x6b, 0x7f, 0x36, // 8
  0x26, 0x6f, 0x6b, 0x6b, 0x7f, 0x3e, // 9
  0x00, 0x00, 0x36, 0x36, 0x00, 0x00,
  0x00, 0x00, 0x76, 0x36, 0x00, 0x00, 
  0x00, 0x18, 0x3c, 0x66, 0x42, 0x00, 
  0x00, 0x00, 0x36, 0x36, 0x36, 0x36,
  0x00, 0x00, 0x42, 0x66, 0x3c, 0x18, 
  0x00, 0x02, 0x5b, 0x5b, 0x0f, 0x06, 
  0x3c, 0x42, 0x4a, 0x56, 0x5c, 0x00, 
  0x7e, 0x7f, 0x0b, 0x0b, 0x7f, 0x7e, 
  0x7f, 0x7f, 0x6b, 0x6b, 0x7f, 0x36, 
  0x3e, 0x7f, 0x77, 0x63, 0x63, 0x22,
  0x7f, 0x7f, 0x63, 0x63, 0x7f, 0x3e, 
  0x7f, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 
  0x7f, 0x7f, 0x0b, 0x0b, 0x03, 0x03, 
  0x3e, 0x7f, 0x63, 0x6b, 0x7b, 0x32, 
  0x7f, 0x7f, 0x08, 0x08, 0x7f, 0x7f, 
  0x00, 0x00, 0x7f, 0x7f, 0x00, 0x00, 
  0x00, 0x23, 0x63, 0x63, 0x7f, 0x3f, 
  0x7f, 0x7f, 0x1c, 0x36, 0x63, 0x41, 
  0x7f, 0x7f, 0x60, 0x60, 0x60, 0x60, 
  0x7f, 0x7f, 0x03, 0x06, 0x03, 0x7f, 
  0x7f, 0x7f, 0x0e, 0x18, 0x7f, 0x7f, 
  0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x3e, 
  0x7f, 0x7f, 0x0b, 0x0b, 0x0f, 0x06, 
  0x3e, 0x7f, 0x63, 0x73, 0x3f, 0x5e, 
  0x7f, 0x7f, 0x1b, 0x3b, 0x7f, 0x6e, 
  0x26, 0x6f, 0x6b, 0x6b, 0x7b, 0x32, 
  0x03, 0x03, 0x7f, 0x7f, 0x03, 0x03, 
  0x3f, 0x7f, 0x60, 0x60, 0x7f, 0x3f, 
  0x0f, 0x7f, 0x78, 0x40, 0x7f, 0x0f, 
  0x3f, 0x7f, 0x60, 0x78, 0x60, 0x3f, 
  0x63, 0x77, 0x1c, 0x1c, 0x77, 0x63, 
  0x07, 0x0f, 0x78, 0x78, 0x0f, 0x07, 
  0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 
  0x7d, 0x7e, 0x0b, 0x0b, 0x7e, 0x7d, 
  0x3d, 0x7e, 0x66, 0x66, 0x7e, 0x3d, 
  0x3d, 0x7d, 0x60, 0x60, 0x7d, 0x3d, 
  0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
  0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 
  0x7e, 0x7f, 0x0b, 0x0b, 0x7f, 0x7e, 
  0x7f, 0x7f, 0x6b, 0x6b, 0x7f, 0x36, 
  0x3e, 0x7f, 0x77, 0x63, 0x63, 0x22, 
  0x7f, 0x7f, 0x63, 0x63, 0x7f, 0x3e, 
  0x7f, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 
  0x7f, 0x7f, 0x0b, 0x0b, 0x03, 0x03, 
  0x3e, 0x7f, 0x63, 0x6b, 0x7b, 0x32, 
  0x7f, 0x7f, 0x08, 0x08, 0x7f, 0x7f, 
  0x00, 0x00, 0x7f, 0x7f, 0x00, 0x00, 
  0x00, 0x23, 0x63, 0x63, 0x7f, 0x3f, 
  0x7f, 0x7f, 0x1c, 0x36, 0x63, 0x41, 
  0x7f, 0x7f, 0x60, 0x60, 0x60, 0x60, 
  0x7f, 0x7f, 0x03, 0x06, 0x03, 0x7f, 
  0x7f, 0x7f, 0x0e, 0x18, 0x7f, 0x7f, 
  0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x3e, 
  0x7f, 0x7f, 0x0b, 0x0b, 0x0f, 0x06, 
  0x3e, 0x7f, 0x63, 0x73, 0x3f, 0x5e, 
  0x7f, 0x7f, 0x1b, 0x3b, 0x7f, 0x6e, 
  0x66, 0x6f, 0x6b, 0x6b, 0x7b, 0x32, 
  0x03, 0x03, 0x7f, 0x7f, 0x03, 0x03, 
  0x3f, 0x7f, 0x60, 0x60, 0x7f, 0x3f, 
  0x0f, 0x7f, 0x78, 0x40, 0x7f, 0x0f, 
  0x3f, 0x7f, 0x60, 0x78, 0x60, 0x3f, 
  0x63, 0x77, 0x1c, 0x1c, 0x77, 0x63, 
  0x07, 0x0f, 0x78, 0x78, 0x0f, 0x07, 
  0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 
  0x7d, 0x7e, 0x0b, 0x0b, 0x7e, 0x7d, 
  0x3d, 0x7e, 0x66, 0x66, 0x7e, 0x3d, 
  0x3d, 0x7d, 0x60, 0x60, 0x7d, 0x3d, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    
};


// LUT For OBEGRÄNSAD
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


// -----------------------------------------------------------
void p_init(int p_latch, int p_clock, int p_data)
{

    _pLatch = p_latch;
    _pClock = p_clock;
    _pData  = p_data;
    pinMode(_pLatch, OUTPUT);
    pinMode(_pClock, OUTPUT);
    pinMode(_pData,  OUTPUT);
 }


// -----------------------------------------------------------

// Clear the Panel Buffer
void p_clear(){
    for (int i = 0; i < 256; i++)  {
        p_buf[i] = 0;
    }
}


// -----------------------------------------------------------
// SCAN DISPLAY, output Bytes to Serial
void p_scan(uint8_t cmask){

 
    analogWrite(P_EN,255);
    delayMicroseconds(TT); 
 
    uint8_t w=0;
    uint8_t w2=0;
     for (int i = 256; i>0 ; i--)  {            
          digitalWrite(_pData, cmask & p_buf[w++]);  
          digitalWrite(_pClock, HIGH);               
          digitalWrite(_pClock, LOW);                
          w2++;
          if(w2 >15) { 
              w2=0;    
              digitalWrite(_pLatch, HIGH);           
              digitalWrite(_pLatch, LOW);            
           }
        }  
       analogWrite(P_EN,brightness);  // re enable brightmess
       
}


 // -----------------------------------------------------------

void p_drawPixel(int8_t x, int8_t y, uint8_t color)
{ 
  if((x<16) && (y<16)) {
#ifdef H_OBEGRANSAD
     p_buf[lut[y][x]] = color;
#endif
#ifdef  H_FREKVENS
    if (x > 7) { y += 0x10; x &= 0x07; }
    p_buf[(y*8+x)] = color ;
#endif
  }
}

// -----------------------------------------------------------
void p_fillScreen(uint8_t col){
  for (uint8_t x=0;x < 16; x++)
     for (uint8_t y=0;y < 16; y++)
       p_drawPixel(x, y, col);
}

// -----------------------------------------------------------
void test_display() {

    for (int i = 0; i<2; i++) {          
        p_fillScreen(0xff);  p_scan(1);  analogWrite(P_EN,250); delay(300);
        p_fillScreen(0x00);  p_scan(1);  analogWrite(P_EN,250); delay(300);
        
    }
}


// -----------------------------------------------------------
void p_printChar(uint8_t xs, uint8_t ys, char ch) {
    uint8_t d;
 
    for (uint8_t x=0;x<6;x++) { 
   
        d = pgm_read_byte_near((ch-32)*6+  // Buchstabennummer (ASCII ) minus 32 da die ersten 32 Zeichen nicht im Font sind
                               x +         // jede Spalte
                               System6x7); // Adrress of Font
   
     if ((d&1)    ==  1)  p_drawPixel(x+xs, 0+ys, 0xFF);else p_drawPixel(x+xs, 0+ys, 0);
     if ((d&2)    ==  2)  p_drawPixel(x+xs, 1+ys, 0xFF);else p_drawPixel(x+xs, 1+ys, 0);
     if ((d&4)    ==  4)  p_drawPixel(x+xs, 2+ys, 0xFF);else p_drawPixel(x+xs, 2+ys, 0);
     if ((d&8)    ==  8)  p_drawPixel(x+xs, 3+ys, 0xFF);else p_drawPixel(x+xs, 3+ys, 0);
     if ((d&16 )  == 16)  p_drawPixel(x+xs, 4+ys, 0xFF);else p_drawPixel(x+xs, 4+ys, 0);
     if ((d&32 )  == 32)  p_drawPixel(x+xs, 5+ys, 0xFF);else p_drawPixel(x+xs, 5+ys, 0);
     if ((d&64 )  == 64)  p_drawPixel(x+xs, 6+ys, 0xFF);else p_drawPixel(x+xs, 6+ys, 0);
   
    }
 }  
 

// -----------------------------------------------------------
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
 

// -----------------------------------------------------------
 void set_clock(void) {   
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



// -----------------------------------------------------------
void set_clock_from_tm() {
    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time  

   // update time from struct
  // sec  = tm.tm_min; 
   minute = tm.tm_min;   
   hour   = tm.tm_hour;   
}



// -----------------------------------------------------------
void setup() {

  pinMode(P_LDR,INPUT);
  pinMode(P_KEY, INPUT_PULLUP);  // RED KEY
  pinMode(P_EN, OUTPUT);         // Pseudo Analog out for FM Brightess
  analogWrite(P_EN, brightness); // adjust brightness

  p_init(P_CLA, P_CLK, P_DI);    // init Display

  test_display();
  Serial.begin(74880);           // Native Baud Rate of ESP
   
  p_clear();
  p_printChar(2,0,'A');          // Print "AP" while waiting for WiFi Manager
  p_printChar(9,0,'P');
  p_scan(1);
  delay(1000);
  
  WiFiManager wm;
  wm.setMinimumSignalQuality(50);
  bool res;
#ifdef H_FREKVENS
  res = wm.autoConnect("Y-CLOCK");  
#endif
#ifdef H_OBEGRANSAD
  res = wm.autoConnect("X-CLOCK");  
#endif

/*
// enable fo use without wifi manager
// start network
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
*/
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   p_printChar(2,9,'O');
   p_printChar(9,9,'K');
   p_scan(1);
   delay(500);
   
   //if you get here you have connected to the WiFi   
   Serial.println("Wifi connected!");
   // Sync clock
   set_clock();
   set_clock_from_tm() ;
   print_time();
   
}


// -----------------------------------------------------------
void print_time() {
        p_clear();
        p_printChar(2,0,(hour / 10)   +48);
        p_printChar(9,0,(hour % 10)   +48);
        p_printChar(2,9,(minute / 10) +48);
        p_printChar(9,9,(minute % 10) +48);
        p_scan(1); // refreshes display
  
}
// -----------------------------------------------------------
/// MAIN LOOP

void loop() {

  // JEDE SEKUNDE
  if (millis()>mil+1000)
  {
    mil = millis();
    sec ++;
    
    // Jede Minute
    if (sec>60) {
        sec = 0;
        print_time();
        
        set_clock_from_tm() ;
        set_clock();  

        brightness = map(analogRead(P_LDR), 1024, 0 , 240,254);
        Serial.print("Brightness:");Serial.println(brightness);
        analogWrite(P_EN, brightness); 
      }
   }
 
}