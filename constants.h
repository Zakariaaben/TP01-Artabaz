#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_NAME_LENGTH  25
#define MAX_FAMILY_NAME_LENGTH 25
#define MAX_CITY_NAME_LENGTH 25
#define MAX_ACQUIRED_SKILLS_LENGTH 255
#define VERBOSE true

typedef unsigned int uint;

typedef struct cost {
    int reads;
    int writes;
} cost;

typedef struct student_record { // TOF
    uint ID;
    char name[MAX_NAME_LENGTH];
    char family_name[MAX_FAMILY_NAME_LENGTH];
    char date_of_birth[20];
    char city_of_birth[MAX_CITY_NAME_LENGTH];
    bool is_deleted;
} student_record;

typedef struct student_additional_info { // csv
    uint ID, year_of_study;
    char acquired_skills[MAX_ACQUIRED_SKILLS_LENGTH];
} student_additional_info;

typedef struct complete_student_record  {
    uint ID;  // Common field
    char name[MAX_NAME_LENGTH];
    char family_name[MAX_FAMILY_NAME_LENGTH];
    char date_of_birth[20];
    char city_of_birth[MAX_CITY_NAME_LENGTH];
    uint year_of_study;
    char acquired_skills[MAX_ACQUIRED_SKILLS_LENGTH];
    bool is_deleted;
} complete_student_record;

#endif //CONSTANTS_H
