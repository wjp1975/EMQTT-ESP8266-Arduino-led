/*
   EMQTT-ESP82666-Arduino-led

   作者：imliubo 时间：2017-09-10
   个人主页：http://www.imliubo.me/
   ssid和password改成你所在的网络
   mqtt_server改成你的服务器IP（因为这个服务器过几天就到期了。。。。。）
   首次使用Arduino IDE请先安装ESP8266环境，具体教程百度一下即可，这里不再赘述。
   PubSubClient库下载解压后放到Arduino安装文件夹下的libraries的文件夹中。
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Tenda_1F1000";
const char* password = "13290109635";
const char* mqtt_server = "119.29.244.94";

WiFiClient espClient;
PubSubClient client(espClient);
int value = 0;
int LED = 5;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '0')
  {
    digitalWrite(5, LOW);
    //向客户端推送设备状态
    client.publish("outTopic", "off-state");
    Serial.print("Publish message: off-state");
    Serial.println();
  }
  else if ((char)payload[0] == '1')
  {
    digitalWrite(5, HIGH);
    //向客户端推送设备状态
    client.publish("outTopic", "on-state");
    Serial.print("Publish message: on-state");
    Serial.println();
  }
  else
  {
    client.publish("outTopic", "nocommand");
    Serial.print("Publish message: nocommand");
    Serial.println();
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 生成一个设备的随机ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println();
      Serial.println("connected");
      // 设备上线推送上线消息
      client.publish("outTopic", "设备上线了！");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
