O etsi_client.cpp e o etsi_server.cpp servem apenas para testar a sintaxe das funções criadas no ficheiro etsi_qkd_004.h que serão usadas para que seja possível fazer pedidos open_connect, get_key e close e lidar com eles(handle_open_connect,handle_get_key e handle_close).

Compilação:

g++ -std=c++17 etsi_server.cpp -o etsi_server
 
g++ -std=c++17 etsi_client.cpp -o etsi_client

Dois terminais:
Executa primeiro o server:
./etsi_server

Depois o client:
./etsi_client
