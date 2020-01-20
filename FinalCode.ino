
//using RB-FEE-24 IRTEMP

///////////////////////////////////////////PINS////////////////////////////////////////
//Pin 2
byte dataIn = 2; 

//Pin 3
byte clk = 3;

//Pin 4
byte acquire = 4;
///////////////////////////////////////////PINS////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//Pin outputs for the Display
#define sclk 8
#define mosi 9
#define dc   10
#define cs   11
#define rst  12

//Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


//Libraries included
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_SSD1351.h>
#include <SoftwareSerial.h>
//////////////////////////////////////////////////////////////////////////////////////

////// Class Variables/////////////////////////////////
// Address for Data pin
byte dataAddress = 0x4C; 

// Create an Instance of the OLED Display
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);

// Uses Bluetooth, 0 for RX, 1 for TX
SoftwareSerial bluetooth(0,1);

uint16_t Color = 0x0000;
///////////////////////////////////////////////////////



// Initializations
void setup() 
{    
    Serial.begin(9600);
    InitLCD();  
    InitSensor();
    InitBluetooth();  
    //Serial.println("Initializing...");  
 }
  
void InitLCD()
{
  // Initialize Display
  tft.begin();

  // "Clear" Screen by drawing the whole screen with black
  tft.fillScreen(BLACK);
  
  // Display constant labels
  DisplayText("Infrared ", WHITE);  
  DisplayText("Thermo", WHITE); 
  
  //Serial.println("OLED Initialized");  
}

void InitSensor()
{
  // If the RB-FEE-24 is not set to low power mode
  // Keep acquire as the Output on all the time
  if(acquire != -1)
  {
  pinMode(acquire, OUTPUT);
  digitalWrite(acquire, HIGH);  
  }
  // Enable Inputs 
  // Pin to send data to the Arduino
  pinMode(dataIn, INPUT);
  // Pin to send Clock to Arduino
  pinMode(clk, INPUT);

  digitalWrite(dataIn,HIGH);
  digitalWrite(clk,HIGH);

  // Turn on the sensor
  ToggleSensor(true);  
  
  //Serial.println("Sensor Initialized"); 
}
void InitBluetooth()
{
  bluetooth.begin(38400);
  
  //Serial.println("Bluetooth Initialized"); 
}
void loop()
{
  // Receive temperature reading
  float Temperature = GetTemp();
  
  // Each temperature reading only wipes out the temperature, not the labels
  tft.setCursor(10,50);    
  tft.fillRect(10,50,128,128,BLACK);

  // If the temp value is not a number, show error
  if(isnan(Temperature))
  {
     DisplayText("[ Failed ]", RED);
     return;
  }
  
  // Show temperature as different colors depending on temperature
  if(Temperature > 50)
  {
    Color = RED;
  }
  else if(Temperature < 0)
  {
   Color = BLUE;
  }
  else
  {
   Color = YELLOW;
  }

  // Add °C (Alt + 0176) to the end of the temperature
  String displayTemp = (String)Temperature + " C";
  // Display temperature with appropriate color
  DisplayText(displayTemp, Color);
  
  // Send Data to Bluetooth
  Serial.print(Temperature);
  Serial.print("\n");
  
  // Prevent Spam, delay program for 1 sec
  delay(1000);
}

float GetTemp()
{
  //Time elapsed
  long timeout = millis() + 2000;

  //temp value
  float actualTemp = 0;

  //Most and Least significant bit   
  int msb = 0;
  int lsb = 0;

  // Turn off sensor when processing
  ToggleSensor(false);

  while(true)
  {
    //will hold the 5 bytes of data
    byte data[5] = {0};

    //grab the first byte, then go through the bits starting at the end
    for(int dByte = 0; dByte < 5; dByte++)
     {
      for(int dBit = 7; dBit >= 0; dBit--)
      {
        //Clock idles on high, data changes on falling edge, sampling on rising edge
        while(digitalRead(clk) == HIGH && millis() < timeout)
        {
        }
        while(digitalRead(clk) == LOW && millis() < timeout)
        {
        }
        //sample were at the rising edge!
        if(digitalRead(dataIn))
        {
          data[dByte] |= 1 << dBit;
        }        
      }
    }
      //Toggle sensor true its been idling for too long
     if(millis() >= timeout)
     {
      ToggleSensor(true);
      return NAN;
     }
     //if the data at 0 is address for IRtemp
     if(data[0] == 0x4C)
     {
      ToggleSensor(true);
      
      msb = data[1] << 8;
      lsb = data[2];

      actualTemp = ((msb + lsb) / 16 - 273.15);
     }           
      delay(500);
      return actualTemp;
  }
}
  
// Method for displaying text
void DisplayText(String text,uint16_t color)
{
  tft.setTextSize(2); 
  tft.setTextColor(color);
  // Render onto OLED
  tft.println(text);
}

// Toggle Sensor State as On or Off
void ToggleSensor(bool state)
{
  if(acquire != -1)
  {
    digitalWrite(acquire, state);
  }
}






