#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>


namespace webserver {

    char http_header[25] = "HTTP/1.1 200 Ok\r\n";

    class WebServer{
        public:
            int server_fd, socket_descriptor; 
            struct sockaddr_in address;
            int addrlen = sizeof(address);
        
        WebServer(int port)
        {
      
            // Creating socket file descriptor
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                printf("Error: socket() failed\n");
                exit(EXIT_FAILURE);
            }
            
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( port );
            
            memset(address.sin_zero, '\0', sizeof address.sin_zero);
        }


        void handle()
        {
            if ((socket_descriptor = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            {
                printf("Error: accept() failed\n");
                exit(1);
            }
            char buffer[30000] = {0};
            long valread = read( socket_descriptor , buffer, 30000);

            printf("incoming message: \n");
            printf("%s", buffer);

            char *method = strtok(buffer, " ");
            printf("METHOD: %s\n", method);

            char *path = strtok(NULL, " ");  
            printf("PATH:   %s\n", path);

            char *response = (char *)malloc(strlen(http_header) +200);
            strcpy(response, http_header);
        }
    };
}