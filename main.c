#include <stdio.h>
#include "Base64.h"
#include <string.h>


int main(int argc, char* argv[]){
    int doTest = 0;
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "test") == 0) doTest = 1;
    }

    if(doTest) test();

    return 0;
}