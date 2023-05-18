#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
String tag;
String lastTag;


const char* ssid = "preamza02";
const char* password = "12345678";
const char* mqtt_server = "34.126.157.245";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

EspSoftwareSerial::UART testSerial;

// int 
int yellowPin = D0;
int redPin = D8;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char nice[20];
  String truePayload;
  Serial.println("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    nice[i] = (char)payload[i];
    // testSerial.write(nice[i]);
  }
  nice[length] = '/';
  // truePayload = (String)payload;
  Serial.println(nice);
  testSerial.write(nice);

  // Switch on the LED if an 1 was received as first character
  // if (nice[0] == '-') {
  digitalWrite(redPin, HIGH);
  // } else {
  //   digitalWrite(redPin, HIGH);
  //   digitalWrite(yellowPin, LOW);
  // }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("WaterAmount");
    } else {
      // Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// void setup_connection_to_stm32(){
//   Serial.print("Connecting to STM 32\n");
//   testSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, D0, D1,false, 95, 11);
//   Serial.print("finish to STM 32\n");
// }
void setup() {
  // put your setup code here, to run once:
  testSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, D1, D2,false, 95, 11);
  pinMode(yellowPin,OUTPUT);
  pinMode(redPin,OUTPUT);
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  // pinMode(10,OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe("WaterAmount");
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}

// char get_RFID(){
//   return "";
// }

float humunity;
float temperature;
float waterD;
int MSG_BUFFER_SIZE = 50;
String msg;
int state = 0;
char c;
String temp = "";
String humid = "";
String waterDrank = "";
bool s = true;
void loop() {
  // // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // state = 0;
  // digitalWrite(redPin, LOW);
  // digitalWrite(yellowPin, LOW);
  while (testSerial.available() > 0) {
    // a = testSerial.read();
    c = testSerial.read();
    if (c =='P' || c=='B'){
      state = 0;
      continue;
    }
    switch(state) {
      case 0:
        if (c == '/') {
          state = 1;
          break;
          }
        
        temp += c;
        break;
      case 1:
        if (c == '/') {
          state = 2;
          break;
          }
        humid += c;
        break;
      case 2:
        if (c == '/') {
          state = 0;
          break;
          }
        waterDrank += c;
        break;
      // case 3:
      //   state = 0;
      //   break;
      default:
        break;
    }
    Serial.print(c);
    // yield();
  }
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
  lastTag = "";
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, s);
  s = !s;
  lastMsg = now;
  // humunity = 0;
  // temperature = 0;
  // snprintf (msg, MSG_BUFFER_SIZE, "humunity %.2f temperature %.2f\n", humunity,temperature);
  // if humid == ""
  msg = "humunity "+ humid +" temperature " + temp+" waterDrank " +waterDrank + "\n";
  Serial.print("\n");
  Serial.print("Publish message: ");
  Serial.print(msg);
  client.publish("Humidity",humid.c_str());
  client.publish("Temperatures",temp.c_str());
  client.publish("DrankWater",waterDrank.c_str());
  humid = "";
  temp = "";
  waterDrank = "";
  if ( ! rfid.PICC_IsNewCardPresent()){
    Serial.println("new card not present");
    Serial.println(rfid.PICC_ReadCardSerial());
  }
  else if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    tag = tag.substring(0,6);
    if (lastTag != tag){
      Serial.print("publish new tag : ");
      Serial.print(tag);
      Serial.print("\n");
      client.publish("RFID",tag.c_str());
      lastTag = tag;
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  }
}
