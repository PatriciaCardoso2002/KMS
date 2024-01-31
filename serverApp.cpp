#include <iostream>
#include <fstream>
#include <thread>
#include "etsi_qkd_004.hpp"




int main() {
    //Leitura do ficheiro 
    std::ifstream binaryFile("arquivo_binario.bin", std::ios::binary);
    if(!binaryFile.is_open()){
        std::cerr << "Erro ao abrir o arquivo binario." << std::endl;
        return -1;
    }
    binaryFile.close();

    return 0;
}