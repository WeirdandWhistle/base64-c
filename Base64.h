#ifndef BASE_64_HEADER
#define BASE_64_HEADER

    char charFrom6BitGroup(unsigned char bits);
    unsigned char bitGroupFromChar(char encoded);
    
    int encode(char* out, unsigned char* bytes, int length);
    int decode(unsigned char* out, char* bytes, unsigned long long length);
    int decodeStrict(unsigned char* out, char* bytes, unsigned long long length);

    int encodedStringLength(unsigned long long* outLength, char* chars, unsigned long long length);

    int get_base64_length_from_bin(unsigned long long* base64_length, unsigned long long bin_length, int padding);
    int get_bin_length_from_base64(unsigned long long* bin_length, char* base64, unsigned long long base64_length);

    // retuns 0 if fine otherwise 1
    int test();

#endif