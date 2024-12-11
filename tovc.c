#include "tovc.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TOF.h"
// Function to initialize the file's header
void CREATE_TOVCFile(char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("Error creating TOVC file.\n");
        return;
    }

    Entete header = {0, 0, 0.0};
    fwrite(&header, sizeof(Entete), 1, file);
    fclose(file);
    printf("TOVC file initialized.\n");
}
// Open or create a TOVC file
int openTOVC(TOVC_File *tovcFile, const char *filename, char mode)
{
    switch (mode)
    {
    case 'E': // Open an existing file
        tovcFile->filePtr = fopen(filename, "rb+");
        if (tovcFile->filePtr != NULL)
        {
            rewind(tovcFile->filePtr);
            fread(&tovcFile->header, sizeof(Entete), 1, tovcFile->filePtr);
            return 0;
        }
        return -1;
    case 'N': // Create a new file
        tovcFile->filePtr = fopen(filename, "wb+");
        if (tovcFile->filePtr != NULL)
        {
            tovcFile->header.lastBlockNum = 0;
            tovcFile->header.recordCount = 0;
            tovcFile->header.NBRIns = 0;
            tovcFile->header.NBRDel = 0;
            fwrite(&tovcFile->header, sizeof(Entete), 1, tovcFile->filePtr);
            return 0;
        }
        return -1;
    default:
        return -2; // Invalid mode
    }
}

// Close a TOVC file
int closeTOVC(TOVC_File *tovcFile)
{
    rewind(tovcFile->filePtr);
    fwrite(&tovcFile->header, sizeof(Entete), 1, tovcFile->filePtr); // Update the header
    fclose(tovcFile->filePtr);
    return 0;
}

// Get a specific header field value
int getHeaderField(Entete header, int field)
{
    switch (field)
    {
    case 1:
        return header.lastBlockNum;
    case 2:
        return header.recordCount;
    case 3:
        return header.NBRIns;
    case 4:
        return header.NBRDel;
    default:
        return -1; // Invalid field
    }
}

// Set a specific header field value
int setHeaderField(Entete *header, int field, int value)
{
    switch (field)
    {
    case 1:
        header->lastBlockNum = value;
        return 0;
    case 2:
        header->recordCount = value;
        return 0;
    case 3:
        header->NBRIns = value;
        return 0;
    case 4:
        header->NBRDel = value;
        return 0;
    default:
        return -1; // Invalid field
    }
}
// Read a block from the file
// Read a block from the file
int readBloc(TOVC_File *tovcFile, int blockNum, Tbloc *block)
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
int writeBloc(TOVC_File *tovcFile, int blockNum, Tbloc *block)
{
    // Validate block number
    int lastBlockNum = getHeaderField(tovcFile->header, 1);
    if (blockNum <= 0)
    {
        return -1; // Invalid block number
    }

    // Seek to the block's position
    if (fseek(tovcFile->filePtr, sizeof(Entete) + (blockNum - 1) * sizeof(Tbloc), SEEK_SET) != 0)
    {
        perror("Seek error");
        return -2; // Seek error
    }

    // Write the block
    if (fwrite(block, sizeof(Tbloc), 1, tovcFile->filePtr) != 1)
    {
        perror("Write error");
        return -3; // Write error
    }

    // Update the header if writing a new block
    if (blockNum > lastBlockNum)
    {
        setHeaderField(&(tovcFile->header), 1, blockNum); // Update lastBlockNum
    }

    return 0; // Success
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

