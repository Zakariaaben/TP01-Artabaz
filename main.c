#include  <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TOF.h"
#include "tovs.h"



void printMenu() {
    printf("1. Create TOF file\n");
    printf("2. Load TOF file with students_data_1a.csv\n");
    printf("3. Create TOVS file\n");
    printf("4. Expand TOF to TOVS\n");
    printf("5. Print TOF file\n");
    printf("6. Print TOF Header\n");
    printf("7. Print TOVS file\n");
    printf("8. Print TOVS Header\n");
    printf("9. Print file of TOF Insertion cost\n");
    printf("10. Print file of TOVS expansion cost\n");
    printf("11. Delete Records for csv file\n");
    printf("12. Print Loading Factor\n");
    printf("13. Exit\n");
}

int main(void) {
    const char  *   tof_filename = "tof_file.bin";
    const char  *  tovs_filename = "tovs_file.bin";

    bool endMenu = false;
    int made[10]= {0,0,0,0,0,0,0,0,0,0};



    while (!endMenu) {
        system("@cls||clear");

        printMenu();
        printf("Enter your choice:  ");

        int choice;
        scanf("%d", &choice);

        int  * validatedInsertions ;

        switch (choice) {
            case 1: // 1. Create TOF file

                if (made[0] == 0) {
                    const bool created = create_TOF_file(tof_filename);
                    printf("TOF File created Successfully\n");
                    made[0] = 1;

                } else {
                    printf("TOF file already created\n");
                }
            system("pause");
                break;
            case 2: // 2. Load TOF file with students_data_1a.csv\n
                if (made[0] == 0) {
                    printf("TOF file not created yet.\n");

                } else if (made[1] == 0) {
                    TOF_file tof_file = open_TOF_file(tof_filename);
                    validatedInsertions = load_TOF_file_from_csv("../files/students_data_1a.csv", &tof_file);
                    printf("Number of records inserted: %d\n", validatedInsertions[0]);
                    printf("Number of records not inserted: %d\n", validatedInsertions[1]);
                    close_tof_file(&tof_file);
                    made[1] = 1;
                } else {
                    printf("TOF file already loaded from csv data.\n");
                }
                system("pause");
                break;
            case 3:
                if (made[2] == 0) {
                    create_TOVS_file("tovs_file.bin");
                    made[2] = 1;
                } else {
                    printf("TOVS file already created\n");
                }
                system("pause");
                break;
            case 4: // 4. Expand TOF to TOVS
                if (made[1] == 0 ) {
                    printf("TOF not loaded or not created yet \n");
                }else if (made[2] == 0) {
                    printf("TOVS file not created yet\n");
                }else if (made[3] == 0) {
                    TOVS_file tovs_file = open_TOVS_file(tovs_filename);
                    TOF_file const  tof_file = open_TOF_file(tof_filename);
                    expand_TOF_to_TOVS("../files/students_data_1a.csv", tof_file, &tovs_file);
                    close_TOVS_file(tovs_file);
                    made[3] = 1;
                } else {
                    printf("TOF file is already expanded\n");
                }
                system("pause");
                break;
            case 5 : // 5. Print TOF file
                if (made[0] == 0) {
                    printf("TOF file not created yet\n");
                } else {
                    TOF_file  tof_file = open_TOF_file(tof_filename);
                    print_TOF_file(tof_file);
                    close_tof_file(&tof_file);
                }
            system("pause");

                break;
            case 6: // 6. Print TOF Header

                if (made[0] == 0) {
                    printf("TOF file not created yet\n");
                } else {
                    TOF_file tof_file = open_TOF_file(tof_filename);
                    print_TOF_header(tof_file);
                    close_tof_file(&tof_file);
                }
                system("pause");
                break;
            case 7:
                if (made[3] == 0) {
                    printf("TOVS file not created yet or not expanded\n");
                } else {
                    TOVS_file const tovs_file = open_TOVS_file(tovs_filename);
                    print_TOVS_file(tovs_file);
                    close_TOVS_file(tovs_file);
                }
                system("pause");
                break;
            case 8:
                if (made[2] == 0) {
                    printf("TOVS file not created yet or not expanded\n");
                } else {
                    const TOVS_file  tovs_file = open_TOVS_file(tovs_filename);
                    print_TOVS_header(tovs_file);
                    close_TOVS_file(tovs_file);
                }
                system("pause");
                break;
            case 9:
                if (made[1] == 0) {
                    printf("TOF file not loaded yet\n");
                } else {
                    FILE *tof_insertion_cost_file = fopen("tof_insertion_cost.csv", "r");
                    if (tof_insertion_cost_file == NULL) {
                        printf("Error while opening file %s\n", strerror(errno));
                    } else {
                        char line[255];
                        while (fgets(line, sizeof(line), tof_insertion_cost_file)) {
                            printf("%s", line);
                        }
                        fclose(tof_insertion_cost_file);
                    }
                }
            system("pause");

                break;
            case 10:
                if (made[3] == 0) {
                    printf("TOVS file not created yet or not expanded\n");
                } else {
                    FILE *tovs_expansion_cost_file = fopen("tovs_expansion_cost.csv", "r");
                    if (tovs_expansion_cost_file == NULL) {
                        printf("Error while opening file %s\n", strerror(errno));
                    } else {
                        char line[255];
                        while (fgets(line, sizeof(line), tovs_expansion_cost_file)) {
                            printf("%s", line);
                        }
                        fclose(tovs_expansion_cost_file);
                    }
                }
                system("pause");
                break;
            case 11:
                if (made[1]==1) {
                    TOF_file tof_file = open_TOF_file(tof_filename);
                    delete_TOF_records_from_csv("../files/delete_students.csv", &tof_file);
                    printf("\n");
                    printf("Number of deleted records : %d", getHeader(tof_file, 3));
                    close_tof_file(&tof_file);
                }else {
                    printf("The TOF file has to be loaded first\n");
                }
                system("pause");
                break;
            case 12: // print loading factor
                if (made[1]==1) {
                    const TOF_file tof_file = open_TOF_file(tof_filename);
                    printf("Loading factor of the TOF file is %f\n", ((float) getHeader(tof_file, 2) - (float)getHeader(tof_file,3)) / ((float)(getHeader(tof_file, 1) * TOF_RECORDS_NUMBER)));
                }else {
                    printf("TOF file has to be loaded first\n");
                }
                system("pause");
                break;
            case 13:
                endMenu = true;
                system("pause");
                break;
            default:
                printf("Invalid option\n");
                system("pause");

                break;

        }
    }
}


