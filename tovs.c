#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TOF.h"
#include "tovs.h"

// Function to initialize the file's header
bool create_TOVS_file(const char *filename) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        printf("Error creating TOVS file.\n");
        return false;
    }

    const TOVS_header header = {0, 0, 0, 0};
    fseek(file, 0,SEEK_SET);
    fwrite(&header, sizeof(TOVS_header), 1, file);
    fclose(file);
    printf("TOVS file Created.\n");

    return true;
}


// Open or create a TOVC file
TOVS_file open_TOVS_file(const char *filename) {
    TOVS_file file;
    TOVS_header header = {0, 0, 0, 0};

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
int close_TOVS_file(const TOVS_file file) {
    rewind(file.file);
    fwrite(&file.header, sizeof(TOVS_header), 1, file.file); // Update the header
    fclose(file.file);
    return 0;
}

// Get a specific header field value
int getHeader_TOVS(const TOVS_file file, const int field) {
    switch (field) {
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
void setHeader_TOVS(TOVS_file *file, const int field, const int value) {
    switch (field) {
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
            break; // Invalid field
    }
}


// Read a block from the file
// Read a block from the file
TOVS_block read_TOVS_block(const TOVS_file file, const int block_number) {
    TOVS_block block;
    // Retrieve the last block number from the header
    int lastBlockNum = getHeader_TOVS(file, 1);
    if (block_number > 0 && block_number <= lastBlockNum) {
        // Ensure blockNum is within range
        // Seek to the block's position
        fseek(file.file, sizeof(TOVS_header) + (block_number - 1) * sizeof(TOVS_block), SEEK_SET);
        // Read the block from the file
        fread(&block, sizeof(TOVS_block), 1, file.file);
        return block;
    }
    printf("Error while reading TOVS block , Invalid block number");
    return block;
}


// Write a block to the file
bool write_TOVS_block(const TOVS_file *file, const int block_number, const TOVS_block *block) {
    // Validate block number
    const int lastBlockNum = getHeader_TOVS(*file, 1);
    if (block_number <= 0 || block_number > lastBlockNum + 1) {
        return false; // Invalid block number
    }

    fseek(file->file, sizeof(TOVS_header) + sizeof(TOVS_block) * (block_number - 1),SEEK_SET);
    fwrite(block, sizeof(TOVS_block), 1, file->file);

    return true; // Success
}

// Converts TOVS record to string of format isDeleted,ID,name,familyname,date,city,year,skills
char *convert_full_record_to_string(const complete_student_record record) {
    // Allocate enough memory for the string
    char *string_record = malloc(MAX_RECORD_LENGTH);
    if (!string_record) {
        printf("Error allocating string record");
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
    strcat(string_record, record.is_deleted ? "1," : "0,");

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
    strcat(string_record, record.year_of_study == -1 ? "" : year_of_study);

    strcat(string_record, ",");
    strcat(string_record, record.acquired_skills);
    // add \0 at the end of the string
    strcat(string_record, "\0");
    return string_record;
}


// Parsing student record in format isDeleted,ID,name,family_name,date,city,year,skills
bool parse_complete_student_record(const char *line, complete_student_record *record) {
    const char *start = line;
    int fieldIndex = 0;
    while (fieldIndex < 8) {
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
                ID[length] = '\0';

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
            case 5:
                strncpy(record->city_of_birth, start, length);
                record->city_of_birth[length] = '\0';
                break;

            case 6:
                char year_of_study[5];
                strncpy(year_of_study, start, length);
                year_of_study[length] = '\0';
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

bool parse_additional_info(const char *line, student_additional_info *record) {
    const char *start = line;
    int fieldIndex = 0;
    while (fieldIndex < 3) {
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
                if (atoi(year_of_study) == 0) {
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


bool search_TOVS_record(TOVS_file file, const uint ID, uint *block_number, uint *char_pos) {
    bool is_divided = false;
    bool new_record = true;


    // Variable to hold the size of the current record
    char size[10];
    char record[MAX_RECORD_LENGTH] = "\0";

    int j = 0;
    int i = 1;

    *block_number = 1;
    *char_pos = 0;

    // remaining chars of the size variable/record variable
    int remaining_chars = 0;

    int current_record_size = 0;

    // iterate over all the blocks
    while (i <= getHeader_TOVS(file, 1)) {
        j = 0;

        const int current_last_index = getHeader_TOVS(file, 1) == i ? getHeader_TOVS(file, 2) : MAX_CHAR_BLOCK_TOVS - 1;
        const TOVS_block block = read_TOVS_block(file, i);


        while (j <= current_last_index) {
            const int space_to_end = MAX_CHAR_BLOCK_TOVS - j;

            if (new_record) {
                // If the record is not divided and there is enough space for it
                if (!is_divided && space_to_end >= 5) {
                    *block_number = i;
                    *char_pos = j;
                    strncpy(size, block.data + j, 4);
                    size[4] = '\0';
                    j = j + 5;
                    new_record = false;
                    current_record_size = atoi(size);
                    continue;
                }


                // If the record is not divided yet and there is not enough space then take first part and set is_divided to true
                if (!is_divided) {
                    *block_number = i;
                    *char_pos = j;
                    // get first part of the size in the record i
                    strncpy(size, block.data + j, space_to_end);
                    remaining_chars = 4 - space_to_end;
                    j = j + space_to_end;
                    is_divided = true;
                    continue;
                }

                // Get second part of the record and jump over the comma
                char rest[10] = "\0";
                strncpy(rest, block.data + j, remaining_chars);
                strcat(size, rest);
                size[4] = '\0';
                j = j + remaining_chars + 1;
                is_divided = false;
                new_record = false;
                current_record_size = atoi(size);
            } else {
                // There is enough space for the hole record
                if (!is_divided && space_to_end >= current_record_size + 1) {
                    strncpy(record, block.data + j, current_record_size);
                    record[current_record_size] = '\0';

                    complete_student_record current_record;
                    parse_complete_student_record(record, &current_record);

                    if (current_record.ID == ID) {
                        return true;
                    } else if (current_record.ID > ID) {
                        return false;
                    }

                    // jump over the #
                    j = j + current_record_size + 1;
                    new_record = true;
                }

                //Take first part of the record
                if (!is_divided && space_to_end < current_record_size + 1) {
                    is_divided = true;
                    remaining_chars = current_record_size - space_to_end;

                    strncpy(record, block.data + j, space_to_end);
                    record[space_to_end] = '\0';

                    j = j + space_to_end;
                    continue;
                }

                // Take next part of the record
                if (is_divided) {
                    char part_of_record[MAX_RECORD_LENGTH] = "\0";

                    // If there is not enough space to take the whole record
                    if (remaining_chars > space_to_end) {
                        strncpy(part_of_record, block.data + j, space_to_end);
                        remaining_chars -= space_to_end;
                        j = j + space_to_end;
                    } else {
                        strncpy(part_of_record, block.data + j, remaining_chars);
                        part_of_record[remaining_chars] = '\0';
                        j = j + remaining_chars + 1;
                        is_divided = false;
                        new_record = true;
                    }
                    strcat(record, part_of_record);
                    // We finished getting the hole record
                    if (new_record) {
                        complete_student_record current_record;
                        parse_complete_student_record(record, &current_record);

                        if (current_record.ID == ID) {
                            return true;
                        } else if (current_record.ID > ID) {
                            return false;
                        }
                    }
                }
            }
        }
        i++;
    }


    const int nblk = getHeader_TOVS(file, 1);
    // If the file is not empty, and we are at this point then the record should be inserted at the end
    if (nblk != 0) {
        *char_pos = getHeader_TOVS(file, 2);
        * block_number  = nblk;

        *char_pos +=1 ;
        if (*char_pos == MAX_CHAR_BLOCK_TOVS) {
            printf("biggest !! \n");
            *char_pos = 0;
            * block_number  = nblk+1;
        }

    }
    return false;
}


bool insert_TOVS_record(TOVS_file *file, const complete_student_record record) {
    int block_pos, char_pos;

    const bool found = search_TOVS_record(*file, record.ID, (uint *) &block_pos, (uint *) &char_pos);

    if (found) {
        return false;
    }

    char *string_record = convert_full_record_to_string(record);
    char final_tovs_record[MAX_RECORD_LENGTH];
    char size[10];

    size_t length = strlen(string_record);
    snprintf(size, 10, "%.4d,", length);
    size[length] = '\0';

    // Creating the TOVS record to insert
    strcpy(final_tovs_record, size);
    strcat(final_tovs_record, string_record);
    free(string_record);
    strcat(final_tovs_record,RECORD_SEPARATOR_TOVS);
    printf("  -  record to insert : %s\n should start at %d %d\n",final_tovs_record,block_pos,char_pos );
    length = strlen(final_tovs_record);


    int j = char_pos;
    size_t count = 0;

    for (int i = block_pos; i <= getHeader_TOVS(*file, 1); i++) {
        TOVS_block block = read_TOVS_block(*file, i);
        const int current_last_index = getHeader_TOVS(*file, 1) == i
                                           ? getHeader_TOVS(*file, 2)
                                           : MAX_CHAR_BLOCK_TOVS - 1;

        while (j <= current_last_index) {
            const char temp = block.data[j];
            block.data[j] = final_tovs_record[count];
            final_tovs_record[count] = temp;
            count = (count + 1) % length;
            j++;
        }
        j = 0;
        write_TOVS_block(file, i, &block);
    }

    TOVS_block block;

    int nblk = getHeader_TOVS(*file, 1);
    //get index of character in the last block
    j = getHeader_TOVS(*file, 2);

    // If block where to insert already exists and is not full
    if (nblk != 0 && ((j + 1) != MAX_CHAR_BLOCK_TOVS)) {
        j++;
        block = read_TOVS_block(*file, nblk);
    }else { // jf block is full or it is the first block
        nblk ++;
        j = 0;
    }

    count = 0;

    while (count < length) {
        block.data[j] = final_tovs_record[count];
        j++;
        count++;

        if (j == MAX_CHAR_BLOCK_TOVS) {
            write_TOVS_block(file, nblk, &block);
            setHeader_TOVS(file, 1, nblk);
            nblk++;
            j = 0;
        }
    }

    if (j != 0) {
        write_TOVS_block(file, nblk, &block);
        setHeader_TOVS(file, 1, nblk);
        setHeader_TOVS(file,2,j - 1);
    }else {
        setHeader_TOVS(file,2,MAX_CHAR_BLOCK_TOVS - 1);
    }



    return true;
}



