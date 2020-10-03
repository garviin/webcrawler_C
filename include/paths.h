/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: Header file for paths, please refer to .c file for
 *          documentation of each function.
 */
#ifndef PATHS_H
#define PATHS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_URL_LENGTH 1000
#define MAX_TOTAL_URL 100

// Struct path to store all links. Can dynamically add and pop entries.
struct Paths{
    char url[MAX_TOTAL_URL][MAX_URL_LENGTH];
    int count;
};

void add_path(struct Paths *paths, char *new_url);
char *pop_link(struct Paths *paths);
void free_paths(struct Paths *paths);

#endif