#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <netinet/in.h>


namespace httpserver {

    typedef std::string (*CALLBACK_TYPE) (std::string method, std::string path);

    const std::string HTTP_HEADER_OK =      "HTTP/1.1 200 Ok\r\n"
                                            "Content-Type: text/html\r\n\r\n";
    const std::string HTTP_HEADER_404 =     "HTTP/1.1 404 Not Found\r\n"
                                            "Content-Type: text/html\r\n\r\n";

    class HttpServer{
        private:
            CALLBACK_TYPE callback = NULL;
        public:
            int server_fd, socket_descriptor; 
            struct sockaddr_in address;
            int addrlen = sizeof(address);


        
        HttpServer(int port)
        {
            // Creating socket file descriptor
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                printf("Error: socket() failed\n");
                exit(EXIT_FAILURE);
            }
            int iSetOption = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
            
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( port );

            printf("HttpServer constructor port=%d\n", port);
            
            memset(address.sin_zero, '\0', sizeof address.sin_zero);

            int retry_count = 0;
            while (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
            {
                retry_count ++;
                if(retry_count > 3){
                    printf("Retry count has reached the limit. Giving up \n");
                    break;
                }
                printf("Error: failed to bind port %d. Retry in 3 seconds.. \n", port);
                sleep(3);
            }
            if (listen(server_fd, 10) < 0)
            {
                printf("Error: listen() failed\n");
                exit(EXIT_FAILURE);
            }

        }

        void setCallback(CALLBACK_TYPE clbk)
        {
            this->callback = clbk;
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

            printf("incoming message: '%s'\n", buffer);

            char *method = strtok(buffer, " ");
            char *path = strtok(NULL, " ");  

            std::string payload = "";
            if(this->callback && method && path){
                payload = this->callback(method, path);
            }

            std::string response = "";
            
            if(payload.rfind("HTTP/1.1 ", 0) != 0){
                response.append(HTTP_HEADER_OK);
            }
            
            response.append(payload);
            write(socket_descriptor, response.data(), response.length());
            //printf("write to socket. RESP=\n%s\n\n", response.data());
            
            close(socket_descriptor);
        }


        ~HttpServer()
        {
            printf("~HttpServer()\n");
        }
    };
}