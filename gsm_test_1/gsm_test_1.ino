#include <HardwareSerial.h>
HardwareSerial Sim800L(2);
//HardwareSerial Sim800L(1);

void setup() {
  Serial.begin(115200);
 Sim800L.begin(115200, SERIAL_8N1, 17, 16);
  Sim800L.begin(115200);
  delay(200);
  Serial.println("\nInitializing...");
  delay(200);

  // Check if SIM card is ready
  while (!isSimCardReady()) {
    Serial.println("Waiting for SIM card to be ready...");
    delay(2000);  // Adjust the delay as needed
  }

  // Check network registration
  while (!isNetworkRegistered()) {
    Serial.println("Searching for network registration...");
    delay(2000);
  }

  Sim800L.println("AT");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CSQ");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CBC");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CNMI?");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CNMI=2,1,0,0,0");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CNMI?");
  delay(200);
  updateSerial();

  Sim800L.println("AT+CSMP=17,167,0,0");
  delay(200);
  updateSerial();
  Sim800L.println("AT+CREG=2");
  delay(200);
  updateSerial();
  Sim800L.println("ATH");
  delay(200);
  updateSerial();
  Sim800L.println("AT+CNMP=?");
  delay(200);
  updateSerial();
  Sim800L.println("AT+CNMP?");
  delay(200);
  updateSerial();
  Serial.println("\nTest now");
  Serial.println("\trying internet");

  tryingInternet();
}

void tryingInternet(){

 Sim800L.println("AT+CGDCONT=1,\"IP\",\"jionet\""); // Define PDP Context
   delay(200);
  updateSerial();

 Sim800L.println("AT+CGACT=1,1"); // Activate PDP Context
   delay(200);
  updateSerial();

   Sim800L.println("AT+CREG?");
   delay(200);
  updateSerial();
  
   Sim800L.println("AT+CGATT?");
   delay(200);
  updateSerial();

   Sim800L.println("AT+HTTPINIT");
    delay(200);
  updateSerial();
  // Set HTTP profile identifier
  Sim800L.println("AT+HTTPPARA=\"CID\",1");
    delay(200);
  updateSerial();
  
  // Set the URL
  Sim800L.println("AT+HTTPPARA=\"URL\",\"http://www.google.com\"");
   delay(200);
  updateSerial(); 
  // Start GET session (0 denotes GET)
  Sim800L.println("AT+HTTPACTION=0");
    delay(200);
  updateSerial();
  // Wait for a while to let the HTTP request complete
  delay(10000); // Adjust the delay according to your network speed
  
  // Read the HTTP server response
  Sim800L.println("AT+HTTPREAD");
    delay(200);
  updateSerial();
  // Terminate HTTP service
  Sim800L.println("AT+HTTPTERM");
    delay(200);
  updateSerial();
}
void loop() {

  updateSerial();
}

void updateSerial() {
  while (Serial.available()) {
    String a = Serial.readString();
    Sim800L.println(a);
  }

  while (Sim800L.available()) {
    Serial.print(char(Sim800L.read()));
  }
}

bool isSimCardReady() {
  int retryCount = 0;
  int maxRetries = 20;  // Adjust as needed

  while (retryCount < maxRetries) {
    Sim800L.println("AT+CPIN?");
    delay(500);
    String response ="";
    
    while (Sim800L.available()) {
     response = response+ String(char(Sim800L.read()));
    }
      Serial.println("Response from SIM800L: " + String(response));

      if (response.indexOf("READY") != -1) {
        Serial.println("SIM card is ready!");
        return true;
      } else if (response.indexOf("SIM PIN") != -1) {
        Serial.println("SIM card requires PIN. Please enter PIN.");
        // You may want to add code to enter the PIN here
        return false;
      }
      delay(500);
    

    retryCount++;
  }

  Serial.println("SIM card readiness timeout");
  return false;
}

bool isNetworkRegistered() {
  while (true) {
    Sim800L.println("AT+CSQ");
    delay(200);

    // Wait for a response
    while (Sim800L.available()) {
      String response = Sim800L.readString();
      Serial.println("Signal Strength (CSQ): " + response);

      // Extract the signal strength value
      int signalStrength = response.substring(response.indexOf(":") + 2, response.indexOf(",")).toInt();

      // Check if signal strength is within an acceptable range (adjust the threshold)
      if (signalStrength > 0) {
        Serial.println("Network registered!");
        return true;
      }
      delay(500);
    }
  }
}
