# 1 "/Users/chen/www/hass/light/light.ino"

# 3 "/Users/chen/www/hass/light/light.ino" 2
# 4 "/Users/chen/www/hass/light/light.ino" 2
# 5 "/Users/chen/www/hass/light/light.ino" 2
# 6 "/Users/chen/www/hass/light/light.ino" 2

// light gpio



// switch out




// WiFi
const char *ssid = "Xiaomi_F3EE_2.4G"; // Enter your WiFi name
const char *password = "b5z0ubf4"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.210";
const char *topic = "eps8266/bedroom/top/light/switch";
const char *topic_status = "eps8266/bedroom/top/light/status";
const char *mqtt_username = "hass";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;

char switch_open[] = "ON"; // light ON
char switch_close[] = "OFF"; // light OFF

Ticker ticker;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, 0x01);
  pinMode(0, 0x01);
  digitalWrite(2, 0x0);
  digitalWrite(0, 0x0);
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  reconnect_mqtt();
  ticker.attach(10, sayHi);
}


void sayHi(){
  int status = digitalRead(0);
  Serial.printf("switch-%d", status);
   Serial.println();
  if(status == 0x0) {
    client.publish(topic_status, switch_open);
  } else if(status == 0x1) {
    client.publish(topic_status, switch_close);
  }
}

void reconnect_mqtt(){
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
          // publish and subscribe
          client.publish(topic, "hello emqx");
          client.subscribe(topic);
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

    char payload_str[length] = { '\0' };

    memcpy(payload_str, payload, length );

    puts( payload_str );
    Serial.println(payload_str);

  if(0 == strcmp(payload_str,switch_open)) {
    digitalWrite(2, 0x0);
    digitalWrite(0, 0x0);
  } else if(0 == strcmp(payload_str,switch_close)){
    digitalWrite(2, 0x1);
    digitalWrite(0, 0x1);
  } else {
    return;
  }

  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
  reconnect_mqtt();
  // digitalWrite(LED, LOW);
  // delay(1000);
  // digitalWrite(OPEN, HIGH);
  // delay(1000);
}
