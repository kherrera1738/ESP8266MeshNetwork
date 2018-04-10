#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

#define TRIALS 500

unsigned int ttl = 8;
unsigned int indicator = LED_BUILTIN;
unsigned int done = 14;
String neighbors[5];
float averages[TRIALS];
unsigned int num_neighbors = 0;
String ssid_prefix = "Mesh_Node";

String handler(String message) {
  Serial.print("Reply: ");
  Serial.println(message);
}

ESP8266WiFiMesh node = ESP8266WiFiMesh(ESP.getChipId(), handler);

void find_nodes() {
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  num_neighbors = 0;
  
  for(int i = 0; i < n; i++) {
    String current_ssid = WiFi.SSID(i);
    int index = current_ssid.indexOf(ssid_prefix);
    uint32_t target_chip_id = (current_ssid.substring(index + ssid_prefix.length())).toInt();

    /* Connect to any _suitable_ APs which contain _ssid_prefix */
    if (index >= 0) {
      neighbors[num_neighbors] = current_ssid;
      num_neighbors++; 
    }
  }
} 

void print_neighbors() {
  if (num_neighbors == 0) {
    Serial.println("no nodes found");
  } else {
    Serial.print(num_neighbors);
    Serial.println(" networks found");
    for (int i = 0; i < num_neighbors; i++) {
      // Print SSID and RSSI for each network found
      Serial.println(neighbors[i]);
      delay(10);
    }
  }
  Serial.println();
}

void calc_trans_prob() {
  float average = 0, trials = 1, count = 0;
  for(int i = 0; i < TRIALS; i++) {
    find_nodes();
    if(num_neighbors > 0) {count++;}
    average = count/trials;
    averages[i] = average;
    trials++;
    Serial.print("current average: ");
    Serial.print(average);
    Serial.print(" ");
    Serial.println(i);
    Serial.println();
  }  

  
  char result[100];
  sprintf(result, "final prob: %.2f; count: %.0f; trials: %.0f;", average, count, trials - 1);
  Serial.println(result);

  for(int i = 0; i < TRIALS; i++) {
    if(i%20 == 0) {Serial.println();}
    Serial.print(averages[i]);
    Serial.print(", ");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(indicator, OUTPUT);
  pinMode(done, OUTPUT);
  Serial.println();
  Serial.println("Starting up mesh");
  node.begin();
  digitalWrite(indicator, LOW);
//  calc_trans_prob();
  digitalWrite(done, HIGH);
}

void loop() {;}
