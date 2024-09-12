///////////////////////////////////////////////////////////////////////////////
/*
WIRELESS HOME SECURITY USING NRF24L01

Developed by:
- Daniel George Ashok 
- Parthiv Raj
- Rishab Prejesh

### Components Used and Pin Configuration:
- 5V: RTC, LCD_I2C
- 3.3V: NRF24L01
- PIN 2: Buzzer

### I2C Pins:
- SDA: RTC, LCD_I2C
- SCL: RTC, LCD_I2C

### SPI for nRF24L01:
- CE: Pin 9
- CSN: Pin 10
- MOSI: ICSP
- MISO: ICSP

### GSM Module (SIM800L):
- Serial communication on Digital Pin 4, 5
*/
///////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // For LCD display
#include <DS3231.h>              // For RTC (Real Time Clock)
#include <SoftwareSerial.h>      // For SIM800L GSM module
#include <SPI.h>                 // For nRF24L01 communication
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>

#define led_1 2                  // LED or Buzzer connected to Pin 2

// Initialize LCD at I2C address 0x3F with 16x2 display size
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Initialize RTC using I2C
DS3231 rtc(SDA, SCL);

// Buzzer pin
const int buzzer = 2; 

// Initialize Software Serial for GSM communication (SIM800L)
SoftwareSerial mySerial(0, 1);   // Tx, Rx for GSM (change these pins as per wiring)

// Initialize RF24 communication
RF24 radio(9, 10);               // CE, CSN
RF24Network network(radio);      // Include the radio in the network

// Node addresses (in Octal format)
const uint16_t this_node = 00;   // Receiver node address
const uint16_t node01 = 01;      // Transmitter node address

// Function Declarations
void lcd_time();
void locking_the_system();
void all_systems_are_good();
void messaging();
void buzzer_system_compromised();
void test_functions(); // For testing alarm and message features

void setup() {
  // Setup buzzer pin
  pinMode(buzzer, OUTPUT); 

  // GSM Module Initialization
  mySerial.begin(9600);          // Set baud rate for GSM Module 
  Serial.begin(9600);            // Set baud rate for Serial Monitor (Arduino)

  // NRF24L01 Initialization
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  // Start network on channel 90 with receiver node address
  radio.setDataRate(RF24_2MBPS); // Set data rate for communication

  // LCD Initialization
  lcd.begin();                   // Initialize the LCD
  lcd.backlight();                // Turn on the backlight
  lcd.clear();
  lcd.setCursor(4, 0);

  // RTC Initialization
  rtc.begin();                   // Initialize the RTC object
}

void loop() {
  // Display time on LCD
  lcd_time();
  delay(1000);
  lcd.clear();
  delay(100);
  
  // Display system status
  all_systems_are_good();

  // Network update
  network.update();

  // Check for incoming data
  while (network.available()) {
    RF24NetworkHeader header;
    unsigned long buttonState1;
    network.read(header, &buttonState1, sizeof(buttonState1));  // Read incoming data

    // If system is compromised (button pressed), trigger alarm and send SMS
    if (buttonState1 == HIGH) {  
      messaging();               // Send SMS alert
      buzzer_system_compromised(); // Sound the alarm
    } else {
      lcd_time();
      delay(1000);
      lcd.clear();
      delay(100);
      all_systems_are_good();    // Display system status
    }
  }
}

// Function to display time on the LCD using the RTC module
void lcd_time() {
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(rtc.getTimeStr());
  
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());
}

// Function to display "all systems good" message on the LCD
void all_systems_are_good() {
  lcd.setCursor(0, 0);
  lcd.print("All systems good");
 
  lcd.setCursor(0, 1);
  lcd.print("Hello sir!!");
}

// Function to send an SMS alert using the GSM module
void messaging() {
  mySerial.println("AT+CMGF=1");    // Set GSM module in text mode
  delay(1000);  
  mySerial.println("AT+CMGS=\"+919449539865\"\r");  // Replace with your phone number
  delay(1000);
  mySerial.println("YOUR SYSTEM HAS BEEN COMPROMISED");  // SMS text
  delay(100);
  mySerial.println((char)26);  // Send the message (CTRL+Z)
  delay(1000);
}

// Function to sound the buzzer when the system is compromised
void buzzer_system_compromised() {
  tone(buzzer, 1000);  // 1KHz sound for 1 second
  delay(1000);         
  noTone(buzzer);      // Stop sound for 1 second
  delay(1000);
}

// Function to test alarm and SMS functionality
void test_functions() {
  tone(buzzer, 1000);  
  delay(10);           
  noTone(buzzer);      
  delay(10);           
}
