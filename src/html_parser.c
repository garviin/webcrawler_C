/* 
 * Author: Garvin Prathama Bulkin
 * Student ID: 927842 (gbulkin)
 * Purpose: Contains functions to parse & process HTML content
 */

#include "../include/html_parser.h"

// Takes a URL and returns pointer location to the branch
// Example - in: http://web3.comp30023/simple-branch//c.html 
//           out: /simple-branch//c.html
char *get_url(char buffer[], int match_loc){
    // Initialize string container
    char *url = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    
    // Copy url from buffer, quotation mark '"' indicates end of Href att.
    int k = 0;
    for(int i = match_loc; buffer[i]!= '"'; i++){
        if(k == MAX_URL_LENGTH){
            return NULL;
        }
        url[k] = buffer[i];
        k++;
    }
    // Null pointer to indicate end of string
    url[k] = '\0';
    return url;
}

// Input: HTML content in buffer, pointers to path struct of visited links 
//        & unvisited links
// Function will parse non-duplicate links from fetched HTML into *paths
void parse_href(struct Paths *visited, struct Paths *paths, char *buffer, 
                char *current_hostname, char *current_branch){
    // Check if HTML content fetched returns 200 and is type HTML, if 
    // invalid, ignore page
    if((strcmp(get_status_code(buffer),"200") != 0) ||
       (strcmp(get_MIME(buffer), "text/html") != 0)){
        return;
    } 

    // Initialize regex data types to match HREF tags
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "\\shref\\s*=\\s*\"|\\sHREF\\s*=\\s*\"", REG_EXTENDED);

    // Goes through the regex matches, retrieve distinct URLs, and checks 
    // if its valid (refer to is_valid function)
    int last_match = 0;
    while(regexec(&regex, buffer+last_match, 1, &match, 0) == 0){
        char *retrieved_url = get_url(buffer, match.rm_eo+last_match);
        char *abs_url = construct_full_url(retrieved_url, current_hostname, remove_dot_html(current_branch));
        if(retrieved_url                           // Checks if there is a match
           && is_valid_url(retrieved_url)          // Check URLs validity
           && !(is_duplicate(visited, abs_url))    // Check if URL has been visited
           && !(is_duplicate(paths, abs_url))){    // Checks if it has already been queued
            add_path(paths, abs_url);
        }
        last_match += match.rm_eo;
    }

    // Free memory allocated for regex search
    regfree(&regex);
}

// Returns a string containing the status code from the response header
char *get_status_code(char *buffer){
    char *status_code;
    status_code = malloc((STATUS_CODE_LENGTH+1)*sizeof(char));
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "HTTP/1.1 ", 0);

    // Finds the match of the HTTP header, and copies the status code
    if(regexec(&regex, buffer, 1, &match, 0) == 0){
        for(int i = 0 ; i < 3; i++){
            status_code[i] = buffer[i+match.rm_eo];
        }
        status_code[3] = '\0';
    }

    regfree(&regex);
    return status_code;
}

// Returns a string containing the MIMETYPE from response header
char *get_MIME(char *buffer){
    char *type;
    type = malloc((MAX_MIME_TYPE_LENGTH + 1)*sizeof(char));
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "Content-Type: ", 0);

    // Retrieves 9 characters from MIMEtype as text\html is only considered
    if(regexec(&regex, buffer, 1, &match, 0) == 0){
        int i;
        for(i = 0 ; i < 9; i++){
            type[i] = buffer[i+match.rm_eo];
        }
        type[i] = '\0';
    }

    regfree(&regex);
    return type;
}

// Returns new location to handle status 401
char *get_new_location(char *buffer){
    char *loc;
    loc = malloc((MAX_URL_LENGTH)*sizeof(char));
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "Location: ", 0);
    if(regexec(&regex, buffer, 1, &match, 0) == 0){
        int i;
        // \r indicates end of "Location" header
        for(i = 0 ; buffer[i+match.rm_eo] != '\r'; i++){
            loc[i] = buffer[i+match.rm_eo];
        }
        loc[i] = '\0';
    }

    regfree(&regex);
    return loc;
}

// Checks if URL is valid based on the specifications
int is_valid_url(char *buffer){
    int valid = 1;
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "\\/\\.\\.\\/|\\/\\.\\/|\\?|#|%..", REG_EXTENDED);

    if(regexec(&regex, buffer, 1, &match, 0) == 0){
        valid = 0;
    }

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
    return valid;
}

