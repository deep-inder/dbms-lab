#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "movie.h"


void print_movie_info(struct Movie* m){
    printf("-Movie ID: %d\n", m->movie_id);
    printf("-Movie Number: %d\n", m->movie_number);
    printf("-Movie Name: %s\n", m->movie_name);
    printf("-Release Year: %d\n", m->release_year);
    printf("-IMDB rating: %f\n", m->IMDB_rating);
    printf("-Runtime: %d\n", m->runtime);
    printf("-Budget: %d\n", m->budget);
    printf("-Revenue: %d\n", m->revenue);
    printf("-Genre: %s\n", m->genre);
    printf("-Director: %s\n", m->director);
    printf("-Screenwriter: %s\n", m->screenwriter);
    printf("-Producer: %s\n", m->producer); 
    return;
}

int create_db(char* dbname){
    return pds_open(dbname, sizeof(struct Movie));
}

int add_movie(struct Movie* m){
    int status;
    status = put_rec_by_key(m->movie_id, m);
    if( status != PDS_SUCCESS ){
		fprintf(stderr, "Unable to add movie with key %d. Error %d", m->movie_id, status );
		return MOVIE_FAILURE;
	}
	return status;
}

int search_movie_by_id(int id, struct Movie* m){
    return get_rec_by_ndx_key(id, m);
}

int search_movie_by_name(char* movie_name, struct Movie* m){
    int status;
    int count;
    if (m == NULL){
        return MOVIE_FAILURE;
    }
    status = get_rec_by_non_ndx_key(movie_name, m, (int (*)(void*, void*))match_movie_name, &count);
    if (status == 0) return MOVIE_SUCCESS;
    else{
        // printf("PDS error. Error code %d returned.", status);
        return MOVIE_FAILURE;
    }
}

int match_movie_name(struct Movie* m, char* name){
    if (m == NULL || name == NULL){ 
        return 6969;
    }
    else {
        return strcmp(m->movie_name, name);
    }
}

int modify_movie(int id, struct Movie* m){
    return modify_rec_by_key(id, m);
}

int delete_movie(int id){
    return delete_rec_by_ndx_key(id);
}

void fill_movie_attributes(int id, struct Movie* m){
    char mid[100];
    sprintf(mid, "%d", id);
    // printf("%s", mid);
    m->movie_id = id;
    m->movie_number = id;
    m->budget = id;
    m->IMDB_rating = id;
    m->release_year = id;
    m->revenue = id;
    m->runtime = id;
    char name[100] = "Name-of-";
    strcat(name, mid);
    char dir[100] = "Director-of-";
    strcat(dir, mid);
    char pro[100] = "Producer-of-";
    strcat(pro, mid);
    // printf("%s \n", pro);
    char genre[100] = "Genre-of-";
    strcat(genre, mid);
    char scr[100] = "Screenwriter-of-";
    strcat(scr, mid);
    strcpy(m->movie_name, name);
    strcpy(m->director, dir);
    strcpy(m->producer, pro);
    strcpy(m->genre, genre);
    strcpy(m->screenwriter, scr);
    return;
}

void close_db(){
    pds_close();
}



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

