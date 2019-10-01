#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;

int pds_open(char* repo_name, int rec_size)
{
  if(repo_handle.repo_status == 0) repo_handle.repo_status = PDS_REPO_CLOSED;

  if(repo_handle.repo_status == PDS_REPO_OPEN) 
  {
    return PDS_REPO_ALREADY_OPEN;
  }
  else if(repo_handle.repo_status == PDS_REPO_CLOSED)
  {
    char filename[30], indexfile[30];
    strcpy(filename,repo_name);
    strcpy(indexfile,repo_name);
    strcat(filename,".dat");
    strcat(indexfile,".ndx");

    
    FILE *fp = fopen(filename,"ab+");
    fclose(fp);
    fp = fopen(filename,"rb+");
    FILE *ifp = fopen(indexfile,"ab+");
    if(fp  == NULL || ifp == NULL) return PDS_FILE_ERROR;

    repo_handle.pds_data_fp = fp;
    repo_handle.pds_ndx_fp = ifp;
    repo_handle.rec_size = rec_size;
    strcpy(repo_handle.pds_name,repo_name);
    repo_handle.repo_status = PDS_REPO_OPEN;

    int status = pds_load_ndx();

    fclose(ifp);
    repo_handle.pds_ndx_fp = NULL;

    return status;
  }
  return PDS_SUCCESS;
}

int pds_load_ndx()
{
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    FILE* ifp = repo_handle.pds_ndx_fp;
    fseek(ifp,0,SEEK_SET);

    while(1)
    {
      struct PDS_NdxInfo *ndx = (struct PDS_NdxInfo*)malloc(sizeof(struct PDS_NdxInfo));
      int temp = fread(ndx,sizeof(struct PDS_NdxInfo),1,ifp);
      if(temp == EOF || temp == 0) break;

      ndx->is_deleted = 0;
      int status = bst_add_node(&repo_handle.pds_bst,ndx->key,ndx);
    }

    return PDS_SUCCESS;
  }
  return PDS_FILE_ERROR;
}

int put_rec_by_key(int key, void*rec)
{
  FILE* fp = repo_handle.pds_data_fp;
  FILE *ifp = repo_handle.pds_ndx_fp;

  if(repo_handle.repo_status == PDS_REPO_OPEN)
  { 
    fseek(fp,0,SEEK_END);
    int offset = ftell(fp);

    struct PDS_NdxInfo *ndx = (struct PDS_NdxInfo*)calloc(sizeof(struct PDS_NdxInfo),1);
    ndx -> key = key;
    ndx -> offset = offset;
    ndx -> is_deleted = 0;

    int res = bst_add_node(&repo_handle.pds_bst,key,ndx);
    if(res == BST_SUCCESS) 
    {
      // Write the key and then the data
      fwrite(&key,sizeof(int),1,fp);
      fwrite(rec,repo_handle.rec_size,1,fp);
      return PDS_SUCCESS;
    }
    // else if (res == BST_DUP_KEY) return PDS_ADD_FAILED;
    else if(res == BST_DUP_KEY) return PDS_SUCCESS;
    else return PDS_FILE_ERROR;
  }
  else return PDS_ADD_FAILED;
}

int get_rec_by_ndx_key(int key, void*rec)
{
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    struct BST_Node* node = bst_search(repo_handle.pds_bst,key);

    if(node == NULL) return PDS_REC_NOT_FOUND;
    // if(node == NULL) return PDS_FILE_ERROR;
    else
    {
      struct PDS_NdxInfo* temp = node->data;
      if(temp->is_deleted) return PDS_REC_NOT_FOUND;

      FILE* fp = repo_handle.pds_data_fp;
      int offset = temp->offset;

      fseek(fp,offset,SEEK_SET);
      int key1;
      fread(&key1,sizeof(int),1,fp);
      fread(rec,repo_handle.rec_size,1,fp);

      return PDS_SUCCESS;
    }                                            
  }
  return PDS_FILE_ERROR;
}

