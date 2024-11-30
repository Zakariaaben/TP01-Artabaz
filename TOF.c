#include "TOF.h"
#include <errno.h>
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
int create_TOF_file(const char *file_name) {
    FILE *file = fopen(file_name, "wb+");
    if (file == NULL) {
        printf("Error while creating file : %s\n", strerror(errno));
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

    // Jump to the block
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

    //If a new block is created
    if (file->header.number_of_blocks + 1 == block_number) {
        file->header.number_of_blocks++;
        file->header.number_of_records += block.nb_records;
    } else {
        //If we replace an old block
        TOF_block old_block;
        fread(&old_block, sizeof(TOF_block), 1, file->file);
        file->header.number_of_records += block.nb_records - old_block.nb_records;
        //Rewind block
        fseek(file->file, -sizeof(TOF_block), SEEK_CUR);
    }

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
    printf("Number of records in block: %d\n", block.nb_records);
    for (int i = 0; i < block.nb_records; i++) {
        const student_record record = block.records[i];
        printf("\t%-6d |\t%-25s |\t%-25s |\t%10s |\t%-25s\n", record.ID, record.name, record.family_name,
               record.date_of_birth, record.city_of_birth);
    }
}


//Searches for a record with a specific ID in the TOF file, returns true if it finds it and false otherwise
bool search_TOF_record(const TOF_file file,const uint ID,  int * block_pos, int * record_pos) {
    if (file.file == NULL) {
        printf("Error while searching for records in the file: FILE is null \n");
        return false;
    }

    //If there is no block then it should be in the first block
    if (file.header.number_of_blocks == 0) {
        *block_pos = 1;
        *record_pos = 0;
        return false;
    }

    int low = 1, up = getHeader(file, 1);

    int mid = (low+up)/2;
    TOF_block buffer;

    //Searches for block number containing the record
    while( low <= up) {
        buffer = read_TOF_block(file, mid);
        print_TOF_block(buffer);
        if (ID < buffer.records[0].ID) {
            up = mid-1;
            mid = (up+low)/2;
        }else if (ID > buffer.records[buffer.nb_records-1].ID) {
            low = mid+1;
            mid = (low+up)/2;
        }else {
            break;
        }
    }

    *block_pos = mid;

    int low2=0 ,up2 = buffer.nb_records-1;
    int mid2 = (low2+up2)/2;

    //searches for record position inside the block
    while (low2 <= up2) {
        const student_record current_record = buffer.records[mid];
        if (current_record.ID == ID) {
            *record_pos = mid2;
            return true;
        }else if (current_record.ID < ID) {
            low2= mid2+1;
            mid2 = (up2+low2)/2;
        }else {
            up2 = mid2-1;
            mid2 = (low2+up2)/2;
        }
    }

    *record_pos = mid2;
    return false;


}











