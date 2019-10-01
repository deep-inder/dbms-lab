#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {
    FILE* fp = fopen("trial.bin", "wb+");
    char student[] = "This is a trial array";
    int k = 100;
    fwrite(&k, sizeof(int), 1, fp);
    fclose(fp);
    FILE* fp2=fopen("trial.bin", "rb+");
    char st[40];
    int l = 0;
    fread(&l, sizeof(int), 1, fp2);
    printf("%d\n", l);
    return 0;
}