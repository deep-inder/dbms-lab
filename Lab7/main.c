#include<stdio.h>
#include<stdlib.h>
#include "movie.h"


int main(){
    printf("\t***This is a interactable menu for movie database***\n");
    while(1){
        int choice = 0;
        printf("\nSelect any one of the options below:-\n");
        printf("1: Enter database name\n");
        printf("0: Exit app\n");
        printf("Enter a number: ");
        scanf("%d", &choice);
        if (choice == 1){
            char dbname[50];
            printf("Enter database name: ");
            fflush(stdin);
            scanf("%s", dbname);
            int status = create_db(dbname);
            if (status == 0){
                printf("Inside database %s\n", dbname);
            }
            else{
                printf("Unable to create Database. Error: %d\n", status);
                continue;
            }
            while(1){
                int opt = 0;
                printf("\nSelect any one of the options below:-\n");
                printf("1: Add a movie\n");
                printf("2: Search for movie by key\n");
                printf("3: Search for movie by name\n");
                printf("4: Update movie data\n");
                printf("5: Delete movie from database\n");
                printf("0: Save data and go back\n");
                printf("Enter a number: ");
                fflush(stdin);
                scanf("%d", &opt);
                if (opt == 0){
                    close_db();
                    printf("Save successful!\n");
                    break;
                }

                else if (opt == 1){
                    int id;
                    printf("Enter movie ID: ");
                    fflush(stdin);
                    scanf("%d", &id);
                    struct Movie* m = (struct Movie*)malloc(sizeof(struct Movie));
                    fill_movie_attributes(id, m);
                    int stat = add_movie(m);
                    if (stat == 0){
                        printf("Movie added successfully\n");
                        continue;
                    }
                }

                else if (opt == 2){
                    int id;
                    printf("Enter search ID: ");
                    fflush(stdin);
                    scanf("%d", &id);
                    struct Movie* m = (struct Movie*)malloc(sizeof(struct Movie));
                    int stat = search_movie_by_id(id, m);
                    if (stat != 0){
                        printf("Movie not found!\n");
                    }
                    else{
                        printf("Movie found!\n");
                        print_movie_info(m);
                        continue;
                    }
                }

                else if (opt == 3){
                    char name[50];
                    printf("Enter name of movie: ");
                    fflush(stdin);
                    scanf("%s", name);
                    struct Movie* m = (struct Movie*)malloc(sizeof(struct Movie));
                    int stat = search_movie_by_name(name, m);
                    // printf("%d++++++++++++++\n", stat);
                    if (stat != 0){
                        printf("Movie not found!\n");
                        continue;
                    }
                    else{
                        printf("Movie found!\n");
                        print_movie_info(m);
                        continue;
                    }
                }

                else if (opt == 4){
                    int oldid, newid = 0;
                    printf("Enter ID of movie to update: ");
                    fflush(stdin);
                    scanf("%d", &oldid);
                    printf("Enter ID of new movie: ");
                    fflush(stdin);
                    scanf("%d", &newid);
                    struct Movie* m = (struct Movie*)malloc(sizeof(struct Movie));
                    fill_movie_attributes(newid, m);
                    int stat = modify_movie(oldid, m);
                    if (stat != 0){
                        printf("Modify operation failed!\n");
                        continue;
                    }
                    else{
                        printf("Modify operation successful!\n");
                        continue;
                    }
                }

                else if (opt == 5){
                    int id = 0;
                    printf("Enter ID of movie you want to delete: ");
                    fflush(stdin);
                    scanf("%d", &id);
                    int stat = delete_movie(id);
                    if (stat != 0){
                        printf("Delete operation failed\n");
                        continue;
                    }
                    else{
                        printf("Delete operation Successful\n");
                        continue;
                    }
                }

            }

        }
        else if (choice == 0){
            printf("Exiting app\n");
            break;
        }
        else{
            printf("Invalid choice\n");
        }
        // fflush(stdin);
    }
}