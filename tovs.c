#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TOF.h"
#include "tovs.h"

// Function to initialize the file's header
bool create_TOVS_file(const char *filename)
{
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        printf("Error creating TOVS file.\n");
        return false;
    }

    const TOVS_header header = {0, 0, 0,0};
    fseek(file,0,SEEK_SET);
    fwrite(&header, sizeof(TOVS_header), 1, file);
    fclose(file);
    printf("TOVS file Created.\n");

    return true;
}


// Open or create a TOVC file
TOVS_file open_TOVS_file(const char * filename)
{

    TOVS_file file;
    TOVS_header header = {0, 0,0,0};

    file.file = fopen(filename, "rb+");

    fseek(file.file, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, file.file);
    file.header = header;

    if (file.file == NULL) {
        printf("Error while opening file %s\n", strerror(errno));
    }

    return file;
}

// Close a TOVC file
int close_TOVS_file(const TOVS_file file)
{
    rewind(file.file);
    fwrite(&file.header, sizeof(TOVS_header), 1, file.file); // Update the header
    fclose(file.file);
    return 0;
}

// Get a specific header field value
int getHeader_TOVS(const TOVS_file file,const int field)
{
    switch (field)
    {
    case 1:
        return file.header.number_of_blocks;
    case 2:
        return file.header.last_character_position;
    case 3:
        return file.header.number_of_inserted_characters;
    case 4:
        return file.header.number_of_deleted_characters;
    default:
        return -1; // Invalid field
    }
}

// Set a specific header field value
void setHeader_TOVS(TOVS_file * file, const int field,const int value)
{
    switch (field)
    {
    case 1:
        file->header.number_of_blocks = value;
        break;
    case 2:
        file->header.last_character_position = value;
        break;
    case 3:
        file->header.number_of_inserted_characters = value;
        break;
    case 4:
        file->header.number_of_deleted_characters = value;
        break;
    default:
        printf("Error setting header, Invalid number of field \n");
        break;// Invalid field
    }
}


// Read a block from the file
// Read a block from the file
TOVS_block read_TOVS_block(const TOVS_file file,const  int block_number)
{
    TOVS_block block;
    // Retrieve the last block number from the header
    int lastBlockNum = getHeader_TOVS(file,1);
    if (block_number > 0 && block_number <= lastBlockNum)
    { // Ensure blockNum is within range
        // Seek to the block's position
        fseek(file.file, sizeof(TOVS_header) + (block_number - 1) * sizeof(TOVS_block), SEEK_SET);
        // Read the block from the file
        fread(&block, sizeof(TOVS_block), 1, file.file) ;
        return block;
    }
    printf("Error while reading TOVS block , Invalid block number");
    return block;
}


// Write a block to the file
bool write_TOVS_block(const TOVS_file *file,const int block_number,const TOVS_block * block)
{
    // Validate block number
    const int lastBlockNum = getHeader_TOVS(*file, 1);
    if (block_number <= 0 || block_number>lastBlockNum+1)
    {
        return false; // Invalid block number
    }

    fseek(file->file,sizeof(TOVS_header)+sizeof(TOVS_block)*(block_number-1),SEEK_SET);
    fwrite(block,sizeof(TOVS_block),1,file->file);

    return true; // Success
}

// Converts TOVS record to string of format isDeleted,ID,name,familyname,date,city,year,skills
char *convert_full_record_to_string(const complete_student_record record) {
    // Allocate enough memory for the string
    char *string_record = malloc(MAX_RECORD_LENGTH);
    if (!string_record) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Initialize string to empty
    string_record[0] = '\0';

    // Temporary buffers for numbers
    char ID[20], year_of_study[20];

    // Convert numeric fields to strings
    sprintf(ID, "%d", record.ID);
    sprintf(year_of_study, "%d", record.year_of_study);

    // Concatenate all fields into the string
    strcat(string_record,record.is_deleted? "1,": "0,");

    strcat(string_record, ID);

    strcat(string_record, ",");
    strcat(string_record, record.name);

    strcat(string_record, ",");
    strcat(string_record, record.family_name);

    strcat(string_record, ",");
    strcat(string_record, record.date_of_birth);

    strcat(string_record, ",");
    strcat(string_record, record.city_of_birth);

    strcat(string_record, ",");
    strcat(string_record, record.year_of_study==-1?"":year_of_study);

    strcat(string_record, ",");
    strcat(string_record, record.acquired_skills);
    // add \0 at the end of the string
    strcat(string_record,"\0");
    return string_record;
}


