#include <stdio.h>
#include <stdlib.h>
#include "pds.h"
#include "contact.h"

int main(int argc, char** argv){
    if (argc != 2){
        fprintf(stderr, "Usage: contact_create <reponame>");
        exit(1);
    }

    int status;

    status = pds_create(argv[1]);

    if (status == PDS_SUCCESS){
        printf("PDS repository \"%s\" created successfully!\n", argv[1]);
    }
    else{
        printf("Unable to create PDS repository %s\n", argv[1]);
    }
    return 0;
}