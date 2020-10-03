/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: Defines relevant functions to manipulate Paths struct
 */
#include "../include/paths.h"

// Adds given url to given Path struct, and updates the path count
void add_path(struct Paths *paths, char *new_url){
    strcpy(paths->url[paths->count],new_url);
    paths->count++;
}

// Returns a pointer to the most recently added URL, and decrements path count
char *pop_link(struct Paths *paths){
    paths->count--;
    char *link = paths->url[paths->count];
    return link;
}

// Frees up memory for each URL in path struct;
void free_paths(struct Paths *paths){
    for(int i = 0; i < paths->count; i++){
        free(paths->url[i]);
    }
}