#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

int main(int argc, char** argv){
    char fname[30];
    if (argc != 2){
        printf("Insufficient arguments\n");
        exit(1);
    }
    strcpy (fname, argv[1]);
    FILE *fp;
    fp = (FILE*)fopen(fname, "rb");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }
    struct StudentInfo s;

    for (int i = 0; i < 5; i++){
        fread(&s, sizeof(struct StudentInfo), 1, fp);
        printf("Roll no: %d, Name: %s, Age:%d\n", s.rollno, s.name, s.age);
    }

    fclose(fp);
    return 0;
}