// Parsing student record in format isDeleted,ID,name,family_name,date,city,year,skills
bool parse_complete_student_record(const char *line, complete_student_record *record) {
    const char *start = line;
    int fieldIndex = 0;
    while (fieldIndex < 8 ) {
        const char *end = strchr(start, ',');
        if (end == NULL) {
            end = start + strlen(start);
        }

        const size_t length = end - start;

        switch (fieldIndex) {
            case 0:
                char isDeleted[1];
                strncpy(isDeleted, start, length);
                record->is_deleted = atoi(isDeleted);
                break;
            case 1:
                char ID[20];
                strncpy(ID, start, length);
                if (atoi(ID) == 0) {
                    return false;
                }


                record->ID = atoi(ID);

            case 2:
                strncpy(record->name, start, length);
            record->name[length] = '\0';
            break;

            case 3:
                strncpy(record->family_name, start, length);
            record->family_name[length] = '\0';
            break;

            case 4:
                strncpy(record->date_of_birth, start, length);

            record->date_of_birth[length] = '\0';
            break;
            case 5 :
                strncpy(record->city_of_birth, start, length);
                record->city_of_birth[length] = '\0';
                break;

            case 6 :
                char year_of_study[5];
                strncpy(year_of_study, start, length);
                record->year_of_study = atoi(year_of_study);

            case 7:
                strncpy(record->acquired_skills, start, length);
                record->acquired_skills[length] = '\0';
                break;

            default:
                break;
        }

        // Move to the next field
        if (*end == ',') {
            start = end + 1;
        } else {
            break; // End of line reached
        }
        fieldIndex++;
    }
    return true;
}

bool parse_additional_info(const char * line, student_additional_info * record) {

    const char *start = line;
    int fieldIndex = 0;
    while (fieldIndex < 3 ) {
        const char *end = strchr(start, ',');
        if (end == NULL) {
            end = start + strlen(start);
        }

        const size_t length = end - start;

        switch (fieldIndex) {
            case 0:
                char ID[20];
                strncpy(ID, start, length);
                ID[length] = '\0';
                if (atoi(ID) == 0) {
                    return false;
                }

                record->ID = atoi(ID);
                break;
            case 1:
                char year_of_study[5];
                strncpy(year_of_study, start, length);
                year_of_study[length] = '\0';
                if (atoi(year_of_study)==0 ) {
                    record->year_of_study = -1;
                    break;
                }
                record->year_of_study = atoi(year_of_study);
            case 2:
                strncpy(record->acquired_skills, start, length);
                record->acquired_skills[length] = '\0';
                break;
            default:
                break;
        }

        // Move to the next field
        if (*end == ',') {
            start = end + 1;
        } else {
            break; // End of line reached
        }
        fieldIndex++;
    }
    return true;
}

bool search_TOVS_record(TOVS_file file, const uint ID, uint *  block_number, uint * char_pos ) {
    int i = 1;
    char  * record = malloc(MAX_RECORD_LENGTH);
    bool new_record = true;
    bool is_divided = false;
    int remaining = 0;
    int current_record_size = 0;
    char size[10]="\0";


    while (i <= getHeader_TOVS(file, 1)) {
        const TOVS_block block = read_TOVS_block(file, i);

        // if we are in the last block, we should only read until the last character position, because all the other blocks are always full
        const int max_of_current = getHeader_TOVS(file, 1) == i ? getHeader_TOVS(file,2): MAX_CHAR_BLOCK_TOVS;
        int j = 0;

        while(j <= max_of_current) {

            const int free_space = max_of_current - j + 1;
            // get the size of the record if it is a new record (TOVS record will be like size,rest_of_record) size is on 4 characters
            if (new_record) {
                if (is_divided) {
                    char rest[10]="\0";
                    // take the remaining but not the comma
                    strncpy(rest, block.data + j, remaining-1);
                    strcat(size,rest);
                    size[4] = '\0';
                    current_record_size = atoi(size);
                    new_record = false;
                    is_divided = false;
                    j = j + remaining;
                    *block_number =i;
                    *char_pos = j;
                    continue;
                }else if (free_space < 5) {
                    is_divided = true;
                    remaining = 5 - free_space;
                    strncpy(size, block.data + j, free_space);
                    j = j + free_space;
                    continue;
                }



                // if size is in the same block (there is space for size and the comma)
                strncpy(size, block.data + j, 4);
                size[4] = '\0';
                current_record_size = atoi(size);
                new_record = false;
                *block_number =i;
                *char_pos = j;
                printf("\nq: %d\n",j);
                j = j + 5;

            }else {
                if (is_divided) {
                    char rest[MAX_RECORD_LENGTH] = "\0";
                    strncpy(rest, block.data + j, free_space<remaining?free_space:remaining-1);
                    strcat(record, rest);
                    remaining = remaining - free_space;
                    j = j + free_space;
                    if (remaining == 0) {
                        new_record = true;
                        is_divided = false;
                        complete_student_record current_record;
                        parse_complete_student_record(record,&current_record);

                        if (current_record.ID == ID) {
                            return true;
                        }else if (current_record.ID > ID) {
                            return false;
                        }
                    }
                }
                if(free_space < current_record_size+1) {
                    is_divided = true;
                    // size of the record + the hashtag separator
                    remaining = current_record_size+1 - free_space;
                    strncpy(record, block.data + j, free_space);
                    j = j + free_space;
                    continue;
                }

                strncpy(record, block.data + j, current_record_size);
                printf("%s",record);

                complete_student_record current_record;
                parse_complete_student_record(record,&current_record);


                if (current_record.ID == ID) {

                    return true;
                }else if (current_record.ID > ID) {

                    return false;
                }
                new_record = true;

                j = j + current_record_size+1;

            }
        }
        i++;
    }
    return false;
}
