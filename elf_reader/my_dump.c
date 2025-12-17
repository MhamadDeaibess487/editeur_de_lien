#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE* f = fopen(argv[1],"rb");
    char c;
    int i = 0;
    while(fread(&c,sizeof(char),1,f)!=0){
        if (i==8){
            printf("   ");
        }
        if (i==16){
            printf("\n");
            i=0;
        }
        printf("%02x ",c);
        i++;
    }
}