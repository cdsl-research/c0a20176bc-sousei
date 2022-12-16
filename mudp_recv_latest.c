#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "WiFi.h"
#include "AsyncUDP.h"


const char * ssid = "elecom-68d043";
// const char * ssid = "CDSL-A910-11n";
const char * password = "33dcu4jvm9d9";
// const char * password = "11n-ky56$HDxgp";
const int port = 3333;
int count = 0;

AsyncUDP udp;

WiFiServer server(port);


// sequence array & push method
int seq_ary[20];
int *seq_ptr = seq_ary;
int add;
int tmp;
int ary_size;

int array_add(int array[], int add) {
    static e_size = 0;
    
    array[e_size] = add;
    e_size = e_size + 1;
    
    return e_size;
}


void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    } else {
      Serial.println("connected!");

      // Server begin
      server.begin();
      Serial.println("server.begin()");
    }

    if (udp.listenMulticast(IPAddress(239,1,2,3), port)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            // Serial.print(", Data: ");
            // Serial.write(packet.data(), packet.length());
            Serial.println();

            char *recvd;
	          recvd = (char *)malloc(sizeof(char) * packet.length());
            
            sprintf(recvd, "%s", packet.data());
            printf("recv: %s\n", recvd);

            char *token;
            token = strtok(recvd, ":");
            printf("%s\n", token);
            
            int add_item = atoi(token);

            ary_size = array_add(seq_ary, add_item);
            
            printf("\n");
            for (tmp = 0; tmp < ary_size; tmp++) {
                printf("%d: {%d}\n", tmp, seq_ary[tmp]);
            }
                  
            token = strtok(NULL, ":");
            printf("data: %s\n", token);

            count++;
            //reply to the client
            // if (count == 10) {
            //   sleep(1);
            //   packet.printf("2,3,7");
            //   Serial.print("sended!");
            //   count = 0;
            // }
        });
        //Send multicast
        udp.print("Hello!");
    }
}


// TCP送受信部分
void loop()
{
    WiFiClient client = server.available();

    if ( client ) {
      Serial.println("New Client Created");
      while ( client.connected() ) {
        // recv packet loss data from server
        int size = client.available();
        if( size ) {
          Serial.println();
          Serial.println("size:");
          Serial.println(size);

        for (int i = 0; i < size; i++) {
          // uint8_t...8ビット整数型
          // c 10進数. ex) M = 77 = 0x4c  
          uint8_t c = client.read();
          // 1byteを1文字に変換
          Serial.printf("get: %c\n", c);
        }

          // char input[256];
          // memset(input, 0, sizeof(input));
          // client.read((uint8_t*)input, sizeof(input) - 1);
          // Serial.printf("get: [%s]\n", input);
        }

        delay(1);
      }
      // server.write();
      client.stop();
      Serial.println("client.stop");
    }
}
