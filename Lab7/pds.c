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
    // printf("%s\n", fname);
    if(repo_handle.repo_status == 0)
        repo_handle.repo_status = PDS_REPO_CLOSED;
    if(repo_handle.repo_status == PDS_REPO_OPEN)
        return PDS_REPO_ALREADY_OPEN;
    fp = (FILE*)fopen(fname, "ab+");
    fclose(fp);
    fp = fopen(fname, "rb+");
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
    int status = pds_load_ndx();
    fclose(np);
    // bst_print(repo_handle.pds_bst);
    return status;
}

int pds_load_ndx(){
    if(repo_handle.repo_status == PDS_REPO_OPEN){
        FILE* np = repo_handle.pds_ndx_fp;
        fseek(np, 0, SEEK_SET);
        if (np == NULL){
            return PDS_REPO_NOT_OPEN;
        }
        // printf("%d\n", c);
        while (1){
            struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
            int c = fread(index, sizeof(struct PDS_NdxInfo), 1, np);
            if (c == EOF || c == 0)
                break;
            index->is_deleted = 0;
            bst_add_node(&repo_handle.pds_bst, index->key, index);
            // printf("%d ", index->key);
        }
        // repo_handle.pds_bst = root;
        return PDS_SUCCESS;
    }
    return PDS_FILE_ERROR;
}

int put_rec_by_key( int key, void *rec )
{
    FILE* fp = repo_handle.pds_data_fp;
    FILE* np = repo_handle.pds_ndx_fp;
    if (fp == NULL){
        return PDS_FILE_ERROR;
    }
    if (repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    else{
        fseek(fp, 0, SEEK_END);
        int offset = ftell(fp);
        struct PDS_NdxInfo* index = (struct PDS_NdxInfo*)calloc(sizeof(struct PDS_NdxInfo), 1);
        index->key = key;
        index->offset = offset;
        index->is_deleted = 0;
        // fwrite(index, sizeof(struct PDS_NdxInfo), 1, np);
        int status = bst_add_node(&repo_handle.pds_bst, key, index);
        if (status == BST_DUP_KEY){
            return PDS_ADD_FAILED;
        }
        else{
            fwrite(&key, sizeof(int), 1, fp);
            fwrite(rec, repo_handle.rec_size, 1, fp);
        }
    }
    return PDS_SUCCESS;
}

int get_rec_by_ndx_key( int key, void *rec )
{
    if(repo_handle.repo_status != PDS_REPO_OPEN)
        return PDS_FILE_ERROR;
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
            if (index->is_deleted == 0){
                int offset = index->offset;
                fseek(fp, offset, SEEK_SET);
                fread(&dupkey, sizeof(int), 1, fp);
                fread(rec, repo_handle.rec_size, 1, fp);
                return PDS_SUCCESS;
            }
        }
    }
    return PDS_REC_NOT_FOUND;
}

int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key), int *io_count){
    if(repo_handle.repo_status != PDS_REPO_OPEN){
        return PDS_FILE_ERROR;
    }
    (*io_count) = 0;
    FILE* fp = repo_handle.pds_data_fp;
    if (fp == NULL)
        return PDS_FILE_ERROR;
    fseek(fp, 0, SEEK_SET);
    int found = 0;
    int rec_key;
    fread(&rec_key, sizeof(int), 1, fp);
    while(fread(rec, repo_handle.rec_size, 1, fp)){
        (*io_count)++;
        int res = matcher(rec, (char*)key);
        
        struct BST_Node* node = bst_search(repo_handle.pds_bst, rec_key);
        if (node == NULL){
            return PDS_REC_NOT_FOUND;
        }
        else if(((struct PDS_NdxInfo*)(node->data))->is_deleted == 1)
            return PDS_REC_NOT_FOUND;
        
        // printf("count:%d, res:%d\n", count, res);
        if(res == 0){
            found = 1;
            break;
        }

        fread(&rec_key, sizeof(int), 1, fp);
    }
    if(found)
        return PDS_SUCCESS;
    return PDS_REC_NOT_FOUND;
}

int modify_rec_by_key(int key, void* rec){
    if (repo_handle.repo_status != PDS_REPO_OPEN)
        return PDS_FILE_ERROR;
    struct BST_Node* node = bst_search(repo_handle.pds_bst, key);
    if (node != NULL){
        struct PDS_NdxInfo* temp = node->data;
        if (temp->is_deleted == 1)
            return PDS_REC_NOT_FOUND;
        FILE* fp = repo_handle.pds_data_fp;
        if (fp == NULL){
            return PDS_FILE_ERROR;
        }
        int offset = temp->offset;
        fseek(fp, offset, SEEK_SET);
        fwrite(&key, sizeof(int), 1, fp);
        fwrite(rec, repo_handle.rec_size, 1, fp);
        return PDS_SUCCESS;
    }
    else{
            return PDS_MODIFY_FAILED;
    }
}

int delete_rec_by_ndx_key( int key ){
    if (repo_handle.repo_status != PDS_REPO_OPEN)
        return PDS_FILE_ERROR;
    if (repo_handle.pds_data_fp == NULL || repo_handle.pds_ndx_fp == NULL || repo_handle.pds_bst == NULL)
        return PDS_FILE_ERROR;
    struct BST_Node* node = bst_search(repo_handle.pds_bst, key);
    if (node == NULL)
        return PDS_REC_NOT_FOUND;
    struct PDS_NdxInfo* info = (struct PDS_NdxInfo*)(node->data);
    if (info->is_deleted == 1){
        return PDS_DELETE_FAILED;
    }
    else{
        info->is_deleted = 1;
        return PDS_SUCCESS;
    }
}

int pds_close()
{
    if (repo_handle.pds_data_fp == NULL){
        return PDS_FILE_ERROR;
    }
    if(repo_handle.repo_status == PDS_REPO_CLOSED){
        return PDS_SUCCESS;
    }
    char* name = repo_handle.pds_name;
    strcat(name, ".ndx");
    FILE* np = (FILE*)fopen(name, "wb");
    // fseek(np, 0, SEEK_SET);
    repo_handle.pds_ndx_fp = np;
    repo_handle.repo_status = PDS_REPO_CLOSED;
    // bst_print(root);
    pds_preorder(repo_handle.pds_bst);
    // bst_print(repo_handle.pds_bst);
    bst_free(repo_handle.pds_bst);
    fclose(repo_handle.pds_data_fp);
    fclose(repo_handle.pds_ndx_fp);
    repo_handle.pds_bst = NULL;
    repo_handle.pds_ndx_fp = NULL;
    return PDS_SUCCESS;
}

int pds_preorder(struct BST_Node* root){
    // fseek(repo_handle.pds_ndx_fp, 0, SEEK_END);
    if (root == NULL)
        return BST_NULL;
    else{
        FILE* np = repo_handle.pds_ndx_fp;
        struct PDS_NdxInfo* index = root->data;
        if(index->is_deleted == 0)
            fwrite(root->data, sizeof(struct PDS_NdxInfo), 1, np);

        pds_preorder(root->left_child);
        pds_preorder(root->right_child);
    }
    return PDS_SUCCESS;
}
