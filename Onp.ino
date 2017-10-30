#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(168, 83, 20, 187);
EthernetServer server(80);
EthernetClient client;

bool readRequest(EthernetClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
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
const char* denominacion[] = {"Vivero La Piedra", "Vivero Fenix"};
const char* empresa[] = {"La Piedra srl", "Fenix S.A"};
const char* clave[] = {"23418544321", "43418544123"};
const char* direccion[] = { "Piedra Buena 420", "Las Piedras 240" };
const float valores[] = { 4.20, 2.40, 4, 5};
const char* unidades[] = { "C", "lux", "%", "m3" };
const char* duracion[] = { "4:20", "2:40" };
JsonObject& createJson () {
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& vivero = root.createNestedObject("vivero");
  vivero["Denominacion"] = denominacion[random(0,2)];
  vivero["Empresa"] = empresa[random(0,2)];
  vivero["ClaveFiscal"] = clave[random(0,2)];
  vivero["Direccion"] = direccion[random(0,2)];
  JsonObject& medicion = root.createNestedObject("medicion");
  medicion["valor"] = valores[random(0,4)];
  medicion["UnidadFisica"] = unidades[random(0,4)];
  medicion["Estado_Transductor"] = random(1,4);
  medicion["Duracion"] = duracion[random(0,2)];
  medicion["Estado_Transmicion"] = random(1,3);
  return (root);
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New Client");
    bool success = readRequest(client);
    if (success) {
      writeResponse(client, createJson());
    }
    delay(1);
    client.stop();
  }
    Serial.println("client disconnected");
  }

void postJson(JsonObject& json) {
    if (client.connect("168.83.20.187",80)) { // REPLACE WITH YOUR SERVER ADDRESS
    client.println("POST / HTTP/1.1"); 
    client.println("Host: 168.83.20.187"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/json"); 
    client.print("Content-Length: "); 
    client.println(json.measureLength()); 
    client.println(); 
    json.prettyPrintTo(client);
  } 

  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
}




