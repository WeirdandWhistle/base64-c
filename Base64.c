#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Base64.h"

void print_hex(unsigned char* in, unsigned long long len){
    for(unsigned long long i = 0; i < len; i++){
        printf("%02x ", in[i]);
    }
    printf("\n");
}

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

int get_base64_length_from_bin(unsigned long long* base64_length, unsigned long long bin_length, int padding){
    unsigned long long temp = (bin_length / 3) * 4; 
    if(bin_length % 3 != 0){
        if(padding) temp += 4;
        else {
            if(bin_length % 3 == 1) temp += 2;
            if(bin_length % 3 == 2) temp += 3;
        }
    }
    *base64_length = temp;
    return 0;
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

int get_bin_length_from_base64(unsigned long long* bin_length, char* base64, unsigned long long base64_length){
    encodedStringLength(&base64_length, base64, base64_length);
    // printf("base64 string len: %lld\n", base64_length);
    unsigned long long temp = (base64_length / 4);
    temp *= 3;
    if(base64_length % 4 == 0){
        *bin_length = temp;
        return 0;
    }
    if(base64_length % 4 == 2) temp += 1;
    else if(base64_length % 4 == 3) temp += 2;
    else return 1;
    *bin_length = temp;
    return 0;
}

int decodeSmart(unsigned char* out, char* chars, unsigned long long length, int automaticPadding, int enforeAcceptedAlphabet, int logging){
    unsigned long long stringLength = 0;
    if(encodedStringLength(&stringLength, chars, length)){
        if(logging) printf("Finding string length failed for some reason...\n");
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
            if(logging) printf("Base 64 decoding failed! Padding is wrong! Not a multible of 4.\n");
            return 1;
        }   
    }

    unsigned long long charsRead = 0;

    // printf("fullGroup: %lld, stringLen: %lld\n",fullGroups, stringLength);

    unsigned char bitGroups[4] = {0};
    unsigned long long readOffset = 0;
    for(unsigned long long i = 0; i < fullGroups; i++){
        for(int j = 0; j < 4; j++){
            bitGroups[j] = bitGroupFromChar(chars[i*4 + j + readOffset]);
            if(bitGroups[j] == 255) {
                if(enforeAcceptedAlphabet) {
                    if(logging) printf("Base64 decoding failed! Char not in alphabet!\n");
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
            if(logging) printf("Read more chars than in the base64 string. shouldn't be possible.\n");
            return 1;
        }
        int difRead = stringLength - charsRead; 
        if(difRead != 2 && difRead != 3){
            if(logging) printf("Read %d chars under the string length. This shouldn't happen. charsRead: %lld, stringLength: %lld, fullGroups: %lld\n", difRead, charsRead, stringLength, fullGroups);
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
                        if(logging) printf("Base64 decoding failed! Char not in alphabet!\n");
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
                        if(logging) printf("Base64 decoding failed! Char not in alphabet!\n");
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

int base642bin(unsigned char* bin, char* base64, unsigned long long base64_length){
    return decodeSmart(bin, base64, base64_length, 1, 0, 0);
}

int bin2base64(char* base64, unsigned char* bin, int bin_length, int doPadding){

    int fullGroups = bin_length / 3;

    for(int i = 0; i < fullGroups; i++){
        unsigned char bitsGroup[4] = {};
        // 0x3F = 0011 1111
        bitsGroup[0] = (bin[i*3 + 0] >> 2) & 0x3F; 
        bitsGroup[1] = ((bin[i*3 + 0] << 4) | (bin[i*3 + 1] >> 4)) & 0x3F;
        bitsGroup[2] = ((bin[i*3 + 1] << 2) | (bin[i*3 + 2] >> 6)) & 0x3F;
        bitsGroup[3] = (bin[i*3 + 2]) & 0x3F;

        for(int j = 0; j < 4; j++){
            base64[i*4 + j] = charFrom6BitGroup(bitsGroup[j]);
        }
    }
    // 1 of 3 cases:
    // 1. no padding
    // 2. 1 char => 2 padding blocks
    // 3. 2 char => 1 padding block
    int extraBytes = bin_length - (fullGroups * 3);
    // printf("# of extra bytes: %d\n",extraBytes);
    if(extraBytes == 0){

    } else if(extraBytes == 1){
        base64[fullGroups*4 + 0] = charFrom6BitGroup((bin[bin_length - 1] >> 2) & 0x3F);
        base64[fullGroups*4 + 1] = charFrom6BitGroup(((bin[bin_length - 1] << 4)) & 0x3F);
        if(doPadding){
            base64[fullGroups*4 + 2] = '=';
            base64[fullGroups*4 + 3] = '=';
        }
        
    } else if(extraBytes == 2){
        base64[fullGroups*4 + 0] = charFrom6BitGroup((bin[bin_length - 2] >> 2) & 0x3F);
        base64[fullGroups*4 + 1] = charFrom6BitGroup(((bin[bin_length - 2] << 4) | (bin[bin_length - 1] >> 4)) & 0x3F);
        base64[fullGroups*4 + 2] = charFrom6BitGroup(((bin[bin_length - 1] << 2)) & 0x3F);
        if(doPadding){
            base64[fullGroups*4 + 3] = '=';
        }
    } else {
        printf("Cap'in we have a problem.\n");
        return 1;
    }

    return 0;
}

int test(){
    printf("======================\n");
    printf("Nothing should return here (if it does smth wrong):\n");
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

    if(1){
        printf("======================\n");
        char encoded[5] = {0};
        unsigned char toEncode[] = "123";

        bin2base64(encoded, toEncode, 3, 1);
        printf("'123' encoded as '%s' should be 'MITz'.\n", encoded);
        
        unsigned char toEncode2[] = "abc";
        bin2base64(encoded, toEncode2, 3, 1);
        printf("'abc' encoded as '%s' should be 'YWJj'.\n", encoded);

        unsigned char toEncode3[] = "xy";
        bin2base64(encoded, toEncode3, 2, 1);
        printf("'xy'  encoded as '%s' should be 'eHK='.\n", encoded);

        memset(encoded, 0, sizeof(encoded));
        unsigned char toEncode4[] = "!";
        bin2base64(encoded, toEncode4, 1, 0);
        printf("'!'   encoded as '%s' should be 'IQ'. (padding is turned off)\n", encoded);

        char encoded2[100] = {0};
        unsigned char toEncode5[] = "Hello, World! It is a lovely day to day; would you say so..?";
        bin2base64(encoded2, toEncode5, sizeof(toEncode5)-1, 1);
        printf("      encoded as '%s' should be 'SGVsbG8sIFdvcmxkISBJdCBpcyBhIGxvdmVseSBkYXkgdG8gZGF5OyB3b3VsZCB5b3Ugc2F5IHNvLi4/'\n", encoded2);
    }
    if(1){
        printf("======================\n");

        char encoded1[] = "YWJj";
        unsigned char decoded1[4] = {0};
        base642bin(decoded1, encoded1, 4);
        printf("decoded 'YWJj' to '%s', should be 'abc'.\n", decoded1);

        char encoded2[] = "YWJjYW==";
        unsigned char decoded2[5] = {0};
        base642bin(decoded2, encoded2, sizeof(encoded2)-1);
        printf("decoded 'YWJjYW==' to '%s', should be 'abca'.\n", decoded2);

        char encoded3[] = "YWJjYWJ=";
        unsigned char decoded3[6] = {0};
        base642bin(decoded3, encoded3, sizeof(encoded3)-1);
        printf("decoded 'YWJjYWJ=' to '%s', should be 'abcab'.\n", decoded3);

        char encoded4[] = "Y - - - - - - W - -- - -- - - ,. , ., .J   j,.,.,-,.,.,-()Y  W J=";
        unsigned char decoded4[6] = {0};
        base642bin(decoded4, encoded4, sizeof(encoded4)-1);
        printf("decoded '%s' to '%s', should be 'abcab'.\n", encoded4, decoded4);

        char encoded5[] = "IQ";
        unsigned char decoded5[6] = {0};
        base642bin(decoded5, encoded5, sizeof(encoded5)-1);
        printf("decoded '%s' to '%s', should be '!'.\n", encoded5, decoded5);

        char encoded6[] = "I";
        unsigned char decoded6[6] = {0};
        base642bin(decoded6, encoded6, sizeof(encoded6)-1);
        printf("decoded '%s' to '%s', should be ''.\n", encoded6, decoded6);
    }
    if(1){ // automated random testing
        printf("======================\n");

        srand(time(NULL));
        unsigned long long length = (rand() % 100) + 50;
        unsigned char* arr = malloc(length);
        
        unsigned long long charArrLength = -1;
        get_base64_length_from_bin(&charArrLength, length, 1);
        char* charArr = malloc(charArrLength+1);
        charArr[charArrLength] = 0;
        if(arr == NULL) goto exit_if;
        if(charArr == NULL) goto exit_if;
        

        // randomize data
        for(unsigned long long i = 0; i < length; i++){
            arr[i] = rand();
        }

        // generate char arr (aka encode data)
        bin2base64(charArr, arr, length, 1);

        // printf("decoding data in question: %s\n", charArr);

        // decode data
        unsigned long long arrCheckLength = -1;
        get_bin_length_from_base64(&arrCheckLength, charArr, charArrLength);
        // printf("length rc: %d\n", rc);
        if(arrCheckLength != length) {
            printf("OOOHHHHHH.... thats such a simple fix! len: %lld; arrCheckLength: %lld \n", length, arrCheckLength);
        }
        unsigned char* arrCheck = malloc(arrCheckLength);
        if(arrCheck == NULL) goto exit_if;

        base642bin(arrCheck, charArr, charArrLength);

        // make sure its the same
        int good = 1;
        for(unsigned long long i = 0; i < arrCheckLength; i++){
            if(arr[i] != arrCheck[i]) {
                printf("broke at index %lld!\n", i);
                good = 0;
                break;
            }
        }

        // printf("Dumping start: "); print_hex(arr, length);
        // printf("Dumping end  : "); print_hex(arrCheck, arrCheckLength);

        if(!good) printf("Not good! Automated random encoder/decoder test failed.\n");
        else printf("Good! Automated random encoder/decoder test worked.\n");


        exit_if:
        free(arr);
        free(charArr);

    }

    printf("======================\n");

    return bad;
}