#ifndef MOVIE_H_
#define MOVIE_H_

#define MOVIE_SUCCESS 0
#define MOVIE_FAILURE 1

struct Movie{
    int movie_id;
    int movie_number;
    char movie_name[50];
    int release_year;
    float IMDB_rating;
    int runtime;
    int budget;
    int revenue;
    char genre[50];
    char director[50];
    char screenwriter[50];
    char producer[50];
};

extern struct PDS_RepoInfo* repoHandle;

int create_db(char* dbname);

int add_movie(struct Movie* m);

void print_movie_info(struct Movie* m);

int search_movie_by_id(int movie_id, struct Movie* m);

int search_movie_by_name(char* movie_name, struct Movie* m);

int modify_movie(int id, struct Movie* m);

int delete_movie(int id);

int match_movie_name(struct Movie* m, char* name);

void fill_movie_attributes(int id, struct Movie* m);

void close_db();

#endif