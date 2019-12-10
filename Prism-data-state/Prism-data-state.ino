#include <SPI.h>
#include <Ethernet.h>

const int IN7 = 7;
const int IN8 = 8;
const int IN9 = 9;
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
IPAddress ip(192, 168, 2, 94);
EthernetServer server(80);

String HTTP_req; // stores the HTTP request

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(IN7, INPUT_PULLUP); //set pin mode to input pullup
  pinMode(IN8, INPUT_PULLUP); //set pin mode to input pullup
  pinMode(IN9, INPUT_PULLUP); //set pin mode to input pullup
  //while (!Serial) {
  //; // wait for serial port to connect. Needed for native USB port only
  //}

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  HTTP_req = "";
}

void (*resetFunc)(void) = 0; // Soft reset function

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    .Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        if (HTTP_req.length() < 120)
          HTTP_req += c; // save the HTTP request 1 char at a time
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // the connection will be closed after completion of the response
          client.println();
          Serial.println(HTTP_req);
          client.print("<prism-data>"); // start of xml data
          client.print("");
          client.print("<Timestamp>");
          client.print(millis());
          client.print("</Timestamp>");
          client.println("");
          if (digitalRead(IN7) == 0)
          {
            client.println("<State>A</State>"); // State A when pin 7 is connected to ground
          }
          else if (digitalRead(IN8) == 0)
          {
            client.println("<State>B</State>"); // State B when pin 8 is connected to ground
          }
          else if (digitalRead(IN9) == 0)
          {
            client.println("<State>ERROR</State>"); // Error when pin 9 is connected to ground
          }
          else
          {
            client.println("<State>NO CONNECTION</State>"); // No connection when neither of pins 7,8,9 are connected to ground
          }
          client.print("</prism-data>"); // end of xml data
          break;
        }

        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    HTTP_req = "";
    Serial.println("client disconnected");
  }
  if (millis() / 60000 >= 1440)
  {
    resetFunc(); // soft reset arduino after 24 hrs of uptime
  }
}
