#include "load_ascii_20200819.h" // Inclui o cabeçalho para LoadAscii
#include <iostream>

int main() {
    // Inicializa LoadAscii com listas inicializadoras vazias para sinais de entrada e saída
    LoadAscii loadAscii({}, {});

    // Configura LoadAscii para ler o arquivo desejado
    loadAscii.setAsciiFileName("rx_raw0.dat"); // Define o nome do arquivo a ser lido
    loadAscii.setSymbolPeriod(1); // Ajusta o período do símbolo conforme necessário
    loadAscii.setSamplingPeriod(1); // Ajusta o período de amostragem conforme necessário
    
    // Configurações adicionais, se necessário
    loadAscii.setDataType(signal_value_type::t_binary); // Ajusta o tipo de dados esperado, se necessário
    loadAscii.setDelimiterType(LoadAscii::delimiter_type::ConcatenatedValues); // Ajusta o tipo de delimitador, se aplicável
    
    loadAscii.initialize();
    


    while(loadAscii.runBlock()) {
        std::cout << "Dentro do loop." << std::endl;
        // Ou use printf se desejar testar essa saída
        // printf("Dentro do loop.\n");
    }

    // Exibe a quantidade de valores carregados após a leitura ser concluída
    std::cout << "Leitura do arquivo rx_raw0.dat concluída. ";
    std::cout << "Número de valores carregados: " << loadAscii.getNumberOfLoadedValues() << std::endl;
    printf("Leitura do arquivo rx_raw0.dat concluída.\n");
    printf("Número de valores carregados: %lu\n", loadAscii.getNumberOfLoadedValues());

    return 0;
}
