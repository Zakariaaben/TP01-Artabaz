// tovc.h
#ifndef TOVS_H
#define TOVS_H

#include <stdio.h>

#include <stdbool.h>

#include  "constants.h"

#include "TOF.h"


#define MAX_CHAR_BLOCK_TOVS 60 // Number of characters in a record
#define RECORD_SEPARATOR_TOVS "#" // Special character indicating the end of data
#define FIELD_SEPARATOR_TOVS ","
#define MAX_RECORD_LENGTH 120

// Structure definitions
typedef struct TOVS_block
{
    char data[MAX_CHAR_BLOCK_TOVS];
} TOVS_block;

typedef struct TOVS_header
{
    int number_of_blocks; // Number of the last block in the file
    int last_character_position;
    int number_of_inserted_characters;       // Number of deleted characters
    int number_of_deleted_characters;       // Number of inserted records
} TOVS_header;

typedef struct TOVS_file
{
    FILE *file; // File pointer
    TOVS_header header; // File header
} TOVS_file;



// Function prototypes
bool create_TOVS_file(const char *filename);

TOVS_file open_TOVS_file(const char * filename);

int getHeader_TOVS(const TOVS_file file,const int field);

void setHeader_TOVS(TOVS_file * file, const int field,const int value);

TOVS_block read_TOVS_block(const TOVS_file file,const  int block_number);

bool write_TOVS_block(const TOVS_file *file,const int block_number,const TOVS_block * block);

void print_TOVS_header(const TOVS_file file);

bool search_TOVS_record(TOVS_file file, const uint ID, uint *  block_number, uint * char_pos );

bool insert_TOVS_record(TOVS_file *file, const complete_student_record record);

void print_TOVS_file(const TOVS_file file);

bool parse_student_additional_info(const char *line, student_record *record);

int * expand_TOF_to_TOVS(const char * csv_filename, TOF_file  tof_file);

char *convert_full_record_to_string(const complete_student_record record);

bool parse_complete_student_record(const char *line, complete_student_record *record);

bool parse_additional_info(const char * line, student_additional_info * record);

complete_student_record convert_string_to_full_record(char * string);




int close_TOVS_file(TOVS_file file );

#endif // TOVS_H