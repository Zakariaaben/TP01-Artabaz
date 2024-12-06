#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TOF.h"


int main(void) {
    student_record record;
    FILE * file = fopen("files/students_data_1a.csv","r");

    if (file == NULL) {
        printf("Error while opening file %s\n", strerror(errno));
        return 1;
    }

    create_TOF_file("./students_data_1.tof");

    TOF_file tof_file = open_TOF_file("./students_data_1.tof");

    const int  *res = load_TOF_file_csv("files/students_data_1a.csv", &tof_file);

    printf("%d %d\n",res[0],res[1]);

    print_TOF_header(tof_file);

    close_tof_file(&tof_file);

    return 0;
}
