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
    fp = (FILE*)fopen(fname, "wb");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }

    struct StudentInfo s1 = {10, "Bishal", 19};
    struct StudentInfo s2 = {11, "Brahma", 20};
    struct StudentInfo s3 = {12, "Sai", 19};
    struct StudentInfo s4 = {13, "Deep", 21};
    struct StudentInfo s5 = {15, "Eric", 22};

    fwrite(&s1, sizeof(struct StudentInfo), 1, fp);
    fwrite(&s2, sizeof(struct StudentInfo), 1, fp);
    fwrite(&s3, sizeof(struct StudentInfo), 1, fp);
    fwrite(&s4, sizeof(struct StudentInfo), 1, fp);
    fwrite(&s5, sizeof(struct StudentInfo), 1, fp);
    fclose(fp);
    return 0;
}