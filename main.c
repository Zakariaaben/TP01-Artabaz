#include <stdio.h>
#include <string.h>

#include "TOF.h"

int main(void) {
    if (!create_TOF_file("test12.txt")) {
        printf("Error creating TOF file\n");
    }

    TOF_file file = open_TOF_file("test12.txt");

    if (!file.file) {
        printf("Error opening TOF file\n");
        return 1;
    }

    const student_record record={123,"jhon","mohamed","15/11/2021","Century"};
    const student_record record2={124,"jhon","mohamed","15/11/2021","Century"};
    const student_record record3={125,"jhon","mohamed","15/11/2021","Century"};
    TOF_block block;
    block.records[0] = record;
    block.records[1] = record2;
    block.records[2] = record3;
    block.nb_records = 3;

    if(!write_TOF_block(&file, block, 1)) {
        return 1;
    }

    int i,j;

    search_TOF_record(file, 128, &i, &j);

    printf("%d %d\n", i, j);






    return 0;
}
