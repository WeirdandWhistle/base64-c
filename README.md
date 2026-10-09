# base64-c
Variouse Base64 Funcition including encoding/decoding in c. And also maybe HEX.

# How To use:
Just copy the source code into your project.  
 - `Base64.c`
 - `Base64.h`
   
Or if you have a libary setup file use 

```bash
curl -Z -OLS https://raw.githubusercontent.com/WeirdandWhistle/base64-c/refs/heads/main/Base64.c \
    https://raw.githubusercontent.com/WeirdandWhistle/base64-c/refs/heads/main/Base64.h \
```  
No need for credits or anything. Just use the code.  

### What's the API?
You have *4* main functions
1. `int bin2base64(char* base64, unsigned char* bin, int bin_length, int doPadding, int varient);` - takes a byte array (`unsigned char* bin`) of `bin_length` and writes the string into (`char* base64`). Adds padding if `doPadding` is true (`if(doPadding != 0)`). 
2. `int base642bin(unsigned char* bin, char* base64, unsigned long long base64_length, int varient);` - takes a char array encoded in base64 (`char* base64`) of length `base64_length` and writes the bin/binary array into `unsigned char* bin`.  
3. `int get_base64_length_from_bin(unsigned long long* base64_length, unsigned long long bin_length, int padding);` - take byte array length (`unsigned long long bin_length`) and padding constriants and writes a the string length **WIHTOUT A TRAILING NULL BYTE** into `unsigned long long* base64_length`.
4. `int get_bin_length_from_base64(unsigned long long* bin_length, char* base64, unsigned long long base64_length);` - takes a string (`char* base64`) with length `unsigned long long base64_length` and writes how big the bin/byte array will be into `unsigned long long* bin_length`.

for the argument `int varient` found in both the encode and decode functions you can set it to any of the followig:   
* `VARIENT_BASE64_NORMAL` your normal default base64.
* `VARIENT_BASE64_URL` changes the alphebet to be both URL and FILESYSTEM safe.
* `VARIENT_BASE64_FILESAFE` = `VARIENT_BASE64_URL`
* `VARIENT_BASE64_EITHER` = `VARIENT_BASE64_NORMAL`

> all reference in docs (and code) to strings **DO NOT** inlcude a trailing `NULL` byte. They are ASCII strings NOT C string.

Remember to include the header file to use the functions `#include "Base64.h"`

# Demo the code:
```bash
git clone https://github.com/WeirdandWhistle/base64-c.git --depth 1 \
cd base64-c \
make \
./main test
```
