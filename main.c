#include <stdio.h>
#include "TOF.h"

int main(void) {

    if(!create_TOF_file("test.txt")) {
        printf("Error creating TOF file\n");
    }


    TOF_file file = open_TOF_file("test.txt");

    printf("Nblk : %d\n",getHeader(file,1));

    return 0;
}
