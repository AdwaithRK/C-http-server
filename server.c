// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

struct params
{
    char *path;
    char *query;
} current_params, empty_params;

char *http_header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

char * get_header(const char *buffer)
{
    int i=0, j=0;

    while(buffer[i] != '\n'){
        i++;
    }

    char *temp = (char*)malloc((i+1)*sizeof(char));;

    while(j <= i){
        temp[j] = buffer[j];
        j++;
    }
    temp[j] = '\0';

    return temp;
}


int parse(const char* buffer)
{
    const char *line = get_header(buffer);
    const char *start_of_path = strchr(line, ' ') + 1;
    const char *start_of_query;
    const char *end_of_query;
    char *path, *query;
    if( start_of_path ){
        start_of_query = strchr(start_of_path, '?');
    }
    if( start_of_query) {
        end_of_query = strchr(start_of_query, ' ');
    }

    if( start_of_path && start_of_query ){
        int path_length = start_of_query - start_of_path;
        path = (char*)malloc((path_length)*sizeof(char));
        strncpy(path, start_of_path,  start_of_query - start_of_path);
        path[path_length] = 0;
        current_params.path = path;
    }

    if( end_of_query && start_of_query ){
        int path_length = end_of_query - start_of_query;
        query = (char*)malloc((path_length)*sizeof(char));
        strncpy(query, start_of_query, end_of_query - start_of_query);
        query[path_length] = 0;
        current_params.query = query;
    }

}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Read html file
    FILE *f = fopen ("index.html", "rb");


    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    int tempLen = strlen(http_header) + strlen(string) + 2;

    char *content = (char*)malloc(tempLen*sizeof(char));

    strcpy( content, http_header);
    strcat( content, string );

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        current_params = empty_params;
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("-----------------Request--------------------");
        printf("\n%s\n",buffer);
        printf("--------------------------------------------");
        parse(buffer);
        write(new_socket , content , strlen(content));
        // get_header(buffer);
        close(new_socket);
        printf("\n------------Connection Closed----------------\n");
        printf("\n----------------------Params--------------------------\n");
        printf("\n    ----- Path: %s --------\n", current_params.path );
        printf("\n    ----- Query: %s -------\n", current_params.query);
    }
    return 0;
}