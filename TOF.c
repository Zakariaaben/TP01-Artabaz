#include "TOF.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Creates a TOF file with the specified file name and initializes its header.
 *
 * @param file_name The name of the file to create.
 * @return true if the file was created successfully, false otherwise.
 */
bool create_TOF_file(const char *file_name) {
    FILE *file = fopen(file_name, "wb+");
    if (file == NULL) {
        return false;
    }
    TOF_header header;
    header.number_of_blocks = 0;
    header.number_of_records = 0;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, file);
    fclose(file);

    return true;
}


/**
 * Opens a TOF file and reads its header.
 *
 * @param file_name The name of the file to open.
 * @return A TOF_file structure containing the file pointer and header information.
 */
TOF_file open_TOF_file(const char *file_name) {
    TOF_file file;
    TOF_header header = {0, 0};

    file.file = fopen(file_name, "rb+");

    fseek(file.file, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, file.file);
    file.header = header;

    if (file.file == NULL) {
        printf("Error while opening file %s\n", strerror(errno));
    }

    return file;
}

/**
 * Returns the number of blocks or records in the TOF file header.
 *
 * @param file The TOF file whose header is to be read.
 * @param property The property to retrieve: 1 for number of blocks, 2 for number of records.
 * @return The number of blocks if property is 1, the number of records if property is 2, and -1 otherwise.
 */
int getHeader(const TOF_file file, const int property) {
    if (file.file == NULL) {
        printf("Error while getting Header, file is NULL");
        return -1;
    }
    switch (property) {
        case 1:
            return file.header.number_of_blocks;
        case 2:
            return file.header.number_of_records;
        default:
            printf("Error getting header, please Enter 1 for NBlk and 2 for NBrec \n");
            return -1;
    }
}


void setHeader(TOF_file *file, const int property, const int value) {
    if (file->file == NULL) {
        printf("Error while setting Header, file is NULL");
        return;
    }
    switch (property) {
        case 1:
            file->header.number_of_blocks = value;
            break;
        case 2:
            file->header.number_of_records = value;
            break;
        default:
            printf("Error setting header, please Enter 1 for NBlk and 2 for NBrec \n");
            break;
    }
}

/**
 * Reads the nth Block from the TOF file.
 *
 * @param file The TOF file to read from.
 * @param block_number The block number to read.
 * @return The TOF block read from the file. If it fails, block.nb_records is set to -1.
 */
TOF_block read_TOF_block(const TOF_file file, const uint block_number) {
    TOF_block block;
    block.nb_records = -1;

    if (file.file == NULL) {
        printf("Error while reading block %d: The file is NULL", block_number);
        return block;
    }
    if (block_number > file.header.number_of_blocks || block_number < 1) {
        printf("Error while reading block %d: This block number does not exist in the file \n", block_number);
        return block;
    }

    // Jump over header at the file beginning
    fseek(file.file, sizeof(TOF_header), SEEK_SET);

    // Jump to the nth block
    fseek(file.file, (block_number - 1) * sizeof(TOF_block), SEEK_SET);
    // Read Block
    fread(&block, sizeof(TOF_block), 1, file.file);

    return block;
}

//Writes Block into nth position inside the file, returns false if it fails and true otherwise
bool write_TOF_block(TOF_file *file, const TOF_block block, const uint block_number) {
    if (file->file == NULL) {
        printf("Error while writing block %d: This file is NULL", block_number);
        return false;
    }

    if (block_number > file->header.number_of_blocks + 1 || block_number < 1) {
        printf("Could not write block %d to file : Invalid block number \n", block_number);
        return false;
    }

    //Jump over header of file
    fseek(file->file, sizeof(TOF_header), SEEK_SET);

    //Go to nth position to write block into file
    fseek(file->file, (block_number - 1) * sizeof(TOF_block), SEEK_SET);

    fwrite(&block, sizeof(TOF_block), 1, file->file);

    return true;
}


void print_TOF_header(const TOF_file file) {
    if (file.file == NULL) {
        printf("Error while printing file header: File is NULL\n");
        return;
    }
    printf("File Header:\n");
    printf("\tNumber of blocks: %d\n", file.header.number_of_blocks);
    printf("\tNumber of records: %d\n", file.header.number_of_records);
}

