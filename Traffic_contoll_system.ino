#define BLYNK_PRINT Serial //This is Used to know the status of blynk connections if it is connected or not
#include <ESP8266_Lib.h>   //This is the libraries of wifi module that is ESP8266 (*ESP8266 is wifi module name*)
#include <BlynkSimpleShieldEsp8266.h>//This is the library for ESP8266

char auth[] = "e19feff7af3f4c4d967b1a1364db8d2a";      //Your Blynk AUTH code
char ssid[] = "Redmi";                            //wifi connection name for ESP8266
char pass[] = "villian1";                            //wifi connection password for ESP8266

#define EspSerial Serial1   //There are multiple com ports in arduino mega like rx,tx and rx1,tx1 also rx2,tx2. We have chosen rx1, tx1 that is serial1
#define ESP8266_BAUD 115200 // ESP8266 can send 115200 symbols/sec that is taken as baud rate

ESP8266 wifi(&EspSerial);

#include <SPI.h>    //Serial Pheriperal interface
#include <MFRC522.h>  //This is the library for RFID reader (*MFRC522 is RFID reader module name*)

#define RST_PIN         5          //Reset pin for rfid is assigned to pin 5 of arduino mega
#define SS_1_PIN        48         //the pin on each device that can use to enable and disable specific devices of rfid 1
#define SS_2_PIN        53          //the pin on each device that can use to enable and disable specific devices of rfid 2

#define NR_OF_READERS   2   //The total number of readers are 2

byte ssPins[] = {SS_1_PIN, SS_2_PIN}; //take both ss1 and ss2 pins as - ss pins of rfid

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

//Pin Configaration For lane A of traffic lights
int ledA1 = 2;  //Green Light
int ledA2 = 3;  //Amber Light
int ledA3 = 4;  //Red Light

//Pin Configaration For lane B of traffic lights
int ledB1 = 6;  //Green Light
int ledB2 = 7;  //Amber Light
int ledB3 = 8;  //Red Light

//Pin configuration for IR sensors
int ir1 = 22; //IR sensor is connected to pin 22
int ir2 = 23; //IR sensor is connected to pin 23
int ir3 = 24; //IR sensor is connected to pin 24
int ir4 = 25; //IR sensor is connected to pin 25

//variables for sensor inputs
int a1, a2, b1, b2; // Data from the Sensors is stored in the respective variable (*data is either 1 or 0 also HIGH or LOW respectively*)

//For Priority Vehicles
int p = 0;

//For Starting System
int s = 0;

void setup()
{
  //Setting up communication for debuging
  Serial.begin(9600); //9600 is the baud rate for communication

  EspSerial.begin(ESP8266_BAUD); // Initialise wifi module with baud rate
  delay(10);  // taking delay for 10 milliseconds

  Blynk.begin(auth, wifi, ssid, pass); // Blynk is connected to auth code through wifi with the support of given wifi name and password

  pinMode(ledA1, OUTPUT); // pins taken as output
  pinMode(ledA2, OUTPUT);// pins taken as output
  pinMode(ledA3, OUTPUT);// pins taken as output

  pinMode(ledB1, OUTPUT);// pins taken as output
  pinMode(ledB2, OUTPUT);// pins taken as output
  pinMode(ledB3, OUTPUT);// pins taken as output

  pinMode(ir1, INPUT);// pins taken as input
  pinMode(ir2, INPUT);// pins taken as input
  pinMode(ir3, INPUT);// pins taken as input
  pinMode(ir4, INPUT);// pins taken as input

  SPI.begin();        // Init or start  SPI bus
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) //initialise each rfid reader at a time
  {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
}

//sub program for turning red to green light
void roadAopen()
{
  digitalWrite(ledA3, LOW);   //Road A Red light off
  delay(200);
  digitalWrite(ledA2, HIGH);  //Road A Amber light on
  delay(200);
  digitalWrite(ledA2, LOW);   //Road A Amber light off
  digitalWrite(ledA1, HIGH);  //Road A Green light on
  digitalWrite(ledB3, HIGH);  //Road B Red light on
  delay(10000);
  digitalWrite(ledA1, LOW);   //Road A Green light off
  digitalWrite(ledA2, HIGH);  //Road A Amber light on
  delay(1000);
  digitalWrite(ledA2, LOW);   //Road A Amber light off
}

//sub program for turning red to green light
void roadBopen()
{
  digitalWrite(ledB3, LOW);   //Road B Red light off
  delay(200);
  digitalWrite(ledB2, HIGH);  //Road B Amber light on
  delay(200);
  digitalWrite(ledB2, LOW);   //Road B Amber light off
  digitalWrite(ledB1, HIGH);  //Road B Green light on
  digitalWrite(ledA3, HIGH);  //Road A Red light on
  delay(10000);
  digitalWrite(ledB1, LOW);   //Road B Green light off
  digitalWrite(ledB2, HIGH);  //Road B Amber light on
  delay(1000);
  digitalWrite(ledB2, LOW);   //Road B Amber light off
}

