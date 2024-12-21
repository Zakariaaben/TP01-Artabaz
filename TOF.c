
#include "TOF.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "csv.h"

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
    header.number_of_deletions = 0 ;
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
 * @return The number of blocks if property is 1, the number of records if property is 2 and -1 otherwise.
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
        case 3 :
            return  file.header.number_of_deletions;
        default:
            printf("Error getting header, please Enter 1 for NBlk and 2 for NBrec and 3 for number of deleted records \n");
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
        case 3:
            file->header.number_of_deletions = value;
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
    printf("\tNumber of deleted records: %d\n",file.header.number_of_deletions);
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
    char ** escapedLine = split_on_unescaped_newlines(line);
    char ** parsed = parse_csv(escapedLine[0]);

    char ID[10];
    strcpy(ID, parsed[0]);
    if (atoi(ID) == 0) {
        return false;
    }

    record->ID = atoi(ID);

    strcpy(record->name, parsed[1]);
    strcpy(record->family_name, parsed[2]);
    strcpy(record->date_of_birth, parsed[3]);
    strcpy(record->city_of_birth, parsed[4]);
    return  true;

}


// Function that parse CSV File 01 and loads the TOF File and returns the number of records inserted and the number of records not inserted
int *load_TOF_file_from_csv(const char *csv_filename, TOF_file *tof_file) {
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
        i++;
        student_record record;
        parse_student_record(line, &record);
        record.is_deleted = false;

        if (insert_TOF_record(tof_file, record, &cost)) {
            res[0]++;
            fprintf(tof_insertion_cost_file, "%d,%d,%d\n", record.ID, cost.reads, cost.writes);
            //print record each 1000 records
            if (VERBOSE && i%1000==0) {
                printf("Record %d with ID %d has been inserted with %d reads and %d writes\n",i, record.ID, cost.reads,
                       cost.writes);
            }
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

bool delete_TOF_record(TOF_file *file, const uint ID, cost *cost) {
    cost->reads = 0;
    cost->writes = 0;

    int block_pos;
    int record_pos;
    const bool found = search_TOF_record(*file, ID, &block_pos, &record_pos, cost);

    if (!found) {
        return false;
    }

    TOF_block block = read_TOF_block(*file, block_pos);
    cost->reads++;

    block.records[record_pos].is_deleted = true;
    write_TOF_block(file, block, block_pos);
    cost->writes++;

    setHeader(file, 3, getHeader(*file, 3) + 1);

    return true;
}


int * delete_TOF_records_from_csv(const char * csv_filename,TOF_file *file) {
    FILE *file_csv = fopen(csv_filename, "r");
    static int res[2] = {0, 0};

    if (file_csv == NULL) {
        printf("Error while opening csv file %s\n", strerror(errno));
        return res;
    }

    const char * source = "./tof_deletion_cost.csv";
    FILE * tof_deletion_cost_file = fopen(source, "w");
    fprintf(tof_deletion_cost_file, "ID,Reads,Writes\n");

    char line[1024];
    //jump the first line
    fgets(line, 1024, file_csv);
    int i=0;
    while (fgets(line, 1024, file_csv)) {
        cost cost = {0, 0};

        const int ID =atoi(line);
        if (ID == 0 ) {
            continue;
        }

        if (delete_TOF_record(file, ID, &cost)) {
            if (VERBOSE) {
                printf("Record with ID %d has been deleted with %d reads and %d writes\n", ID, cost.reads, cost.writes);
            }
            res[0]++;
            fprintf(tof_deletion_cost_file, "%d,%d,%d\n", ID, cost.reads, cost.writes);
        } else {
            res[1]++;
        }
    }

    fclose(tof_deletion_cost_file);
    fclose(file_csv);
    printf("%d",i);

    return  res;
}

int TOF_fragmentation(const TOF_file file) {
    const int nb_blocks = getHeader(file, 1);
    int records_number = 0;

    for (int i=1;i<=nb_blocks;i++){
        const TOF_block block = read_TOF_block(file,i);
        for (int j=0;j<block.nb_records;j++){
            if (!block.records[j].is_deleted){
                records_number++;
            }
        }
    }

    return  records_number;

}



