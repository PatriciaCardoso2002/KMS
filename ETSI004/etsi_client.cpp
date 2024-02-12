#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "netxpto_20200819.h"
#include "etsi_qkd_004.h"
#include "message_handler.h"
#include "json.hpp"

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

    
    //Criar pedido open_connect
    etsi_qkd_004::URI source = "source";
    etsi_qkd_004::URI destination = "destination";

    etsi_qkd_004::QoS qos;
    qos.key_chunk_size = 3;
    qos.max_bps = 5;
    qos.min_bps = 1;
    qos.jitter = 4;
    qos.priority = 5;
    qos.timeout = 0;
    qos.ttl = 10;
    qos.metadata_mimetype = "metadata";

    etsi_qkd_004::UUID key_stream_id = "key_stream_id";

    //fazer pedido open_connect
    json open_connectJson = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id);

    //Transformar o pedido num sinal do tipo t_handler_message
    t_handler_message openConnectSignal = etsi_qkd_004::open_connectToSignal(open_connectJson);

    //Função para enviar openConnectSignal para o MessageHandler
    

    std::cout << "Chegou aqui" << std::endl;
    close(clientSocket);

    return 0;
}