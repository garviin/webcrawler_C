/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: Header file for html_parser, please refer to .c file for
 *          documentation of each function.
 */

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <regex.h>   
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>   
#include <string.h>

#include "../include/paths.h"
#include "../include/crawler.h"

#define MAX_URL_LENGTH 1000
#define MAX_TOTAL_URL 100
#define MAX_BUFF_SIZE 100000
#define STATUS_CODE_LENGTH 3
#define MAX_MIME_TYPE_LENGTH 10

char *get_url(char buffer[], int match_loc);
void parse_href(struct Paths *visited, struct Paths *paths, char *buffer, 
                char *current_hostname, char *current_branch);
char *get_status_code(char *buffer);
char *get_MIME(char *buffer);
char *get_new_location(char *buffer);
int is_valid_url(char *buffer);
char *get_top_host(char *hostname);
int is_relative1(char *url);
int is_relative2(char *url);
int is_relative3(char *url, char *branch);
char *get_branch(char *url, char *host);
char *remove_protocol(char *url);
char *remove_dbl_slash(char *url);
char *remove_dot_html(char *branch);
char *get_host(char *url);
int is_absolute(char *buffer);
char *construct_full_url(char *url, char* hostname, char* current_branch);
int is_duplicate(struct Paths *visited, char *url);
char *make_branch(char *branch);

#endif