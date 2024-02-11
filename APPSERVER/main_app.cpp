#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    // Cria um sinal de saída
    auto outputSignal = std::make_shared<BinarySignal>();

    // Inicializa LoadAscii com o sinal de saída criado
    LoadAscii loadAscii({}, {outputSignal.get()});

    std::cout << "Iniciando..." << std::endl;

    // Configurações adicionais para LoadAscii
    loadAscii.setAsciiFileName("rx_raw0.dat");
    loadAscii.setSymbolPeriod(1);
    loadAscii.setSamplingPeriod(1);
    loadAscii.setDataType(signal_value_type::t_binary);
    loadAscii.setDelimiterType(LoadAscii::delimiter_type::ConcatenatedValues);
    
    // Inicializa e executa LoadAscii
    loadAscii.initialize();

    while (loadAscii.runBlock()) {
        std::cout << "Dentro do loop." << std::endl;
    }

    std::cout << "Leitura do arquivo rx_raw0.dat concluída. Número de valores carregados: "
              << loadAscii.getNumberOfLoadedValues() << std::endl;

    return 0;
}

