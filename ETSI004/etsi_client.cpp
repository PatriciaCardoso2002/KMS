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

    std::cout << "open_connect passou teste sintaxe" << std::endl;

    // Teste get_key
    etsi_qkd_004::UUID key_stream_id_get_key = "key_stream_id_for_get_key";
    unsigned int index_get_key = 42;
    etsi_qkd_004::Metadata metadata_get_key = {10, "example_metadata"};
    json get_keyJson = etsi_qkd_004::get_key(key_stream_id_get_key, index_get_key, metadata_get_key);

    std::cout << "get_key passou teste sintaxe" << std::endl;

    // Teste close
    etsi_qkd_004::UUID key_stream_id_close = "key_stream_id_for_close";
    json closeJson = etsi_qkd_004::close(key_stream_id_close);

    std::cout << "close passou teste sintaxe" << std::endl;

    //Teste handle_open_connect
    etsi_qkd_004::UUID key_stream_id_handle_open_connect = "key_stream_id_for_handle_open_connect";
    etsi_qkd_004::QoS qos_handle_open_connect = {5, 10, 1, 4, 5, 0, 10, "metadata_type"};
    etsi_qkd_004::Status status_handle_open_connect = etsi_qkd_004::SUCCESSFUL;
    json handle_open_connectJson = etsi_qkd_004::handle_open_connect(key_stream_id_handle_open_connect, qos_handle_open_connect, status_handle_open_connect);

    std::cout << "handle_open_connect passou teste sintaxe" << std::endl;


    //Teste handle_get_key
    etsi_qkd_004::Status status_handle_get_key = etsi_qkd_004::SUCCESSFUL;
    etsi_qkd_004::KeyBuffer key_buffer_handle_get_key = {0x01, 0x02, 0x03};
    unsigned int index_handle_get_key = 10;
    etsi_qkd_004::Metadata metadata_handle_get_key = {15, "another_metadata"};
    json handle_get_keyJson = etsi_qkd_004::handle_get_key(status_handle_get_key, key_buffer_handle_get_key, index_handle_get_key, metadata_handle_get_key);


    std::cout << "handle_get_key passou teste sintaxe" << std::endl;

    //Teste handle_close
    etsi_qkd_004::Status status_handle_close = etsi_qkd_004::SUCCESSFUL;
    json handle_closeJson = etsi_qkd_004::handle_close(status_handle_close);

    std::cout << "handle_close passou teste sintaxe" << std::endl;

    close(clientSocket);

    return 0;
}