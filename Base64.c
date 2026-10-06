#include <stdio.h>
#include "Base64.h"

unsigned char bitGroupFromChar(char encoded){
    if(!(42 < encoded || encoded < 123)) return 0;
    unsigned char c = encoded;
    
    // uppercase letters
    if(64 < c && c < 91) return c - 65;
    // lowercase kettes
    if(96 < c && c < 123) return (c - 97) + 26;
    // numbers
    if(47 < c && c < 58) return (c - 48) + 52;
    // extra
    if(c == '+') return 62; // '+'
    if(c == '/') return 63; // '/'

    return 0;
}

char charFrom6BitGroup(unsigned char bits){
    static char arr[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
    if(bits > 63) return 0;
    return arr[bits];
}

int test(){
    int bad = 0;
    for(int i = 0; i < 64; i++){
        char firstC = charFrom6BitGroup(i);
        if(firstC == 0) {
            printf("Problem on index %d = %c\n", i, firstC);
            bad = 1;
        }

        unsigned char bits = bitGroupFromChar(firstC);
        if(bits != i) {
            bad = 1;
            printf("Problem on index %d; %c => %d\n", i, firstC, bits);
        }
    }

    return bad;
}