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
    fp = (FILE*)fopen(fname, "r");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }

    int count = 20;
    int somenum;

    for (int i = 0; i < count; i++){
        fscanf(fp, "%d", &somenum);
        printf("%d ", somenum);
    }
    printf("\n");
    fclose(fp);

    return 0;
}