void print_TOF_block(const TOF_block block) {
    printf("%d\n", block.nb_records);
    for (int i = 0; i < block.nb_records; i++) {
        const student_record record = block.records[i];
        printf("\t%-6d |\t%-25s |\t%-25s |\t%10s |\t%-25s\n", record.ID, record.name, record.family_name,
               record.date_of_birth, record.city_of_birth);
    }
}

void print_student_record(const student_record record) {
    printf("\t%-6d |\t%-25s |\t%-25s |\t%10s |\t%-25s\n", record.ID, record.name, record.family_name,
           record.date_of_birth, record.city_of_birth);
}

// Ancienne version de la recherche

// //Searches for a record with a specific ID in the TOF file, returns true if it finds it and false otherwise
// bool search_TOF_record(const TOF_file file, const uint ID, int *block_pos, int *record_pos,cost * cost) {
//     if (file.file == NULL) {
//         printf("Error while searching for records in the file: FILE is null \n");
//         return false;
//     }
//
//     //If there is no block then it should be in the first block at the first position
//     if (file.header.number_of_blocks == 0) {
//         *block_pos = 1;
//         *record_pos = 0;
//         return false;
//     }
//
//     int low = 1, up = getHeader(file, 1);
//
//     TOF_block buffer;
//
//     bool found=false;
//
//
//     //Searches for block number containing the record
//     while (low <= up ) {
//         *block_pos = (low + up) / 2;
//         buffer = read_TOF_block(file, *block_pos);
//         if (ID > buffer.records[buffer.nb_records - 1].ID) {
//             low = *block_pos + 1;
//         } else if (ID < buffer.records[0].ID) {
//             up = *block_pos - 1;
//         } else {
//             found = true;
//             break;
//         }
//     }
//     if (!found) {
//         *block_pos = low;
//     }
//
//     int low2 = 0, up2 = buffer.nb_records - 1;
//
//
//     //searches for record position inside the block
//     while (low2 <= up2) {
//         *record_pos = (low2 + up2) / 2;
//         const student_record current_record = buffer.records[low];
//         if (current_record.ID == ID) {
//             return true;
//         } else if (current_record.ID < ID) {
//             low2 = *record_pos + 1;
//         } else {
//             up2 = *record_pos - 1;
//         }
//     }
//
//     *record_pos = low;
//
//     return true;
// }


// Search (Hidouci Version)
bool search_TOF_record(const TOF_file file, const uint ID, int *block_pos, int *record_pos, cost *cost) {
    cost->reads = 0;
    cost->writes = 0;

    if (file.file == NULL) {
        printf("Error while searching for records in the file: FILE is null \n");
        return false;
    }

    //If there is no block then it should be in the first block at the first position
    if (getHeader(file, 1) == 0) {
        *block_pos = 1;
        *record_pos = 0;
        return false;
    }

    int low = 1, up = getHeader(file, 1);

    bool Trouv = false, stop = false;
    bool is_deleted = false;
    *block_pos = 1;

    while (low <= up && !Trouv && !stop) {
        *block_pos = (low + up) / 2;
        const TOF_block buffer = read_TOF_block(file, *block_pos);
        cost->reads++;
        if (ID < buffer.records[0].ID) {
            up = *block_pos - 1;
        } else if (ID > buffer.records[buffer.nb_records - 1].ID) {
            low = *block_pos + 1;
        } else {
            int low2 = 0, up2 = buffer.nb_records - 1;
            while (low2 <= up2 && !Trouv) {
                *record_pos = (low2 + up2) / 2;
                if (buffer.records[*record_pos].ID == ID) {
                    Trouv = true;
                    is_deleted = buffer.records[*record_pos].is_deleted;
                } else if (buffer.records[*record_pos].ID < ID) {
                    low2 = *record_pos + 1;
                } else {
                    up2 = *record_pos - 1;
                }
            }
            if (low2 > up2) {
                *record_pos = low2;
            }
            stop = true;
        }
    }

    if (low > up) {
        *block_pos = low;
        *record_pos = 0;
    }

    if (Trouv && !is_deleted) {
        return true;
    } else {
        return false;
    }
}


