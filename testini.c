#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1024
#define MAX_RECORDS 1000

typedef struct
{
    char name[50];
    int id;
    float score;
} student_record;

typedef struct
{
    char data[BLOCK_SIZE];
    int nb;
} Tbloc;

// Function prototype
void CreatingtheTOVS(const char *tof_file_path, const char *cvs_file_path, const char *tovs_file_path);

int main()
{
    // Create dummy TOF file
    FILE *tof_file = fopen("test_tof.dat", "wb");
    if (!tof_file)
    {
        printf("Failed to create TOF file.\n");
        return 1;
    }

    student_record tof_records[] = {
        {"Alice", 1, 85.5},
        {"Bob", 2, 90.0},
        {"Charlie", 3, 78.3}};

    fwrite(tof_records, sizeof(student_record), 3, tof_file);
    fclose(tof_file);

    // Create dummy CSV file
    FILE *csv_file = fopen("test_cvs.csv", "w");
    if (!csv_file)
    {
        printf("Failed to create CSV file.\n");
        return 1;
    }

    fprintf(csv_file, "David,4,88.7\n");
    fprintf(csv_file, "Eve,5,92.1\n");
    fclose(csv_file);

    // Call the CreatingtheTOVS function
    CreatingtheTOVS("test_tof.dat", "test_cvs.csv", "test_tovs.dat");

    // Verify the contents of the TOVS file
    FILE *tovs_file = fopen("test_tovs.dat", "rb");
    if (!tovs_file)
    {
        printf("Failed to open TOVS file.\n");
        return 1;
    }

    Tbloc block;
    printf("TOVS File Contents:\n");
    while (fread(&block, sizeof(Tbloc), 1, tovs_file) == 1)
    {
        printf("Block Data: %s\n", block.data);
        printf("Block Size: %d bytes\n", block.nb);
    }

    fclose(tovs_file);

    return 0;
}
