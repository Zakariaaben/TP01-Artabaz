#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "tovs.h"
#include "TOF.h"
#include "csv.h"
bool parse_additional_info_v2(const char *line, student_additional_info *record);

int main(void) {




    create_TOVS_file("./test_tovs.bin");

    TOVS_file tovs_file = open_TOVS_file("./test_tovs.bin");

    create_TOF_file("./test_tof.bin");
    TOF_file tof_file = open_TOF_file("./test_tof.bin");

    load_TOF_file_from_csv("./files/test-file.csv",&tof_file);

    print_TOF_header(tof_file);

    // complete_student_record record  = convert_string_to_full_record("0,18766,Amine,Hamdi,22/06/2003,Tizi Ouzou,5,\"Dynamic Structures, C++, Web Development\"")';
    // insert_TOVS_record(&tovs_file, record);
    // record




    expand_TOF_to_TOVS("./files/test-file2.csv",tof_file,&tovs_file);







    close_TOVS_file(tovs_file);
    close_tof_file(&tof_file);

    return 0;
}


