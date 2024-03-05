O etsi_client.cpp e o etsi_server.cpp servem apenas para testar a sintaxe das funções criadas no ficheiro etsi_qkd_004.h que serão usadas para que seja possível fazer pedidos open_connect, get_key e close e lidar com eles(handle_open_connect,handle_get_key e handle_close).

Compilação no diretorio build:

cmake ..
make 

Dois terminais no diretorio bin:
Executa primeiro o server:
./etsi_server

Depois o client:
./etsi_client
