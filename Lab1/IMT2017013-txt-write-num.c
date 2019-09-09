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
    fp = (FILE*)fopen(fname, "w");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }

    int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 10, 11, 12, 22, 33, 44, 55, 66, 77, 88};
    int count = 20;

    for (int i = 0; i < count; i++){
        fprintf(fp, "%d ", nums[i]);
    }
    fclose(fp);

    return 0;
}