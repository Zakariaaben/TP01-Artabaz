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

    complete_student_record record = {
        1234,
        "John",
        "Doe",
        "01/01/2000",
        "Paris4NWOB",
        1,
        "CJavaydiayShG",

    };
    char record_str[MAX_RECORD_LENGTH];



    int  bias[20] = {2,-6,-12,16,26,-4,6,18,14,-26,105, 12, 14, 16, 18, 20, 22, 24,-150,1};
    const int orginal = record.ID;
    for ( int i = 0  ; i<20; i++){
        record.ID = orginal + bias[i];
        insert_TOVS_record(&file, record);
        printf("Header : %d %d\n", getHeader_TOVS(file,1),getHeader_TOVS(file,2));

    }


    for (int i =1; i <= getHeader_TOVS(file,1); i++) {
        TOVS_block block = read_TOVS_block(file, i);
        printf("block %d : %.*s\n",i,i==getHeader_TOVS(file,1)?getHeader_TOVS(file,2)+1:MAX_CHAR_BLOCK_TOVS,block.data);
    }




    return 0;
}
