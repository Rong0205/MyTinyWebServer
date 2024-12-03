#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1234
#define MAX_CLIENTS 1
#define BUFFER_SIZE 1024

int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if(setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd,(struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_CLIENTS)<0){
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server Listening on Port" << std::endl;
    while(true)
    {
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), client_ip,  INET_ADDRSTRLEN);
        uint16_t client_port = ntohs(address.sin_port);

        std::cout << "Connection from "<< client_ip<< ": "<< client_port<<std::endl;

        while(true)
        {
            int valread = recv(new_socket, buffer, BUFFER_SIZE,0);
            if(valread<=0){
                break;
            }

            std::cout << "Received: "<<std::string(buffer, 0, valread) << std::endl;

            send(new_socket, buffer, valread, 0);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}