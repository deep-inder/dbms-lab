#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "pds.h"
#include "bst.h"


// Define the global variable
struct PDS_RepoInfo repo_handle;

int pds_preorder(struct BST_Node* root);

// int pds_create(char* repo_name){
//     FILE *fp;
//     int status;
//     fp = (FILE*)fopen(repo_name, "wb+");
//     if (fp != NULL)
//         status = PDS_SUCCESS;
//     else
//         status = PDS_FILE_ERROR;
//     fclose(fp);
//     return status;
// }

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
    strcat(nname, ext2);
    printf("%s\n", fname);
    fp = (FILE*)fopen(fname, "ab+");
    np = (FILE*)fopen(nname, "ab+");
    if (fp == NULL){
        // printf("lololololol\n");
        return PDS_FILE_ERROR;
    }
    if (np == NULL){
        return PDS_FILE_ERROR;
    }
    strcpy(repo_handle.pds_name, repo_name);
    repo_handle.pds_data_fp = fp;
    repo_handle.pds_ndx_fp = np;
    repo_handle.rec_size = rec_size;
    repo_handle.repo_status = PDS_REPO_OPEN;
    pds_load_ndx();
    return PDS_SUCCESS;
}

int pds_load_ndx(){
    repo_handle.pds_bst = NULL;
    FILE* np = repo_handle.pds_ndx_fp;
    if (np == NULL){
        return PDS_REPO_NOT_OPEN;
    }
    fseek(np, 0, SEEK_SET);
    int c = fgetc(np);
    // printf("%d\n", c);
    while (c > 0){
        // printf("Bak\n");
        ungetc(c, np);
        struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
        fread(index, sizeof(struct PDS_NdxInfo), 1, np);
        bst_add_node(&repo_handle.pds_bst, index->key, index);
        c = fgetc(np);
    }
    // repo_handle.pds_bst = root;
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
        fwrite(&key, sizeof(int), 1, fp);
        fwrite(rec, repo_handle.rec_size, 1, fp);
        struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
        index->key = key;
        index->offset = offset;
        // fwrite(index, sizeof(struct PDS_NdxInfo), 1, np);
        int status = bst_add_node(&repo_handle.pds_bst, key, index);
        if (status == BST_DUP_KEY){
            return PDS_ADD_FAILED;
        }
    }
    return PDS_SUCCESS;
}

int get_rec_by_ndx_key( int key, void *rec )
{
    int dupkey;
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
            fread(&dupkey, sizeof(int), 1, fp);
            fread(rec, repo_handle.rec_size, 1, fp);
            return PDS_SUCCESS;
        }
    }
    else{
            return PDS_REC_NOT_FOUND;
    }
}

int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key), int *io_count){
    int count = 0;
    FILE* fp = repo_handle.pds_data_fp;
    fseek(fp, 0, SEEK_SET);
    int searched_key;
    void* searched_rec = (void*)malloc(repo_handle.rec_size);
    // printf("%ld\n",ftell(fp));
    while(!feof(fp)){
        fseek(fp, sizeof(int), SEEK_CUR);
        fread(searched_rec, repo_handle.rec_size, 1, fp);
        count++;
        int res = matcher(searched_rec, (char*)key);
        // printf("count:%d, res:%d\n", count, res);
        if (res == 0){
            memcpy(rec, searched_rec, repo_handle.rec_size);
            *io_count = count;
            return PDS_SUCCESS;
        }
        // printf("rekt.....\n");
    }
    printf("Nothing found\n");
    return PDS_REC_NOT_FOUND;
}

int modify_rec_by_key(int key, void* rec){
    int dupkey;
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
            fwrite(&dupkey, sizeof(int), 1, fp);
            fwrite(rec, repo_handle.rec_size, 1, fp);
            return PDS_SUCCESS;
        }
    }
    else{
            return PDS_MODIFY_FAILED;
    }
}

int pds_close()
{
    if (repo_handle.pds_ndx_fp == NULL){
        return PDS_FILE_ERROR;
    }
    if (repo_handle.pds_data_fp == NULL){
        return PDS_FILE_ERROR;
    }
    if(repo_handle.repo_status == PDS_REPO_CLOSED){
        return PDS_SUCCESS;
    }
    char* name = repo_handle.pds_name;
    strcat(name, ".ndx");
    FILE* np = (FILE*)fopen(name, "wb");
    fseek(np, 0, SEEK_SET);
    repo_handle.pds_ndx_fp = np;
    repo_handle.repo_status = PDS_REPO_CLOSED;
    struct BST_Node* root = repo_handle.pds_bst;
    pds_preorder(root);

    bst_free(root);
    fclose(repo_handle.pds_data_fp);
    fclose(repo_handle.pds_ndx_fp);
    return PDS_SUCCESS;
}

int pds_preorder(struct BST_Node* root){
    fseek(repo_handle.pds_ndx_fp, 0, SEEK_END);
    if (root != NULL){
        fwrite(root->data, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp);
        // printf("bacl))))))))\n");
    
        if(root->left_child != NULL)
            pds_preorder(root->left_child);
        if(root->right_child != NULL)
            pds_preorder(root->right_child);
    }
    return PDS_SUCCESS;
}
