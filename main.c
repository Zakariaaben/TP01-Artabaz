#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "tovs.h"
#include "TOF.h"



int main(void) {
    char string[5]="few";
    char rest[5]="\0";

    strncpy(rest,"12", 2);
    strcat(string,rest);

    create_TOVS_file("./test_tovs.bin");

    TOVS_file file = open_TOVS_file("./test_tovs.bin");

    TOVS_block block;
    char line[1000];
    char line2[1000]="";
    strcpy(line, "0048,0,12345,Hello,FamilyName,15/11/68,algiers,skills#0048,0,12347,Hello,FamilyName,15/11/68,algiers,skills#0048,0,12349,Hello,FamilyName,15/11/68,algiers,skills#");



    strcpy(block.data, line);
    setHeader_TOVS(&file,2,106);
    write_TOVS_block(&file, 1, &block);

    setHeader_TOVS(&file,1,1);

    printf("Header  : %d blocks and the last pos : %d\n\n",file.header.number_of_blocks,file.header.last_character_position);

    int x,y;
    search_TOVS_record(file, 12349, (uint *) &x, (uint * )&y);

    printf("%d %d",x,y );









    return 0;
}
