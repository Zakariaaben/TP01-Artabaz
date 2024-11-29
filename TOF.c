#include "TOF.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Takes a file path as input and creates the according TOF file with header initialization, returs
int create_TOF_file(const char *file_name) {
    FILE *file = fopen(file_name, "wb+");
    if (file == NULL) {
        printf("Error while creating file : %s\n",strerror(errno));
        return false;
    }
    TOF_header header;
    header.number_of_blocks =0;
    header.number_of_records=0;
    fwrite(&header, sizeof(header), 1, file);
    fclose(file);

    return true;
}


TOF_file open_TOF_file(const char *file_name) {
    FILE * raw_file = fopen(file_name, "rb");
    TOF_file file;
    TOF_header header = {-1,-1};

    //initialize TOF_File struct
    file.file = raw_file;
    file.header = header;

    if (file.file == NULL) {
        printf("Error while reading file : %s\n",strerror(errno));
        return file;
    }

    fseek(raw_file, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, raw_file);
    file.header = header;

    return file;
}

// Returns the number of Blocks if property = 1, the number of records if 2 and -1 if else
int getHeader(const TOF_file file,const int property) {

    switch (property) {
        case 1:
            return file.header.number_of_blocks;
            break;
        case 2:
            return file.header.number_of_records;
            break;
        default:
            printf("Error getting header, please Enter 1 for NBlk and 2 for NBrec \n");
            return -1;
    }
}

// Reads the nth Block from the TOF file, Block.nb_records = -1 if it fails.
TOF_block read_TOF_block(const TOF_file file,uint block_number) {
    TOF_block block;
    block.nb_records = -1;

    if (file.file == NULL) {
        printf("Error while reading block: This file does not exist");
        return block;
    }
    if (block_number > file.header.number_of_blocks || block_number < 1) {
        printf("Error while reading block: Block does not exist in file \n");
        return block;
    }

    //Jump over header at the file beginning
    fseek(file.file, sizeof(TOF_header), SEEK_SET);

    //Jump to the block
    fseek(file.file,  (block_number-1) * sizeof(TOF_block), SEEK_SET);
    //Read Block
    fread(&block, sizeof(TOF_block), 1, file.file);

    return block;

}

//Writes Block into nth position inside the file, returns false if it fails and true otherwise
bool write_TOF_block(const TOF_file file,const TOF_block block, int block_number) {
    if (file.file == NULL) {
        printf("Error while reading block: This file does not exist");
        return false;
    }
    if (block_number > file.header.number_of_blocks+1 || block_number < 1) {
        printf("Could not read block number from file \n");
        return false;
    }
    //Jump over header of file
    fseek(file.file, sizeof(TOF_header), SEEK_SET);

    //Go to nth position to write block into file
    fseek(file.file, (block_number-1) * sizeof(TOF_block), SEEK_SET);
    fwrite(&block, sizeof(TOF_block), 1, file.file);

}

