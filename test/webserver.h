#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <netinet/in.h>


namespace webserver {

    typedef std::string (*CALLBACK_TYPE) (std::string method, std::string path);

    std::string http_header =   "HTTP/1.1 200 Ok\r\n"
                                "Content-Type: text/html\r\n\r\n";

    class WebServer{
        private:
            CALLBACK_TYPE callback = NULL;
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
            int iSetOption = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
            
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( port );

            printf("WebServer constructor port=%d\n", port);
            
            memset(address.sin_zero, '\0', sizeof address.sin_zero);

            while (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
            {
                printf("Error: bind() failed\n");
                sleep(3);
                //close(server_fd);
                //exit(EXIT_FAILURE);
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


            // Put the socket in non-blocking mode:
            // if(fcntl(socket_descriptor, F_SETFL, fcntl(socket_descriptor, F_GETFL) | O_NONBLOCK) < 0) {
            //     // handle error
            // }


            char buffer[30000] = {0};
            long valread = read( socket_descriptor , buffer, 30000);

            printf("incoming message: \n");
            printf("%s", buffer);

            char *method = strtok(buffer, " ");
            printf("METHOD: %s\n", method);

            char *path = strtok(NULL, " ");  
            printf("PATH:   %s\n", path);

            std::string payload = "";
            if(this->callback){
                payload = this->callback(method, path);
                //printf("callback call. PAYLOAD=%s\n", payload.data());
            }

            std::string response = "";
            response.append(http_header);
            response.append(payload);
            write(socket_descriptor, response.data(), response.length());
            printf("write to socket. RESP=\n%s\n\n", response.data());


            close(socket_descriptor);
        }


        ~WebServer()
        {
            printf("~WebServer()\n");
        }
    };
}