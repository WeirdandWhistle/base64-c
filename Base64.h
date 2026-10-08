#ifndef BASE_64_HEADER
#define BASE_64_HEADER

    char charFrom6BitGroup(unsigned char bits);
    unsigned char bitGroupFromChar(char encoded);
    
    int encode(char* out, unsigned char* bytes, int length);
    int decode(unsigned char* out, char* bytes, unsigned long long length);
    int decodeStrict(unsigned char* out, char* bytes, unsigned long long length);

    int encodedStringLength(unsigned long long* outLength, char* chars, unsigned long long length);

    // retuns 0 if fine otherwise 1
    int test();

#endif