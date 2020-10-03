/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: A simple webcrawler made for Computer Systems (COMP30023) Project 1
 */

#include "../include/crawler.h"

int main(int argc, char ** argv)
{
    // Buffer to store fetched HTML content
    char buffer[MAX_BUFF_SIZE];

    // Paths data structure to track visited & unvisited 
    struct Paths *visited = malloc(sizeof(struct Paths));
    struct Paths *unvisited = malloc(sizeof(struct Paths));

    // Intialize path structs and visit counts
    add_path(visited, argv[1]);
    unvisited->count = 0;
    int visit_count = 1;

    // Extract hostname & branch provided in the argument
    char *hostname = get_host(argv[1]);
    char *branch = get_branch(argv[1], hostname);

    // Error control if used without arguments
    if (argc < 2)
    {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }
    
    // Send initial request to provided link in the argument
    char *current_host = hostname;
    char *current_branch = branch;
    
    // Sends GET request to fetch given url
    send_request(buffer, hostname, branch);

    //Parses in the HREF attributes from the fetched HTML document
    parse_href(visited, unvisited, buffer, hostname, branch);

    // Loop through unvisited links until its empty or exceeds 100 visits
    while((unvisited->count > 0) && (visit_count <= 100)){
        // Take link from the unvisited paths
        char *current_link = pop_link(unvisited);

        // If the url is not a duplicate, crawl the link
        if(!(is_duplicate(visited, current_link))){
            // Adds url to list of visited urls
            add_path(visited, current_link);

            // Gets host & branch for sending request
            current_host = get_host(current_link);
            current_branch = get_branch(current_link, current_host);

            // Sends request to fetch the current url
            send_request(buffer, current_host, current_branch);

            // Parse in all unvisited links
            parse_href(visited, unvisited, buffer, current_host, current_branch);

            visit_count++;
        }
    }

    // Print visited urls
    print_visits(visited);

    // Free memory pointers used to track URLs
    free(visited);
    free(unvisited);
    return 0;
}

// Prints all urls in given Paths struct
void print_visits(struct Paths *visited){
    for(int i = 0; i < visited->count; i++){
        printf("%s\n", visited->url[i]);
    }
}

// Send in a GET request to fetch into the buffer
void send_request(char *buffer, char* hostname, char *current_branch){
    // Creates header based on given hostname & current branch
    char header1[] = "GET ";
    char header2[] = " HTTP/1.1\r\nHost: %s\r\nUser-Agent: gbulkin\r\nConnection: close\r\n\r\n";
    char *branch = make_branch(current_branch);
    char *req_template = concat(concat(header1, branch), header2);

    // Creates a socket and sends GET request using given hostname & branch
    setup_socket(buffer, hostname, req_template);

    // Handling diferent status codes
    // 401: Adds Basic Authorization header with user:pw in base64 encoding
    if(strcmp(get_status_code(buffer),"401") == 0){
        char auth_header[] = " HTTP/1.1\r\nHost: %s\r\nUser-Agent: gbulkin\r\nAuthorization: Basic Z2J1bGtpbjpwYXNzd29yZA==\r\nConnection: close\r\n\r\n";
        char *req_template = concat(concat(header1, branch),auth_header);
        setup_socket(buffer, hostname, req_template);
    } 
    // 501: Resends request to refetch page
    else if(strcmp(get_status_code(buffer),"503") == 0){
        setup_socket(buffer, hostname, req_template);
    } 
    // 301: Resend GET request to the new location
    else if(strcmp(get_status_code(buffer), "301") == 0){
        char *new_loc = get_new_location(buffer);
        char *new_host = get_host(new_loc);
        branch = get_branch(new_loc, new_host);
        char *req_template = concat(concat(header1, branch),header2);
        setup_socket(buffer, new_host, req_template);
    }
}

// Sets up socket and sends in the given request
void setup_socket(char *buffer, char *hostname, char *request_header){
    char request[MAX_REQUEST_LEN];
    int sockfd, portno = 80;
    struct sockaddr_in serv_addr;
    struct hostent * server;
    
    // Retrieves hostname & writes the request header into request buffer
    server = gethostbyname(hostname);
    int request_len = snprintf(request, MAX_REQUEST_LEN, request_header, hostname);

    // Check if hostname lookup was successful
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Creating TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation was successful
    if (sockfd < 0){
        perror("ERROR opening socket");
        exit(0);
    }

    // Establish socket connection to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR connecting");
        exit(0);
    }
   
   // Sending the GET request through
   int bytes_processed = 0, n;
    while (bytes_processed < request_len) {
        n = write(sockfd, request + bytes_processed, request_len - bytes_processed);
        bytes_processed += n;
    }

    // Reads the response into the buffer
    while(bytes_processed > 0){
        bytes_processed = read(sockfd, buffer, BUFSIZ);
    }

    // Closing socket
    close(sockfd); 
}