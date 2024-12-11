// tovc.h
#ifndef TOVS_H
#define TOVS_H

#include <stdio.h>

#include <stdbool.h>

#include  "constants.h"

#include "TOF.h"


#define MAX_CHAR_BLOCK_TOVS 1000 // Number of characters in a record
#define RECORD_SEPARATOR_TOVS '#' // Special character indicating the end of data
#define FIELD_SEPARATOR_TOVS ','

// Structure definitions
typedef struct TOVS_block
{
    char data[MAX_CHAR_BLOCK_TOVS];
} TOVS_block;

typedef struct TOVS_header
{
    int number_of_blocks; // Number of the last block in the file
    int number_of_records;  // Total number of records
    int number_of_inserted_characters;       // Number of deleted characters
    int number_of_deleted_characters;       // Number of inserted records
} TOVS_header;

typedef struct TOVS_file
{
    FILE *file; // File pointer
    TOVS_header header; // File header
} TOVS_file;



// Function prototypes
bool create_TOVS_file(char *filename);

TOVS_file open_TOVS_file(TOVS_file * file);

int getHeader_TOVS(const TOVS_file file,const int field);

void setHeader_TOVS(const TOVS_file * file, int field, int value);

TOVS_block read_TOVS_block(const TOVS_file file,const  int block_number);

bool write_TOVS_block(TOVS_file *file,const int block_number, TOVS_block * block);

void print_TOF_header(const TOVS_file file);

bool search_TOF_record(const TOVS_file file, const uint ID, int *block_pos,int *record_pos,cost * cost);

bool insert_TOF_record(TOVS_file *file, const complete_student_record record, cost * cost);

void print_TOVS_file(const TOVS_file file);

bool parse_student_additional_info(const char *line, student_record *record);

int * expand_TOF_to_TOVS(const char * csv_filename, TOF_file  tof_file);

int * load_TOF_file_from_csv(const char * csv_filename, TOF_file tof_file, TOVS_file *tovs_file);

int close_TOVS_file(TOVS_file *file);

#endif // TOVS_H