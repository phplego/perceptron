#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "definitions.h"

#define PORT 8089

char http_header[25] = "HTTP/1.1 200 Ok\r\n";


int main(int argc, char * argv []) 
{ 
    pf_green("HTTP Server test..\n");

    int server_fd, new_socket, pid; 
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        pf_red("Error: socket() failed\n");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    while (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        pf_red("Error: bind() failed\n");
        sleep(3);
        //close(server_fd);
        //exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        pf_red("Error: listen() failed\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        pf_magenta("\nWaiting for connection...\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            pf_red("Error: accept() failed\n");
            exit(1);
        }
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);

        printf("incoming message: \n");
        pf_gray("%s", buffer);

        char *method = strtok(buffer, " ");
        pf_blue("METHOD: %s\n", method);

        char *path = strtok(NULL, " ");  
        pf_blue("PATH:   %s\n", path);

        char *response = (char *)malloc(strlen(http_header) +200);
        strcpy(response, http_header);

        if(strcmp("GET", method) == 0){
            strcat(response, "Content-Type: text/html\r\n\r\n");
            strcat(response, "my response ");
            strcat(response, method);
            strcat(response, " path: ");
            strcat(response, path);
            write(new_socket, response, strlen(response));
            //write(new_socket, path, strlen(path));
            pf("<<< RESPONSE SENT\n");
        }
        
        if(strcmp("POST", method) == 0)
        {
            // POST
        }

        close(new_socket);
        free(response);  
    }

    
}