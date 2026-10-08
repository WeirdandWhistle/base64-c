#include <stdio.h>
#include "Base64.h"

unsigned char bitGroupFromChar(char encoded){
    if(!(42 < encoded || encoded < 123)) return 255;
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
    if(c == '=') return -2;

    return 255;
}

char charFrom6BitGroup(unsigned char bits){
    static char arr[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
    if(bits > 63) return 0;
    return arr[bits];
}

int encodedStringLength(unsigned long long* outLength, char* chars, unsigned long long length){
    if(outLength == NULL) return 1;
    unsigned long long runningLength = 0;
    for(unsigned long long i = 0; i < length; i++){
        if(bitGroupFromChar(chars[i]) < 64) runningLength++;
    }
    *outLength = runningLength;
    return 0;
}

int decodeSmart(unsigned char* out, char* chars, unsigned long long length, int automaticPadding, int enforeAcceptedAlphabet){
    unsigned long long stringLength = 0;
    if(encodedStringLength(&stringLength, chars, length)){
        printf("Finding string length failed for some reason...\n");
        return 1;
    }
    unsigned long long fullGroups = stringLength / 4;

    int isPadding = 0;
    if(!automaticPadding){
        if(chars[length-1] == '=') isPadding = 1;
        if(isPadding && chars[length-2] == '=') isPadding = 2;

        if(isPadding) fullGroups -= 1;
    }
    

    if(fullGroups * 4 != length) {
        if(!automaticPadding){
            printf("Base 64 decoding failed! Padding is wrong! Not a multible of 4.\n");
            return 1;
        }   
    }

    unsigned long long charsRead = 0;

    printf("fullGroup: %lld, stringLen: %lld\n",fullGroups, stringLength);

    unsigned char bitGroups[4] = {0};
    unsigned long long readOffset = 0;
    for(unsigned long long i = 0; i < fullGroups; i++){
        for(int j = 0; j < 4; j++){
            bitGroups[j] = bitGroupFromChar(chars[i*4 + j + readOffset]);
            if(bitGroups[j] == 255) {
                if(enforeAcceptedAlphabet) {
                    printf("Base64 decoding failed! Char not in alphabet!\n");
                    return 1;
                }
                j--;
                readOffset++;
            }
        }
        charsRead += 4;

        out[i*3 + 0] = ((bitGroups[0] << 2) & 0xFC) | ((bitGroups[1] >> 4) & 0x03);
        out[i*3 + 1] = ((bitGroups[1] << 4) & 0xF0) | ((bitGroups[2] >> 2) & 0x0F);
        out[i*3 + 2] = ((bitGroups[2] << 6) & 0xC0) | ((bitGroups[3]) & 0x3F);
    }
    if(charsRead != stringLength){
        if(charsRead > stringLength){
            printf("Read more chars than in the base64 string. shouldn't be possible.\n");
            return 1;
        }
        int difRead = stringLength - charsRead; 
        if(difRead != 2 && difRead != 3){
            printf("Read %d chars under the string length. This shouldn't happen. charsRead: %lld, stringLength: %lld, fullGroups: %lld\n", difRead, charsRead, stringLength, fullGroups);
            return 1;
        }
        if(difRead == 2) isPadding = 2;
        if(difRead == 3) isPadding = 1;
    }

    if(isPadding){
        if(isPadding == 2){
            for(int i = 0; i < 2; i++){
                bitGroups[i] = bitGroupFromChar(chars[fullGroups*4 + i + readOffset]);
                if(bitGroups[i] == 255) {
                    if(enforeAcceptedAlphabet) {
                        printf("Base64 decoding failed! Char not in alphabet!\n");
                        return 1;
                    }
                    i--;
                    readOffset++;
                }
            }
            
            out[fullGroups*3 + 0] = ((bitGroups[0] << 2) & 0xFC) | ((bitGroups[1] >> 4) & 0x03);
        } else if(isPadding == 1){
            for(int i = 0; i < 3; i++){
                bitGroups[i] = bitGroupFromChar(chars[fullGroups*4 + i + readOffset]);
                if(bitGroups[i] == 255) {
                    if(enforeAcceptedAlphabet) {
                        printf("Base64 decoding failed! Char not in alphabet!\n");
                        return 1;
                    }
                    i--;
                    readOffset++;
                }
            }

            out[fullGroups*3 + 0] = ((bitGroups[0] << 2) & 0xFC) | ((bitGroups[1] >> 4) & 0x03);
            out[fullGroups*3 + 1] = ((bitGroups[1] << 4) & 0xF0) | ((bitGroups[2] >> 2) & 0x0F);
        }
    }
    return 0;
}

int decodeStrict(unsigned char* out, char* chars, unsigned long long length){
    return decodeSmart(out, chars, length, 0, 1);
}
int decode(unsigned char* out, char* chars, unsigned long long length){
    return decodeSmart(out, chars, length, 1, 0);
}

int encode(char* out, unsigned char* bytes, int length){

    int fullGroups = length / 3;

    for(int i = 0; i < fullGroups; i++){
        unsigned char bitsGroup[4] = {};
        // 0x3F = 0011 1111
        bitsGroup[0] = (bytes[i*3 + 0] >> 2) & 0x3F; 
        bitsGroup[1] = ((bytes[i*3 + 0] << 4) | (bytes[i*3 + 1] >> 4)) & 0x3F;
        bitsGroup[2] = ((bytes[i*3 + 1] << 2) | (bytes[i*3 + 2] >> 6)) & 0x3F;
        bitsGroup[3] = (bytes[i*3 + 2]) & 0x3F;

        for(int j = 0; j < 4; j++){
            out[i*4 + j] = charFrom6BitGroup(bitsGroup[j]);
        }
    }
    // 1 of 3 cases:
    // 1. no padding
    // 2. 1 char => 2 padding blocks
    // 3. 2 char => 1 padding block
    int extraBytes = length - (fullGroups * 3);
    // printf("# of extra bytes: %d\n",extraBytes);
    if(extraBytes == 0){

    } else if(extraBytes == 1){
        out[fullGroups*3 + 0] = charFrom6BitGroup((bytes[length - 1] >> 2) & 0x3F);
        out[fullGroups*3 + 1] = charFrom6BitGroup(((bytes[length - 1] << 4)) & 0x3F);
        out[fullGroups*3 + 2] = '=';
        out[fullGroups*3 + 3] = '=';
    } else if(extraBytes == 2){
        out[fullGroups*3 + 0] = charFrom6BitGroup((bytes[length - 2] >> 2) & 0x3F);
        out[fullGroups*3 + 1] = charFrom6BitGroup(((bytes[length - 2] << 4) | (bytes[length - 1] >> 4)) & 0x3F);
        out[fullGroups*3 + 2] = charFrom6BitGroup(((bytes[length - 1] << 2)) & 0x3F);
        out[fullGroups*3 + 3] = '=';
    } else {
        printf("Cap'in we have a problem.\n");
        return 1;
    }

    return 0;
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

    if(0){
        char encoded[5] = {0};
        unsigned char toEncode[] = "123";

        encode(encoded, toEncode, 3);
        printf("'123' encoded as '%s' should be 'MITz'.\n", encoded);
        
        unsigned char toEncode2[] = "abc";
        encode(encoded, toEncode2, 3);
        printf("'abc' encoded as '%s' should be 'YWJj'.\n", encoded);

        unsigned char toEncode3[] = "xy";
        encode(encoded, toEncode3, 2);
        printf("'xy'  encoded as '%s' should be 'eHK='.\n", encoded);

        unsigned char toEncode4[] = "!";
        encode(encoded, toEncode4, 1);
        printf("'!'   encoded as '%s' should be 'IQ=='.\n", encoded);

        char encoded2[100] = {0};
        unsigned char toEncode5[] = "Hello, World! It is a lovely day to day; would you say so..?";
        encode(encoded2, toEncode5, sizeof(toEncode5)-1);
        printf("      encoded as '%s' should be 'SGVsbG8sIFdvcmxkISBJdCBpcyBhIGxvdmVseSBkYXkgdG8gZGF5OyB3b3VsZCB5b3Ugc2F5IHNvLi4/'.\n", encoded2);
    }
    if(1){
        char encoded1[] = "YWJj";
        unsigned char decoded1[4] = {0};
        decode(decoded1, encoded1, 4);
        printf("decoded 'YWJj' to '%s', should be 'abc'.\n", decoded1);

        char encoded2[] = "YWJjYW==";
        unsigned char decoded2[5] = {0};
        decode(decoded2, encoded2, sizeof(encoded2)-1);
        printf("decoded 'YWJjYW==' to '%s', should be 'abca'.\n", decoded2);

        char encoded3[] = "YWJjYWJ=";
        unsigned char decoded3[6] = {0};
        decode(decoded3, encoded3, sizeof(encoded3)-1);
        printf("decoded 'YWJjYWJ=' to '%s', should be 'abcab'.\n", decoded3);

        char encoded4[] = "Y - - - - - - W - -- - -- - - ,. , ., .J   j,.,.,-,.,.,-()Y  W J=";
        unsigned char decoded4[6] = {0};
        decode(decoded4, encoded4, sizeof(encoded4)-1);
        printf("decoded '%s' to '%s', should be 'abcab'.\n", encoded4, decoded4);
    }

    return bad;
}