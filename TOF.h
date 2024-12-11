#ifndef TOF_H
#define TOF_H

#include <stdio.h>
#include <stdbool.h>
#include "constants.h"



#define TOF_LOADING_FACTOR  0.7 // Number between 0 and 1
#define TOF_RECORDS_NUMBER  100



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




bool create_TOF_file(const char * file_name);

TOF_file open_TOF_file(const char * file_name);

int getHeader(const TOF_file file,const  int property);

void setHeader(TOF_file *file, const int property, const int value);

TOF_block read_TOF_block(const TOF_file file,uint block_number);

bool write_TOF_block(TOF_file *file, const TOF_block block, const uint block_number);

void print_TOF_block(const TOF_block block);

void print_TOF_header(const TOF_file file);

void print_student_record(const student_record record);

bool search_TOF_record(const TOF_file file, const uint ID, int *block_pos,int *record_pos,cost * cost);

bool insert_TOF_record(TOF_file *file, const student_record record, cost * cost);

void print_TOF_file(const TOF_file file);

bool parse_student_record(const char *line, student_record *record);

int * load_TOF_file_from_csv(const char * csv_filename, TOF_file * tof_file);

void close_tof_file(const TOF_file *file);

int * delete_TOF_records_from_csv(const char * csv_filename,TOF_file *file);

int TOF_fragmentation(const TOF_file file);


#endif //TOF_H
