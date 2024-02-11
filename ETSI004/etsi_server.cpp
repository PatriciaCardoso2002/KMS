#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erro ao criar o socket do servidor." << std::endl;
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); 

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Erro ao vincular o socket à porta." << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Erro ao colocar o socket em modo de escuta." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Aguardando conexão do cliente..." << std::endl;

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        std::cerr << "Erro ao aceitar a conexão do cliente." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Cliente conectado." << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Erro ao receber dados do cliente." << std::endl;
    } else {
        std::cout << "Dados recebidos do cliente: " << buffer << std::endl;
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}
