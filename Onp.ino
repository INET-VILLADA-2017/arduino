#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(168, 83, 20, 187);
EthernetServer server(80);
EthernetClient client;
String reaString = String(100); //string for fetching data from address
String toSave = String(41);
int addr = 0;
byte byteArray[41];

bool readRequest(EthernetClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (reaString.length() < 100) {
        reaString.concat(c);
      }
      if (c == '\n' && currentLineIsBlank) {
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  return false;
}

void writeResponse(EthernetClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
}


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // wait serial port initialization
  }
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


}
const int arraysize = 10;
const char* denominacion[] = {"Vivero La Piedra", "Vivero Fenix"};
const char* empresa[] = {"La Piedra srl", "Fenix S.A"};
const char* clave[] = {"23418544321", "43418544123"};
const char* direccion[] = { "Piedra Buena 420", "Las Piedras 240" };
const float valores[] = { 4.20, 2.40, 4, 5};
const char* unidades[arraysize] = { "c", "so", "%", "m3", "r1", "r2", "pr1", "pr2", "z", "s"};
const char* duracion[] = { "4:20", "2:40" };
JsonObject& createJson () {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& vivero = root.createNestedObject("v");
  vivero["c"] = clave[random(0, 2)];
  for (int unidad = 0; unidad < arraysize; unidad++) {
    JsonObject& medicion = root.createNestedObject(unidades[unidad]);
    medicion["v"] = valores[random(0, 4)];
    medicion["er"] = random(1, 4);
    medicion["d"] = duracion[random(0, 2)];
    medicion["en"] = random(1, 3);
  }
  return (root);
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New Client");
    bool success = readRequest(client);
    if (success && reaString.startsWith("POST")) {
      Serial.println("POST");
      Serial.println(reaString);
      toSave = reaString.substring(7, 48);
      Serial.println(toSave);
      saveString(toSave);
      toSave  = "";
      reaString = "";
      readMemory();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
    } else {
      Serial.println(success);
      Serial.println(reaString);
      reaString = "";
      writeResponse(client, createJson());
    }
    delay(1);
    client.stop();
  }
}

void saveString(String toSave) {
  Serial.println(toSave.length());
  for ( addr = 0; addr <= toSave.length(); addr++) {
    if (addr == EEPROM.length()) {
      addr = 0;
    }
    Serial.println(toSave[addr]);
    EEPROM.write(addr, toSave[addr]);
  }
}

String readMemory(){
    String storedValues;
    for ( addr = 0; addr <= 41; addr++) {
      byteArray[addr] = EEPROM.read(addr);
  }
  storedValues = String((char *)byteArray);
  Serial.println(storedValues);
  return storedValues;
}


