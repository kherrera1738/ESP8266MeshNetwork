#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

unsigned int request_i = 0;
unsigned int response_i = 0;
unsigned int indicator = 16;
bool acked = false;

String manageRequest(String request);

/* Create the mesh node object */
ESP8266WiFiMesh mesh_node = ESP8266WiFiMesh(ESP.getChipId(), manageRequest);

/**
   Callback for when other nodes send you data
   @request The string received from another node in the mesh
   @returns The string to send back to the other node
*/
String manageRequest(String request) {
  /* Print out received message */
  Serial.print("received: ");
  Serial.println(request);

  /* return a string to send back */
  char response[60];
  sprintf(response, "Hello world response #%d from Mesh_Node%d.", response_i++, ESP.getChipId());
  return response;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Setting up mesh node...");

  pinMode(indicator, OUTPUT);
  digitalWrite(indicator, LOW);
  /* Initialise the mesh node */
  mesh_node.begin();
}

void loop() {
  /* Accept any incoming connections */
  Serial.println("receive");
  mesh_node.acceptRequest();
  delay(1000);
}


