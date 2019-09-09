#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    //format is: fwrite(&data, bitsize of data, number of data elements per entry, filename)

    int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 10, 11, 12, 22, 33, 44, 55, 66, 77, 88};
    fwrite(nums, sizeof(int), 20, fp);
    fclose(fp);
}