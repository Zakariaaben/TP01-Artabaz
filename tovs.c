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
TOVS_file open_TOVS_file(char * filename)
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
int getHeader_TOVS(const TOVS_file* file,const int field)
{
    switch (field)
    {
    case 1:
        return file->header.number_of_blocks;
    case 2:
        return file->header.number_of_records;
    case 3:
        return file->header.number_of_inserted_characters;
    case 4:
        return file->header.number_of_deleted_characters;
    default:
        return -1; // Invalid field
    }
}

// Set a specific header field value
void setHeader_TOVS(TOVS_file * file, const int field, int value)
{
    switch (field)
    {
    case 1:
        file->header.number_of_blocks = value;
        break;
    case 2:
        file->header.number_of_records = value;
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
        fread(&block, sizeof(TOVS_block), 1, file.file) == 1);
        return block;
    }
    printf("Error while reading TOVS block , Invalid block number");
    return block;
}

// Read a block from the file
// Read a block from the file
TOVS_block read_TOVS_block(const TOVS_file file,const  int block_number)
{
    // Retrieve the last block number from the header
    int lastBlockNum = getHeaderField(tovcFile->header, 1);
    if (blockNum > 0 && blockNum <= lastBlockNum)
    { // Ensure blockNum is within range
        // Seek to the block's position
        fseek(tovcFile->filePtr, sizeof(Entete) + (blockNum - 1) * sizeof(Tbloc), SEEK_SET);
        // Read the block from the file
        if (fread(block, sizeof(Tbloc), 1, tovcFile->filePtr) == 1)
        {
            return 0; // Success
        }
        else
        {
            return -2; // Read error
        }
    }
    return -1; // Invalid block number
}

// Write a block to the file
bool write_TOVS_block(TOVS_file *file,const int block_number, TOVS_block * block)
//int writeBloc(TOVC_File *tovcFile, int blockNum, Tbloc *block)
{

    // Validate block number
    int lastBlockNum = getHeader_TOVS(file, 1);
    if (block_number <= 0 || block_number>lastBlockNum+1)
    {
        return false; // Invalid block number
    }

    return true; // Success
}
/*
void convertTOFToTOVS(const char *tofFile, const char *tovsFile)
{
   TOVC_File tof;
   if (openTOVC(&tof, tofFile, 'E') != 0)
   {
       printf("Error opening TOF file: %s\n", tofFile);
       return;
   }

   TOVC_File tovs;
   if (openTOVC(&tovs, tovsFile, 'N') != 0)
   {
       printf("Error creating TOVS file: %s\n", tovsFile);
       closeTOVC(&tof);
       return;
   }

   Tbloc block, currentBlock;
   currentBlock.nb = 0;
   int currOffset = 0;

   for (int i = 1; i <= getHeaderField(tof.header, 1); i++)
   {
       if (readBloc(&tof, i, &block) == 0)
       {
           for (int j = 0; j < block.nb; j++)
           {
               int recordSize = strlen(block.data[j]) + 1; // Account for null terminator

               if (currOffset + recordSize > MAX_RECORDS)
               {
                   writeBloc(&tovs, getHeaderField(tovs.header, 1) + 1, &currentBlock);
                   currentBlock.nb = 0;
                   currOffset = 0;
               }

               memcpy(&currentBlock.data[currOffset], block.data[j], recordSize);
               currOffset += recordSize;
           }
       }
   }

   if (currOffset > 0)
   {
       writeBloc(&tovs, getHeaderField(tovs.header, 1) + 1, &currentBlock);
   }

   closeTOVC(&tof);
   closeTOVC(&tovs);

   printf("TOF file converted to TOVS: %s\n", tovsFile);
} */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1024 // Define the block size for TOVS
#define MAX_RECORDS 1000

// Example of student_record structure
typedef struct {
    char name[50];
    int id;
    float score;
} student_record;

typedef struct {
    char data[BLOCK_SIZE];
    int nb; // Number of bytes used in the block
} Tbloc;

void LoadTOVS(const char *tof_file_path, const char *cvs_file_path, const char *tovs_file_path) {
    FILE *tof_file, *cvs_file, *tovs_file;
    Tbloc buffer;
    student_record record;

    // Open the TOF file for reading
    tof_file = fopen(tof_file_path, "rb");
    if (tof_file == NULL) {
        printf("Error opening TOF file.\n");
        return;
    }

    // Open the CSV file for reading
    cvs_file = fopen(cvs_file_path, "r");
    if (cvs_file == NULL) {
        printf("Error opening CSV file.\n");
        fclose(tof_file);
        return;
    }

    // Open the TOVS file for writing
    tovs_file = fopen(tovs_file_path, "wb+");
    if (tovs_file == NULL) {
        printf("Error opening TOVS file.\n");
        fclose(tof_file);
        fclose(cvs_file);
        return;
    }

    // Process records from the TOF file
    while (fread(&record, sizeof(student_record), 1, tof_file) == 1) {
        memset(&buffer, 0, sizeof(Tbloc)); // Clear the buffer
        int record_size = snprintf(buffer.data, BLOCK_SIZE, "%s,%d,%.2f", record.name, record.id, record.score);
        if (record_size < BLOCK_SIZE) {
            buffer.nb = record_size;
            fwrite(&buffer, sizeof(Tbloc), 1, tovs_file); // Write block to TOVS
        } else {
            printf("Record size exceeds block size.\n");
        }
    }

    // Process records from the CSV file
    while (fscanf(cvs_file, "%49[^,],%d,%f\n", record.name, &record.id, &record.score) == 3) {
        memset(&buffer, 0, sizeof(Tbloc)); // Clear the buffer
        int record_size = snprintf(buffer.data, BLOCK_SIZE, "%s,%d,%.2f", record.name, record.id, record.score);
        if (record_size < BLOCK_SIZE) {
            buffer.nb = record_size;
            fwrite(&buffer, sizeof(Tbloc), 1, tovs_file); // Write block to TOVS
        } else {
            printf("Record size exceeds block size. Skipping...\n");
        }
    }

    printf("TOVS file created successfully.\n");

    // Close all files
    fclose(tof_file);
    fclose(cvs_file);
    fclose(tovs_file);
}
