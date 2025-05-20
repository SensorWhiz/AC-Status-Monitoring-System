#include <SoftwareSerial.h>
#include <WinsonLib.h>

WCS WCS1 = WCS(0, _WCS1700);
double data = 0;

SoftwareSerial NSerial(6, 7); 

unsigned long startTime = 0;     // Time when the AC was turned on
unsigned long sessionTime = 0;   // Time the AC has been on in the current session
unsigned long totalTime = 0;     // Total time the AC has been on during the day
unsigned long dayStartMillis = 0; // Time when the day started (for 24-hour reset)

bool acIsOn = false;  

void setup() {
  NSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Reset");
  WCS1.Reset();
  
  dayStartMillis = millis();  // Initialize the day start time
}

void loop() {
  data = WCS1.A_AC();

  Serial.print("Current(A): ");
  Serial.println(data, 3);  // Debugging print for current reading

  unsigned long currentMillis = millis();

  // Check if 24 hours (86400000 ms) have passed since the day started
  if (currentMillis - dayStartMillis >= 86400000) {
    // Reset for a new day
    totalTime = 0;
    dayStartMillis = currentMillis; // Reset the day start time
    Serial.println("New day.");
  }

  if (data >= 0.500) {
    if (!acIsOn) {
      acIsOn = true;
      startTime = currentMillis;
      Serial.println("AC just turned ON");  
    }
    sessionTime = currentMillis - startTime;  // Update sessionTime while AC is on
    
    // Convert sessionTime and totalTime to hours, minutes, seconds
    unsigned long sessionSeconds = sessionTime / 1000;
    unsigned int sessionHours = sessionSeconds / 3600;
    unsigned int sessionMinutes = (sessionSeconds % 3600) / 60;
    unsigned int sessionRemainingSeconds = sessionSeconds % 60;

    unsigned long totalSeconds = totalTime / 1000;
    unsigned int totalHours = totalSeconds / 3600;
    unsigned int totalMinutes = (totalSeconds % 3600) / 60;
    unsigned int totalRemainingSeconds = totalSeconds % 60;

    // Create a string to send the data to the app
    String dataToSend = "Current(A): " + String(data, 3) + " A\n";
    dataToSend += "AC is ON\n";
    dataToSend += "AC ON Time Now - " + String(sessionHours) + ":" + String(sessionMinutes) + ":" + String(sessionRemainingSeconds) + "\n";
    dataToSend += "Total AC ON Time Today - " + String(totalHours) + ":" + String(totalMinutes) + ":" + String(totalRemainingSeconds) + "\n";
    
    NSerial.println(dataToSend);
  } else {
    if (acIsOn) {
      acIsOn = false;
      totalTime += sessionTime;  // Add the session time to totalTime when AC turns off
      sessionTime = 0;  // Reset sessionTime when AC is off
      Serial.println("AC just turned OFF");  
    }
    
    // Even when AC is off, send the total ON time for the day
    unsigned long totalSeconds = totalTime / 1000;
    unsigned int totalHours = totalSeconds / 3600;
    unsigned int totalMinutes = (totalSeconds % 3600) / 60;
    unsigned int totalRemainingSeconds = totalSeconds % 60;
    
    String dataToSend = "Current(A): " + String(data, 3) + " A\n";
    dataToSend += "AC is OFF\n";
    dataToSend += "Total AC ON Time Today - " + String(totalHours) + ":" + String(totalMinutes) + ":" + String(totalRemainingSeconds) + "\n";
    
    NSerial.println(dataToSend);
  }

  delay(1000);
}
