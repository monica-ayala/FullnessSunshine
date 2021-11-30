#include <ESP8266WiFi.h>
#include <SPI.h>
#include "Ubidots.h"

#define DEVICE  "esp8266principal"  // Put here your Ubidots device label
#define ID_TEMP "61a2f8666ae377000a846b1d" //Izq INF
#define ID_TEMP1 "61a2f8875c1d77000de950f0" //Izq SUP
#define ID_TEMP2 "61a31f9d7409ed000def7588" // Der SUP
#define ID_TEMP3 "61a31f95be350a000b84d3e2" //Der INF
#define ID_TEMP4 "61a697cc58f48c0f34efc645" // ANGULO SABAJO
#define ID_TEMP5 "61a697c6b56955049e2685ba" //ANGULO SARRIBA
#define TOKEN  "BBFF-Eap0qzwSjag0IGLPmhXQLPdG4WL20n"  // Put here your Ubidots TOKEN
#define CS 15 //define chip select line for manual control
#define WIFISSID "Node" // Put here your Wi-Fi SSID
#define PASSWORD "marrero1234" // Put here your Wi-Fi password


Ubidots client(TOKEN);
String str_data;
int intvect[6];

void setup() {
    Serial.begin(115200); 
    -
    //client.wifiConnection(WIFISSID, PASSWORD);
    
    client.wifiConnect(WIFISSID, PASSWORD);
    pinMode(CS, OUTPUT);                                       //configure the line as output
                                       //Set UART baud rate
    digitalWrite(CS, HIGH);                                    //Initialize the CS line to HIGH
    SPI.begin();                                               //Initialize the SPI module--> configures the MOSI, MISO and CLOCK lines
 
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
}

void loop() 
{

   
    //  delay(5000);

    char buf [20];

  // enable Slave Select
  digitalWrite(CS, LOW);    
  SPI.transfer (1);   // initiate transmission
  for (int pos = 0; pos < sizeof (buf) - 1; pos++)
    {
    delayMicroseconds (15);
    buf [pos] = SPI.transfer (0);
    if (buf [pos] == 0)
      {
      break;
      }
    }

  buf [sizeof (buf) - 1] = 0;  // ensure terminating null

  // disable Slave Select
  digitalWrite(CS, HIGH);

  Serial.print ("We received: ");
  Serial.println (buf);
 int j = 0;
  for(int i=0;i<20;i++){
    str_data += buf[i];
    if(buf[i]==','){
      intvect[j]=str_data.toInt();
      j++;
      str_data="";
    }
  }

  delay (1000);  // 1 second delay 

   digitalWrite(CS, LOW);    
  SPI.transfer (2);   // initiate transmission
  digitalWrite(CS, HIGH);
      // 1 second delay 
//UBIDOTS

    Serial.println(intvect[0]);
    client.add(ID_TEMP, intvect[0]); 
    client.add(ID_TEMP1, intvect[1]);
    client.add(ID_TEMP2, intvect[2]);
    client.add(ID_TEMP3, intvect[3]);
    client.add(ID_TEMP4, intvect[4]);
    client.add(ID_TEMP5, intvect[5]);
    client.send();
    delay (1000);
}