bool insert_TOF_record(TOF_file *file, student_record record, cost *cost) {
    cost->reads = 0;
    cost->writes = 0;
    record.is_deleted = false;

    int block_pos;
    int record_pos;
    const bool found = search_TOF_record(*file, record.ID, &block_pos, &record_pos, cost);

    if (found) {
        return false;
    }

    // Insert in new block
    if (block_pos == getHeader(*file, 1) + 1) {
        TOF_block block;
        block.records[0] = record;
        block.nb_records = 1;
        setHeader(file, 1, getHeader(*file, 1) + 1);
        write_TOF_block(file, block, getHeader(*file, 1));
        cost->writes++;
        setHeader(file, 2, getHeader(*file, 2) + 1);
        return true;
    }

    bool stop = false;

    while (!stop && block_pos <= getHeader(*file, 1)) {
        TOF_block block = read_TOF_block(*file, block_pos);
        cost->reads++;

        const student_record last_record = block.records[block.nb_records - 1];

        int i = block.nb_records - 1;

        //Décalage inter Block
        while (i > record_pos) {
            block.records[i] = block.records[i - 1];
            i--;
        }

        block.records[record_pos] = record;

        if (block.nb_records < TOF_RECORDS_NUMBER * TOF_LOADING_FACTOR) {
            block.records[block.nb_records] = last_record;
            block.nb_records++;
            write_TOF_block(file, block, block_pos);
            cost->writes++;
            stop = true;
        } else {
            write_TOF_block(file, block, block_pos);
            cost->writes++;
            block_pos++;
            record_pos = 0;
            record = last_record;
        }
    }

    if (block_pos > getHeader(*file, 1)) {
        TOF_block block;
        block.records[0] = record;
        block.nb_records = 1;
        setHeader(file, 1, getHeader(*file, 1) + 1);
        write_TOF_block(file, block, getHeader(*file, 1));
        cost->writes++;
    }

    setHeader(file, 2, getHeader(*file, 2) + 1);

    return true;
}

void print_TOF_file(TOF_file file) {
    for (int i = 1; i <= file.header.number_of_blocks; i++) {
        const TOF_block block = read_TOF_block(file, i);
        printf("Block %d\n", i);
        print_TOF_block(block);
    }
}


// Parsing student record
bool parse_student_record(const char *line, student_record *record) {
    const char *start = line;
    int fieldIndex = 0;
    while (fieldIndex < 5) {
        const char *end = strchr(start, ',');
        if (end == NULL) {
            end = start + strlen(start);
        }

        const size_t length = end - start;

        switch (fieldIndex) {
            case 0:

                char ID[5];
                strncpy(ID, start, length);

                if (atoi(ID) == 0) {
                    return false;
                }
                record->ID = atoi(ID);
                break;
            case 1:
                strncpy(record->name, start, length);
                record->name[length] = '\0';
                break;
            case 2:
                strncpy(record->family_name, start, length);
                record->family_name[length] = '\0';
                break;
            case 3:
                strncpy(record->date_of_birth, start, length);

                record->date_of_birth[length] = '\0';
                break;
            case 4:
                strncpy(record->city_of_birth, start, length);
                record->city_of_birth[length] = '\0';
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


// Function that parse CSV File 01 and loads the TOF File and returns the number of records inserted and the number of records not inserted
int *load_TOF_file_csv(const char *csv_filename, TOF_file *tof_file) {
    FILE *file = fopen(csv_filename, "r");

    static int res[2] = {0, 0};

    if (file == NULL) {
        printf("Error while opening file %s\n", strerror(errno));
        return res;
    }

    // Create a file to store the cost of insertion at each iteration
    const char * source = "tof_insertion_cost.csv";
    FILE *tof_insertion_cost_file = fopen(source, "w");
    fprintf(tof_insertion_cost_file, "ID,Reads,Writes\n");

    char line[1024];
    //jump the first line
    fgets(line, 1024, file);
    int i = 0;
    while (fgets(line, 1024, file)) {
        cost cost = {0, 0};
        student_record record;
        parse_student_record(line, &record);
        record.is_deleted = false;

        if (insert_TOF_record(tof_file, record, &cost)) {
            res[0]++;
            fprintf(tof_insertion_cost_file, "%d,%d,%d\n", record.ID, cost.reads, cost.writes);
        } else {
            res[1]++;
        }

    }
    fclose(tof_insertion_cost_file);
    fclose(file);

    return res;
}



void close_tof_file(const TOF_file *file) {
    if (file->file != NULL) {
        fseek(file->file, 0,SEEK_SET);
        fwrite(&file->header, sizeof(TOF_header), 1, file->file);
        fclose(file->file);
    }
}






