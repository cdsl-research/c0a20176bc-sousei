#include "stdio.h"
#include "stdlib.h"
#include "string.h"


// sequence array & push method
int seq_ary[5] = { 3,5,6,7,8 };

int tmp;
int count;
int ary_size;


void printArray(const int* array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%d ", array[i]);
    }
}

int main() {
              ary_size = sizeof(seq_ary)/ sizeof(seq_ary[0]);
              printf("ary_size: {%d}\n", ary_size);
              printArray(seq_ary, ary_size);
              printf("\n");

              char loss[30] = "";
              char buf[4];

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
              
              // 10 = 最大シーケンス番号
              if ( seq_ary[ary_size-1] != 10 ) {
                for ( count=seq_ary[ary_size-1]+1; count <= 10; count++ ) {
                    snprintf(buf, sizeof(buf), "%d", count);
                    strcat(loss, buf);
                    strcat(loss, ",");
                }
              }
              
              printf("\n");
              printf("%s sended!\n", loss);
        
              count = 0;
}


