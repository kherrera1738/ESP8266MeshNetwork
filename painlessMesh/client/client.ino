#include <painlessMesh.h>
#include <HashMap.h>

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2

#define   MESH_SSID       "EE122mesh"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555
#define   HASH_SIZE       6
#define   FIN             13
#define   TRIALS          100
#define   MSG_SIZE        3325

// Prototypes 
String create_message(uint32_t to);
void sendMessage();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

char dst = '2';
char src = '3';

uint32_t send_time;
bool wait = false;
int trial = 0;
uint32_t sum = 0;
unsigned long data[TRIALS];

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
HashType<char,uint32_t> router[HASH_SIZE]; 
HashMap<char,uint32_t> routerT = HashMap<char,uint32_t>( router , HASH_SIZE ); 

HashType<uint32_t, char> ids[HASH_SIZE]; 
HashMap<uint32_t, char> idsT = HashMap<uint32_t, char>( ids , HASH_SIZE ); 

SimpleList<uint32_t> nodes;
String msg;
void sendMessage() ; // Prototype
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(FIN, OUTPUT);
  //setup hashmap
  routerT[0]('1',571298772);
  routerT[1]('2',571296790);
  routerT[2]('3',571172289);
  routerT[3]('4',571166159);
  routerT[4]('5',571114422);
  routerT[5]('6',571097391);

  idsT[0](571298772, '1');
  idsT[1](571296790, '2');
  idsT[2](571172289, '3');
  idsT[3](571166159, '4');
  idsT[4](571114422, '5');
  idsT[5](571097391, '6');
  
  Serial.println("\nstarting mesh..");
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  msg = create_message(dst, MSG_SIZE);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  taskSendMessage.setInterval(TASK_SECOND); 

  randomSeed(analogRead(A0));
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}

void sendMessage() {
  if(!wait || mesh.getNodeTime() - send_time > 100000){
    if(!wait) send_time = mesh.getNodeTime();
    uint32_t dst_id = routerT.getValueOf(dst);
    mesh.sendSingle(dst_id, msg);
    wait = true;
  }  
}

void receivedCallback(uint32_t from, String & msg) {
//  Serial.printf("%s\n", msg.c_str());
  wait = false;
  Serial.printf("%d diff: %u\n", trial, mesh.getNodeTime() - send_time);
  if(trial < TRIALS){
    unsigned long diff = mesh.getNodeTime() - send_time;
    data[trial] = diff;
    sum += diff;
    trial++;
  }

  if(trial == TRIALS) {
    digitalWrite(FIN, HIGH);
//    for(int i = 0; i < TRIALS%10; i++) {
//      for(int j = 0; j < 10; j++) {
//        Serial.print(data[i*10 + j]);
//      }
//      Serial.println();
//    }
    Serial.println(sum/trial);
    taskSendMessage.disable();
  } 
  send_time = 0;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  nodes = mesh.getNodeList();

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {return;}

void delayReceivedCallback(uint32_t from, int32_t delay) {return;}

String create_message(char dst, int payload_size) {
  msg = "";
  msg += dst;
  msg += ",";
  msg += src;
  for(int i = 0; i < payload_size;i++) {msg += 'a';}
  return msg;
}