//sub program for Turning on red lights in both streets
void roadclose()
{
  digitalWrite(ledA3, HIGH);  //Road A Red light on
  digitalWrite(ledB3, HIGH);  //Road B Red light on
  digitalWrite(ledA2, LOW);   //Road A Amber light off
  digitalWrite(ledB2, LOW);   //Road B Amber light off
  digitalWrite(ledA1, LOW);   //Road A Green light off
  digitalWrite(ledB1, LOW);   //Road B Green light off

}

//sub program for Turning off the entire leds
void systemoff()
{
  digitalWrite(ledA3, LOW);   //Road A Red light off
  digitalWrite(ledB3, LOW);   //Road B Red light off
  digitalWrite(ledA2, LOW);   //Road A Amber light off
  digitalWrite(ledB2, LOW);   //Road B Amber light off
  digitalWrite(ledA1, LOW);   //Road A Green light off
  digitalWrite(ledB1, LOW);   //Road B Green light off
}

//Sub program for rfid reader
void rfid()
{
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())// Look for new cards, if present specify which reader either 0 or 1
    {
      if (reader == 0) // If rfid reader 1 activates
      {
        p = 1; // set p=1
        Blynk.notify("Ambulance in Road A");  //Send notification to Blynk app
        Serial.println("Ambulance in Road A");// Printing in serial monitor
        roadAopen();
        Blynk.notify("Ambulance Passed");//Send notification to Blynk app
        Serial.println("Ambulance Passed");// Printing in serial monitor
        p = 0;
      }
      else if (reader == 1)// If rfid reader 2 activates
      {
        p = 2;
        Blynk.notify("Ambulance in Road B");//Send notification to Blynk app
        Serial.println("Ambulance in Road B");// Printing in serial monitor
        roadBopen();
        Blynk.notify("Ambulance Passed");//Send notification to Blynk app
        Serial.println("Ambulance Passed");// Printing in serial monitor
        p = 0;
      }

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }
  }
}

WidgetTerminal terminal(V1); // It is the widget to send the data on or off through blynk app 

BLYNK_WRITE(V1)
{
  if (String("on") == param.asStr()) // Getting data from blynk app, if the data received is on then the entire system turns on
  {
    s = 1;
    Blynk.virtualWrite(V0, "SYSTEM ON"); // Printing on LCD display on Blynk app
    Serial.println("System on");// Printing in serial monitor
  }
  else if (String("off") == param.asStr())// Getting data from blynk app, if the data received is on then the entire system turns off
  {
    s = 2;
    Blynk.virtualWrite(V0, "SYSTEM OFF");// Printing on LCD display on Blynk app
    Serial.println("System off");// Printing in serial monitor
  }
}

//To continuously repeat the process
void loop()
{
  Blynk.run(); // Setting continuous connection to blynk app

  a1 = digitalRead(ir1); //Reading values from sensor and storing them in the variable
  a2 = digitalRead(ir2);//Reading values from sensor and storing them in the variable
  b1 = digitalRead(ir3);//Reading values from sensor and storing them in the variable
  b2 = digitalRead(ir4);//Reading values from sensor and storing them in the variable

  roadclose(); //Calling sub program

  if (s == 1) // if s is equal to 1 then turn on the entire system
  {
    rfid(); // calling sub program of rfid
    if (p == 0) //IF there is no ambulance in the streets then
    {
      if (a1 == LOW || b1 == LOW) // Viewing that is there any vehicles is present 
      {
        if (a2 == LOW) // If vehicle is present and if it reached the maximum number of vehicles then turn lane A traffic lights to green
        {
          Blynk.notify("Road A Green"); //Sending notification to Blynk app
          Serial.println("Road A Green");
          roadAopen();                   //Calling Sub program
          Blynk.notify("Road A clear");//Sending notification to Blynk app
          Serial.println("Road A clear");
          a2 = HIGH;
        }
        else if (b2 == LOW)// If vehicle is present and if it reached the maximum number of vehicles then turn lane B traffic lights to green
        {
          Blynk.notify("Road B Green");//Sending notification to Blynk app
          Serial.println("Road B Green");
          roadBopen();                  //Calling Sub program
          Blynk.notify("Road B Clear");//Sending notification to Blynk app
          Serial.println("Road B clear");
          a2 = HIGH;
        }
      }
      else //If there are no vehicles present in the streets
      {
        roadclose(); //calling Sub program to dispplay red light on both streets
      }
    }
  }
  else if (s == 2) // If the data received is off then turn the entire system off
  {
    systemoff(); //Calling sub program to tuen the entire system off
  }
}
