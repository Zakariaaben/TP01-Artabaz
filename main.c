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

    const complete_student_record record = {
        1234,
        "John",
        "Doe",
        "01/01/2000",
        "Paris4NWOB",
        1,
        "C,JavaFESABGSAG"

    };
    insert_TOVS_record(&file, record);




    return 0;
}
