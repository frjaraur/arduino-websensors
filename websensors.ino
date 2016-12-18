#include <Dhcp.h>
#include <EthernetClient.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <EthernetServer.h>
#include <Ethernet.h>
#include <SPI.h>
#include <DHT11.h>

byte mac[] = {0xA0,0xAA,0xBB,0xCC,0xDE,0x02};

EthernetClient client;

EthernetServer server(80);  

int webserverport=80;

int webpagerefresh=15; //15 seconds

int pin=2;
DHT11 dht11(pin);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  
  while (!Serial){
      }
     if (Ethernet.begin(mac) == 0) //Si devuelve error
     {       
      Serial.println("No ha sido posible configurar Ethernet por DHCP");
      while (true); // No sigas,....
     }
     
  Serial.print("IP Address: ");
  for (byte B = 0; B < 4; B++)   
     {
         Serial.print(Ethernet.localIP()[B], DEC);
         Serial.print(".");
      }   
  Serial.println();
  
  Serial.print("Default Router: ");
  for (byte B = 0; B < 4; B++) {
    Serial.print(Ethernet.gatewayIP()[B], DEC);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Netmask: ");
  for (byte B = 0; B < 4; B++) {
  Serial.print(Ethernet.subnetMask()[B], DEC);
  Serial.print(".");
  }
  Serial.println();
  
  Serial.print("DNS: ");
  for (byte B = 0; B < 4; B++) {
    Serial.print(Ethernet.dnsServerIP()[B], DEC);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("MAC Address: ");
  for (byte B = 0; B < 4; B++) {
    Serial.print(mac[B],HEX);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Starting Web Server on ");
  Serial.print(Ethernet.localIP());
  Serial.print(" Port ");
  Serial.print(webserverport);
  Serial.println();
  server.begin();
//  Serial.println(Ethernet.localIP());

  
}

void loop() {

  EthernetClient client = server.available();

  //Clean Serial
//  Serial.write(27);       // ESC command
//  Serial.print("[2J");    // clear screen command
//  Serial.write(27);
//  Serial.print("[H");     // cursor to home command

  int err;
  float temp, hum;    
  
  if((err = dht11.read(hum, temp)) != 0){
    Serial.println();
    Serial.print("Error Num :");
    Serial.print(err);
    Serial.println();
    return;
  }
  delay(1000);


  if (client){    
    boolean currentLineIsBlank = true;  
    while (client.connected()){ 
      if (client.available()){  
        char c = client.read();
        Serial.write(c); 
        if(c=='\n' && currentLineIsBlank){   
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.print("Refresh: ");
          client.print(webpagerefresh);
          client.println();
          client.println();          
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          Serial.print("Temperature: ");
          Serial.print(temp);
          Serial.println();
          Serial.print("Humidity: ");       
          Serial.println(hum);
          Serial.println();
          client.print("<head><title>Sensors on ");
          client.print(Ethernet.localIP());
          client.print("</title></head>");
          client.print("<body><h1>Sensors on ");
          client.print(Ethernet.localIP());         
          client.print("</h1><p>Temperature - ");
          client.print(temp);
          client.print(" Celsius</p>");
          client.print("<p>Humidity:  ");
          client.print(hum); // Aqui va la humedad
          client.print(" %</p>");
          client.print("<p><em>Page will be updated every ");
          client.print(webpagerefresh);           
          client.print(" seconds</em></p></body></html>");
          client.println();
          break;
        }
           if (c=='\n')
              currentLineIsBlank = true;       
               else if (c != '\r')
                currentLineIsBlank = false;
          }
        }
     delay(10); 
     client.stop();  
     Serial.println("cliente desconectado");
   }
              
}
