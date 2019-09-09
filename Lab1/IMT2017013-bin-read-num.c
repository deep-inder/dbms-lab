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
    fp = (FILE*)fopen(fname, "rb");
    if (fp == NULL){
        printf("Error: No file of that name exists!\n");
        exit(1);
    }
    int mynums[20];
    fopen("ex2a.dat", "rb");
    fread(mynums, sizeof(int), 20, fp);
    fclose(fp);

    for (int i = 0; i < 20; i++){
        printf("%d ", mynums[i]);
    }
    printf("\n");
    return 0;
}