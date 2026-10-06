#ifndef BASE_64_HEADER
#define BASE_64_HEADER

    char charFrom6BitGroup(unsigned char bits);
    unsigned char bitGroupFromChar(char encoded);

    // retuns 0 if fine otherwise 1
    int test();

#endif