#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

const char* ssid = "Bhruhathi:)";
const char* password = "05021312";

SoftwareSerial arduinoSerial(D1, D0); // NodeMCU pins D1 (RX) and D0 (TX)

String acData = "";
String acDataLog = ""; // Log of received data

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  arduinoSerial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  server.begin();
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            // HTML content with auto-refresh every 1 second
            client.println("<html>");
            client.println("<head><meta http-equiv='refresh' content='1'></head>");
            client.println("<body>");
            client.print(acDataLog); // Display the log of data
            client.println("</p></body>");
            client.println("</html>");
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }

  if (arduinoSerial.available()) {
    acData = arduinoSerial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(acData);
    
    acDataLog += acData + "<br>"; // Append received data to the log with HTML line break

    // Optional: Limit the log to the last 10 lines
    int lineCount = 0;
    for (int i = 0; i < acDataLog.length(); i++) {
      if (acDataLog[i] == '<') {
        lineCount++;
      }
    }
    if (lineCount > 10) {
      acDataLog = acDataLog.substring(acDataLog.indexOf("<br>") + 4);
    }
  }
}
