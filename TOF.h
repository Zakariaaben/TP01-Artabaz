#ifndef TOF_H
#define TOF_H

#include <stdio.h>

typedef unsigned int uint;

#define MAX_NAME_LENGTH  25
#define MAX_FAMILY_NAME_LENGTH 25
#define MAX_CITY_NAME_LENGTH 25
#define MAX_ACQUIRED_SKILLS_LENGTH 255
#define TOF_LOADING_FACTOR  0.7 // Number between 0 and 1
#define TOF_RECORDS_NUMBER  10



typedef struct student_record {
    uint ID;
    char name[MAX_NAME_LENGTH];
    char family_name[MAX_FAMILY_NAME_LENGTH];
    char date_of_birth[10];
    char city_of_birth[MAX_CITY_NAME_LENGTH];
} student_record;

typedef struct student_additional_info {
    uint ID, year_of_study;
    char acquired_skills[MAX_ACQUIRED_SKILLS_LENGTH];
} student_additional_info;

typedef struct TOF_header {
    int
    number_of_blocks;
    int
    number_of_records;
} TOF_header;

typedef struct TOF_block {
    student_record records[TOF_RECORDS_NUMBER];
    int nb_records;
}TOF_block;

typedef struct TOF_file {
    FILE * file;
    TOF_header header;
}TOF_file;

int create_TOF_file(const char * file_name);

TOF_file open_TOF_file(const char * file_name);

int getHeader(const TOF_file file,const  int property);

TOF_block read_TOF_block(const TOF_file file,uint block_number);




#endif //TOF_H