// Returns the top part of the hostname
// Example - in: web1.comp30023 out: .comp30023
char *get_top_host(char *hostname){
    char *host_copy = malloc((MAX_URL_LENGTH)*sizeof(char));
    strcpy(host_copy, hostname);
    char * token = strtok(host_copy, ".");
    token = strtok(NULL, ".");
    return token;
}

// Checks if the input URL is a relative link with implied protocol
// Example: //web.comp30023/path
int is_relative1(char *url){
    if((url[0] == '/') && (url[1] == '/')){
        return 1;
    }
    return 0;
}

// Checks if the input URL is a relative link with implied protocol & host
// Example: /path/path.html
int is_relative2(char *url){
    if((url[0] == '/') && (url[1] != '/')){
        return 1;
    }
    return 0;
}

// Check if the input URL is relative with implied protocol, host, and path
// Example: path.html
int is_relative3(char *url, char *branch){
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "[a-zA-Z0-9_]*\\.html", 0);

    if((url[0] != '/') 
        && (regexec(&regex, url, 1, &match, 0) == 0) 
        && (strcmp(branch, "") == 0)){
        return 1;
    }
    return 0;
}

// Returns the branch of a URL
// Example - in: google.com/branch/ out: /branch/
char *get_branch(char *url, char *host){
    char *branch;
    branch = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    if(!is_relative2(url)){
        regex_t regex;
        regmatch_t match;
        regcomp(&regex, host, 0);

        if(regexec(&regex, url, 1, &match, 0) == 0){
            int i;
            for(i = 0 ; url[i] != '\0'; i++){
                branch[i] = url[i+match.rm_eo];
            }
            branch[i] = '\0';
        }
        regfree(&regex);
        return branch;
    }
    return url;
}

// Removes protocol from URL
char *remove_protocol(char *url){
    char *result;
    result = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    if(is_absolute(url)){
        strcpy(result, &url[5]);
    } else {
        return url;
    }
    return result;
}

// Removes "//" from relative url with implied protocol
// Example - in: //web.comp30023 out: web.comp30023
char *remove_dbl_slash(char *url){
    char *result;
    result = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    if(is_relative1(url)){
        strcpy(result, &url[2]);
        return result;
    }
    return url;
}

// Removes /something.html" from a branch
// Example - in: /branch/file.html out: /branch/
char *remove_dot_html(char *branch){
    char *result;
    result = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "[a-zA-Z0-9_]*\\.html", 0);
    if(regexec(&regex, branch, 1, &match, 0) != 0){
        return branch;
    }
    int i;
    for(i = 0; i < match.rm_so; i++){
        result[i] = branch[i];
    }
    result[i] = '\0';

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
    return result;
}

// Takes URL and returns the hostname
char *get_host(char *url){
    char *result;
    result = malloc((MAX_URL_LENGTH+1)*sizeof(char));
    char *cleaned = remove_dbl_slash(remove_protocol(url));
    int i = 0;
    while(cleaned[i] != '/'){
        result[i] = cleaned[i];
        i++;
    }
    result[i] = '\0';
    return result;
}

// Takes a url and returns 0 if its absolute
int is_absolute(char *buffer){
    int absolute = 0;
    if(strstr(buffer, "http:")){
        absolute = 1;
    }
    return absolute;
}

// Constructs absolute URL based on given path & parent path
char *construct_full_url(char *url, char* hostname, char* current_branch){
    // If already absolute path, return the original input
    if(is_absolute(url)){
        return url;
    } else if(is_relative1(url)){
        return concat("http:", url);
    }

    char *partial_url = concat("http://", hostname);
    char *clean_url = url;
    if(is_relative3(url, current_branch)){
         clean_url = concat("/", url);
    }

    char *partial_url2 = concat(current_branch, clean_url);

    return concat(partial_url, partial_url2);
}

// Checks if a given URL exists in the given Path struct
int is_duplicate(struct Paths *visited, char *url){
    for(int i = 0; i < visited->count; i++){
        if(strcmp(visited->url[i], url) == 0){
            return 1;
        }
    }
    return 0;
}   

// Adds a '/' character for branches that does not start with one
// Example - in: bear.html out: /bear.html
char *make_branch(char *branch){
    if(branch[0] == '/'){
        return branch;
    }
    char *root = "/";
    return root;
}