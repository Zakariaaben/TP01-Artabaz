// tovc.h
#ifndef TOVC_H
#define TOVC_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_RECORDS 1000 // Size of Tbloc
#define KEYVAL 7         // Number of characters in the key for searching
#define SPECIAL_CHAR '#' // Special character indicating the end of data

// Structure definitions
typedef struct
{
    char data[MAX_RECORDS];
} Tbloc, Tbuffer;

typedef struct
{
    int lastBlockNum; // Number of the last block in the file
    int recordCount;  // Total number of records
    int NBRIns;       // Number of inserted records
    int NBRDel;       // Number of deleted records
} Entete;

typedef struct
{
    FILE *filePtr; // File pointer
    Entete header; // File header
} TOVC_File;

// Function prototypes
void CREATE_TOVCFile(char *filename);
int openTOVC(TOVC_File *tovcFile, const char *filename, char mode);
int closeTOVC(TOVC_File *tovcFile);
int getHeaderField(Entete header, int field);
int setHeaderField(Entete *header, int field, int value);
int readBloc(TOVC_File *tovcFile, int blockNum, Tbloc *block);
int writeBloc(TOVC_File *tovcFile, int blockNum, Tbloc *block);

#endif // TOVC_H