int pds_save_ndx(struct BST_Node* root)
{
  if(root == NULL) return BST_NULL;
  else 
  {
    // Root, left, right;
    FILE* ifp = repo_handle.pds_ndx_fp;

    struct PDS_NdxInfo* ndx = root->data;
    if(!ndx->is_deleted) fwrite(root->data,sizeof(struct PDS_NdxInfo),1,ifp);
    pds_save_ndx(root -> left_child);
    pds_save_ndx(root -> right_child);
  }

  return PDS_SUCCESS;
}

int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key),int *io_count)
{
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    // return FILE_ERROR if matcher > 1
    // return PDS_REC_NOT_FOUND if mathcer == 1
    // return PDS_SUCCESS if matcher == 0

    FILE *fp = repo_handle.pds_data_fp;
    fseek(fp,0,SEEK_SET);

    int found = 0;
    *io_count = 0;
    int temp_key;
    fread(&temp_key,sizeof(int),1,fp);
    while(fread(rec,repo_handle.rec_size,1,fp))
    {
      (*io_count)++;
      int status = matcher(rec,key);
      
      // Checking key
      struct BST_Node* node = bst_search(repo_handle.pds_bst,temp_key);
      struct PDS_NdxInfo* ndx = node->data;

      if(node == NULL) return PDS_REC_NOT_FOUND;
      else if(ndx->is_deleted == 1) return PDS_REC_NOT_FOUND;

      if(status > 1) return PDS_FILE_ERROR;
      else if(status == 0) 
      {
        found = 1;
        break;
      }
      fread(&temp_key,sizeof(int),1,fp);
    }

    if(found) return PDS_SUCCESS;
    else return PDS_REC_NOT_FOUND;
  }
  else return PDS_FILE_ERROR;
}

int modify_rec_by_key(int key, void *rec)
{
  // printf("here\n");
  // fflush(stdout);
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    struct BST_Node* node = bst_search(repo_handle.pds_bst,key);

    if(node == NULL) return PDS_MODIFY_FAILED;
    else
    {
      
      struct PDS_NdxInfo* temp = node->data;
      if(temp->is_deleted) return PDS_REC_NOT_FOUND;
      printf("here\n");
      fflush(stdout);
      FILE *fp = repo_handle.pds_data_fp;
      int offset = temp->offset;

      fseek(fp,offset,SEEK_SET);
      fwrite(&key,sizeof(int),1,fp);
      fwrite(rec,repo_handle.rec_size,1,fp);
      return PDS_SUCCESS;
    }
  }
  return PDS_FILE_ERROR;
}

int delete_rec_by_ndx_key(int key)
{
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    struct BST_Node* node = bst_search(repo_handle.pds_bst,key);
    if(node == NULL) return PDS_REC_NOT_FOUND;
    struct PDS_NdxInfo* ndx = ((struct PDS_NdxInfo*)node -> data);
    if(!ndx -> is_deleted)  
    {
      ndx -> is_deleted = 1;
      return PDS_SUCCESS;
    }
    else return PDS_DELETE_FAILED;
  }
  return PDS_FILE_ERROR;
}

int pds_close()
{
  if(repo_handle.repo_status == PDS_REPO_OPEN)
  {
    char indexfile[30];
    strcpy(indexfile,repo_handle.pds_name);
    strcat(indexfile,".ndx");
    FILE* ifp = fopen(indexfile,"wb");
    repo_handle.pds_ndx_fp = ifp;

    if(ifp == NULL) return PDS_FILE_ERROR;

    FILE* fp = repo_handle.pds_data_fp;

    strcpy(repo_handle.pds_name,"");
    repo_handle.rec_size = 0;
    fclose(fp);

    // Function which copies the BST to ifp
    pds_save_ndx(repo_handle.pds_bst);
    fclose(ifp);
    bst_destroy(repo_handle.pds_bst);
    repo_handle.pds_bst = NULL;
    repo_handle.pds_ndx_fp = NULL;
    repo_handle.repo_status = PDS_REPO_CLOSED;

    return PDS_SUCCESS;
  }
  return PDS_FILE_ERROR;
}
