#include <painlessMesh.h>
#include <HashMap.h>

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2

#define   MESH_SSID       "EE122mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555
#define   HASH_SIZE       6


// Prototypes
void sendMessage();  
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

uint32_t id = 571296790;

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
HashType<char,uint32_t> router[HASH_SIZE]; 
HashMap<char,uint32_t> routerT = HashMap<char,uint32_t>( router , HASH_SIZE ); 

HashType<uint32_t, char> ids[HASH_SIZE]; 
HashMap<uint32_t, char> idsT = HashMap<uint32_t, char>( ids , HASH_SIZE );

SimpleList<uint32_t> nodes;

void setup() {
  Serial.begin(115200);

  //setup hashmap
  routerT[0]('1',571298772);
  routerT[1]('2',571296790);
  routerT[2]('3',571114422);
  routerT[3]('4',571166159);
  routerT[4]('5',571114422);
  routerT[5]('6',571097391);

  idsT[0](571298772, '1');
  idsT[1](571296790, '2');
  idsT[2](571172289, '3');
  idsT[3](571166159, '4');
  idsT[4](571114422, '5');
  idsT[5](571097391, '6');
  

  pinMode(LED, OUTPUT);
  Serial.println("\nstrating mesh..");
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
}

void loop() {
  
  mesh.update();
}

void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("%s\n", msg.c_str());
  String msg_s = msg.c_str();
  char src = msg.charAt(2);
  char dst = msg.charAt(0);
  uint32_t dstn = routerT.getValueOf(dst);
  mesh.sendSingle(dstn, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {return;}

void delayReceivedCallback(uint32_t from, int32_t delay) {return;}
