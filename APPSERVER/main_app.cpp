#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>



int main() {
    auto outputSignal = std::make_shared<Binary>();
    LoadAscii loadAscii({}, {outputSignal.get()});
    std::cout << "Iniciando..." << std::endl;

    loadAscii.setAsciiFileName("rx_raw");
    loadAscii.setAsciiFileNameExtension(".dat");
    loadAscii.setAsciiFileNameTailNumber("0");
    loadAscii.setSymbolPeriod(1);
    loadAscii.setSamplingPeriod(1);
    loadAscii.setDataType(signal_value_type::t_binary);
    loadAscii.setDelimiterType(LoadAscii::delimiter_type::ConcatenatedValues);

    loadAscii.initialize();

    while (true) {
        if (outputSignal->space() == 0 || !loadAscii.runBlock()) {
            // Se não há espaço no buffer ou não há mais dados para ler, processar os dados
            outputSignal->reset();

            if (!loadAscii.runBlock()) {
                // Se runBlock() retorna falso, significa que não há mais dados para ler
                break;
            }
        }
    }

    std::cout << "Leitura concluída. Número de valores carregados: " << loadAscii.getNumberOfLoadedValues() << std::endl;

    return 0;
}
