/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Seed V2 Ethernet shield.
 If an LCD display is connected on:

 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
   * ends to +5V and ground
   * wiper to LCD VO pin (pin 3)
It will display the dhcp allocated IP address.
This can be set to a static IP by the "Use_Static_IP" flag and the Static IP
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 WAS originally 
 created 4/7/2016
 by Greg Stroot
 modified 4/7/2016
 by Greg Stroot
 
 */

#include <SPI.h>
#include <EthernetV2_0.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2,3,4,5,6,7);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,1,1,177);
//Declare an IP variable so we can display it
IPAddress MyIP(0,0,0,0);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);
#define W5200_CS  10
#define SDCARD_CS 4

boolean Use_Static_IP = false;

void setup() {

 // set up the LCD's number of columns and rows:
 lcd.begin(16, 2);
 
 // Open serial communications and wait for port to open:
 Serial.begin(9600);
 pinMode(SDCARD_CS,OUTPUT);
 digitalWrite(SDCARD_CS,HIGH);//Deselect the SD card
  // start the Ethernet connection and the server:
  if (Use_Static_IP)
  {
  Ethernet.begin(mac, ip);
  }
  else
  {
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  }
  //Start the server
  server.begin();
  MyIP = (Ethernet.localIP());
  lcd.setCursor(0,0);
  if(Use_Static_IP)
  {
    Serial.print("Static ");
    lcd.print("Static ");
  }
  else
  {
    Serial.print("Dynamic ");
    lcd.print("Dynamic ");
  }
  Serial.print("server IP is at ");
  Serial.println(MyIP);
  Serial.println(Ethernet.localIP());
 
  lcd.print("Svr IP: ");
  lcd.setCursor(0,1);
  lcd.print(MyIP);
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

