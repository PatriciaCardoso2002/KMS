#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Erro ao criar o socket do cliente." << std::endl;
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    serverAddr.sin_port = htons(12345);

    
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Erro ao conectar ao servidor." << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Conectado ao servidor." << std::endl;

    
    const char* message = "Olá, servidor!";
    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == -1) {
        std::cerr << "Erro ao enviar dados para o servidor." << std::endl;
    } else {
        std::cout << "Dados enviados para o servidor." << std::endl;
    }

    close(clientSocket);

    return 0;
}
