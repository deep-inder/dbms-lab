#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "pds.h"
#include "bst.h"


// Define the global variable
struct PDS_RepoInfo repo_handle;

int pds_create(char* repo_name){
    FILE *fp;
    int status;
    fp = (FILE*)fopen(repo_name, "wb+");
    if (fp != NULL)
        status = PDS_SUCCESS;
    else
        status = PDS_FILE_ERROR;
    fclose(fp);
    return status;
}

int pds_open( char *repo_name, int rec_size )
{
    FILE *fp;
    FILE *np;
    char fname[30];
    char nname[30];
    char* ext1 = ".dat";
    char* ext2 = ".ndx";
    strcpy(fname, repo_name);
    strcpy(nname, repo_name);
    strcat(fname, ext1);
    strcpy(nname, ext2);
    printf("%s\n", fname);
    fp = (FILE*)fopen(fname, "ab+");
    np = (FILE*)fopen(nname, "ab+");
    if (fp == NULL)
        return PDS_FILE_ERROR;
    if (np == NULL)
        return PDS_FILE_ERROR;
    strcpy(repo_handle.pds_name, repo_name);
    repo_handle.pds_data_fp = fp;
    repo_handle.pds_ndx_fp = np;
    repo_handle.rec_size = rec_size;
    repo_handle.repo_status = PDS_REPO_OPEN;
    return PDS_SUCCESS;
}

int put_rec_by_key( int key, void *rec )
{
    FILE* fp = repo_handle.pds_data_fp;
    FILE* np = repo_handle.pds_ndx_fp;
    if (fp == NULL){
        return PDS_FILE_ERROR;
    }
    if (np == NULL){
        return PDS_ADD_FAILED;
    }
    if (repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    else{
        fseek(fp, 0, SEEK_END);
        fseek(np, 0, SEEK_END);
        int offset = ftell(fp);
        fwrite(rec, repo_handle.rec_size, 1, fp);
        struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
        index->key = key;
        index->offset = offset;
        // fwrite(index, sizeof(struct PDS_NdxInfo), 1, np);
        int status = bst_add_node(&repo_handle.pds_bst, key, index);
        if (status == BST_DUP_KEY)
            return PDS_ADD_FAILED;
    }
    return PDS_SUCCESS;
}

int get_rec_by_key( int key, void *rec )
{
    FILE* fp = repo_handle.pds_data_fp;
    if (fp == NULL){
        return PDS_FILE_ERROR;
    }
    fseek(fp, 0, SEEK_SET);
    struct BST_Node* node = bst_search(repo_handle.pds_bst, key);
    if (node != NULL){
        if (node->key == key){
            struct PDS_NdxInfo* index = node->data;
            int offset = index->offset;
            fseek(fp, offset, SEEK_SET);
            fread(rec, repo_handle.rec_size, 1, fp);
            return PDS_SUCCESS;
        }
    }
    else{
            return PDS_REC_NOT_FOUND;
    }
}

int pds_close()
{
    if (repo_handle.pds_data_fp == NULL){
        return PDS_FILE_ERROR;
    }
    fclose(repo_handle.pds_data_fp);
    fclose(repo_handle.pds_ndx_fp);
    return PDS_SUCCESS;
}

