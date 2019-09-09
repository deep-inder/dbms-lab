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
    fp = (FILE*)fopen(fname, "w");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }

    struct StudentInfo s1 = {10, "Bishal", 19};
    struct StudentInfo s2 = {11, "Brahma", 20};
    struct StudentInfo s3 = {12, "Sai", 19};
    struct StudentInfo s4 = {13, "Deep", 21};
    struct StudentInfo s5 = {15, "Eric", 22};


    fprintf(fp, "%d,%s,%d\n", s1.rollno, s1.name, s1.age);
    fprintf(fp, "%d,%s,%d\n", s2.rollno, s2.name, s2.age);
    fprintf(fp, "%d,%s,%d\n", s3.rollno, s3.name, s3.age);
    fprintf(fp, "%d,%s,%d\n", s4.rollno, s4.name, s4.age);
    fprintf(fp, "%d,%s,%d\n", s5.rollno, s5.name, s5.age);
    fclose(fp);

    return 0;
}