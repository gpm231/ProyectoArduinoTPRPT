#include <SPI.h>
#include <WiFiNINA.h>

// WIFI VARS

#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

int status = WL_IDLE_STATUS;
char server[] = "servidorappconsumo.goranpalacio.repl.co";    

WiFiClient client;

// VOLTAGE VARS

const int voltageSensor = A1;

float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30002.0;
float R2 = 7501.0;
int value = 0;

// CURRENT VARS

float Sensibilidad = 0.0733;

int pinIntensidad = A0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("Connected");
    status = WiFi.begin(ssid, pass);

    delay(2000);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
}

void loop() {
  // WIFI CODE
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // SEND DATA TO SERVER
  float potencia = get_potencia(10, A0, A1);

  Serial.println(potencia, 5);
  
  postData(potencia);

  // WIFI CODE
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }

  delay(1000);
}

float get_potencia(int n_muestras, int pinIntensidad, int pinVoltaje) {
  float totalPotencia = 0;
  
  for (int i = 0; i < n_muestras; i++) {
    value = analogRead(pinVoltaje);
    vOUT = (value * 5.0) / 1024.0;
    vIN = vOUT / (R2/(R1+R2));
    
    float voltaje = vIN;
    float intensidad = get_corriente(1000, pinIntensidad);

    Serial.println(voltaje);
    Serial.println(intensidad);

    totalPotencia = totalPotencia + voltaje * intensidad;
  }

  return totalPotencia / n_muestras;
}

float get_corriente(int n_muestras, int pin)
{
  float voltajeSensor;
  float corriente = 0;
  
  for(int i = 0; i < n_muestras; i++)
  {
    voltajeSensor = analogRead(pin) * (5.0 / 1023.0);
    corriente = corriente + ((voltajeSensor - 2.5) / Sensibilidad);
  }
  corriente = corriente / n_muestras;
  return(corriente);
}

void printWifiStatus() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void postData(float data) {
  String reqBody = "type:set/data:" + String(data);
  
  if (client.connectSSL(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST / HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/json");
    client.println("Accept: */*");
    client.println("Cache-Control: no-cache");
    client.println("Accept-Encoding: gzip, deflate");
    client.println("Accept-Language: en-us");
    client.println("Content-Length: " + String(reqBody.length()));
    client.println("Connection: close");
    client.println(); //end HTTP request header
    client.println(reqBody);
    client.println();
  }
}
