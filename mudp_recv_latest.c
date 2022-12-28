#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "WiFi.h"
#include "AsyncUDP.h"


const char * ssid = "elecom-68d043";
// const char * ssid = "CDSL-A910-11n";
const char * password = "33dcu4jvm9d9";
// const char * password = "11n-ky56$HDxgp";
const int port = 1234;
time_t st = 0;
time_t et = 0;

AsyncUDP udp;

WiFiServer server(port);


// sequence array & push method
int seq_ary[120];
int max_recv_size = 0;
int tmp = 0;
int ary_size = 0;
int count = 0;

int compareInt(const void* a, const void* b) {
    int aNum = *(int*)a;
    int bNum = *(int*)b;

    return aNum - bNum;
}


void printArray(const int* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%d ", array[i]);
    }
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
            char recvd[packet.length()];
            memcpy(recvd, packet.data(), packet.length());

            printf("recv: %s", recvd);
            printf("\n");

            char *token;
            token = strtok(recvd, ":");
            
            max_recv_size = atoi(token);
                  
            token = strtok(NULL, ":");
            
            int add_item = atoi(token);

            seq_ary[ary_size] = add_item;
            ary_size++;

            count++;

            // reply to the client
            if (count == max_recv_size) {
              printf("\n");

              qsort(seq_ary, ary_size, sizeof(int), compareInt);

              printArray(seq_ary, ary_size);

              printf("\n");

              // search loss packets
              char loss[120] = "";
              char buf[4];

              // recv packet 0 case is Abbreviated...

              // count...seq counter
              // ex) seq_ary[3,4,5,6,7] / [1,2] loss search
              if ( seq_ary[0] != 1 ) {
                for ( count=1; count < seq_ary[0]; count++ ) {
                  snprintf(buf, sizeof(buf), "%d", count);
                  strcat(loss, buf);
                  strcat(loss, ",");
                }
              }

              count = seq_ary[0] + 1;
              for ( tmp=1; tmp < ary_size; tmp++ ) {
                while ( count < seq_ary[ary_size-1] ) {
                  if ( seq_ary[tmp] == count ) {
                      count++;
                      break;
                  } else {
                      snprintf(buf, sizeof(buf), "%d", count);
                      strcat(loss, buf);
                      strcat(loss, ",");
                      count++;
                  }
                }
              }
              
              // ex) seq_ary[3,4,5,6,7] / [8,9,10] loss search
              // 10 = 最大シーケンス番号
              if ( seq_ary[ary_size-1] != max_recv_size ) {
                for ( count=seq_ary[ary_size-1]+1; count <= 10; count++ ) {
                    snprintf(buf, sizeof(buf), "%d", count);
                    strcat(loss, buf);
                    strcat(loss, ",");
                }
              }
              
              packet.print(loss);
              Serial.printf("%s sended!\n", loss);
              
              count = 0;
              ary_size = 0;
            }
        });
    }
}


// TCP送受信部分
void loop()
{
    if ( st != 0 && et - et > 2 ) {

      WiFiClient client = server.available();

      if ( client ) {
        Serial.println("New Client Created");
        while ( client.connected() ) {
          // recv packet loss data from server
          int size = client.available();
          // if( size ) {
          //   Serial.println();
          //   Serial.println("size:");
          //   Serial.println(size);

          //   for (int i = 0; i < size; i++) {
          //     // uint8_t...8ビット整数型
          //     // c 10進数. ex) M = 77 = 0x4c  
          //     uint8_t c = client.read();
          //     // 1byteを1文字に変換
          //     Serial.printf("get: %c\n", c);
          //   }

            // char input[256];
            // memset(input, 0, sizeof(input));
            // client.read((uint8_t*)input, sizeof(input) - 1);
            // Serial.printf("get: [%s]\n", input);
          // }
          delay(1);
        }
        // server.write();
        client.stop();
        Serial.println("client.stop");
      }
   }
}
