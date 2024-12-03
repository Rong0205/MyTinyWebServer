#include <iostream>
#include <cstring>  // for memset
#include <unistd.h> // for close
#include <arpa/inet.h> // for inet_addr, inet_ntop
#include <sys/socket.h> // for socket, connect, send, recv

#define SERVER_IP "127.0.0.1"
#define PORT 1234
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    // 创建socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    // 设置服务器地址
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // 将点分十进制 IP 地址转换为网络地址结构
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // 连接到服务器
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // 发送数据
    const char *message = "Hello, Ross!";
    if (send(client_fd, message, strlen(message), 0) < 0) {
        perror("send failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // 接收数据
    int valread = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (valread < 0) {
        perror("recv failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    // 打印接收到的数据
    std::cout << "Received: " << std::string(buffer, 0, valread) << std::endl;

    // 关闭套接字
    close(client_fd);

    return 0;
}