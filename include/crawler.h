/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: Header file for crawler, please refer to .c file for
 *          documentation of each function.
 */

#ifndef CRAWLER_H
#define CRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h> 

#include "../include/paths.h"
#include "../include/html_parser.h"
#include "../include/helper_functions.h"

#define MAX_REQUEST_LEN 1024

void send_request(char *buffer, char* hostname, char *current_branch);
void setup_socket(char *buffer, char *hostname, char *req_template);
void print_visits(struct Paths *visited);

#endif