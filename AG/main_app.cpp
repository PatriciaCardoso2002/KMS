#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"



int main() {
    
    //message_handler() do get key; RX (input etsi(open_connect,get_key,close,status), output parametros)
    auto inputSignalRX = std::make_shared<Message>();
    auto outputSignalRX = std::make_shared<HandlerMessage>();
    DestinationTranslationTable destinationTranslationTableRX;
    destinationTranslationTableRX.add("destinationBlockRX", 0);
    InputTranslationTable inputDescriptionRX;
    inputDescriptionRX.add(0, {"destinationBlockRX", "sourceBlock", 5, 0});
    MessageHandler messageHandlerRX({inputSignalRX.get()}, {outputSignalRX.get()}, 
                                  destinationTranslationTableRX, FUNCTIONING_AS_RX, inputDescriptionRX);
    messageHandlerRX.initialize();
    
    //handle callback etsi;

    //message handler tx
    
    //INITIALIZE LOADASCII
    auto outputSignalLoadAscii = std::make_shared<Binary>();
    LoadAscii loadAscii({}, {outputSignalLoadAscii.get()});
    std::cout << "Iniciando..." << std::endl;
    loadAscii.setAsciiFileName("tx_raw");
    loadAscii.setAsciiFileNameExtension(".dat");
    loadAscii.setAsciiFileNameTailNumber("0");
    loadAscii.setSymbolPeriod(1);
    loadAscii.setSamplingPeriod(1);
    loadAscii.setDataType(signal_value_type::t_binary);
    loadAscii.setDelimiterType(LoadAscii::delimiter_type::ConcatenatedValues);
    loadAscii.initialize();



    //message handler rx

    //INITIALIZE MESSAGE HANDLER TX
    auto outputSignalTX = std::make_shared<Binary>();
    DestinationTranslationTable destinationTranslationTable;
    destinationTranslationTable.add("destinationBlockTX", 0);
    InputTranslationTable inputDescription;
    inputDescription.add(0, {"destinationBlockTX", "sourceBlockTX", 5, 0});
    
    MessageHandler messageHandlerTX({outputSignalLoadAscii.get()}, {outputSignalTX.get()}, 
                                  destinationTranslationTable, FUNCTIONING_AS_TX, inputDescription);
    messageHandlerTX.initialize();
    
    //IP TUNNEL

    
    //INITIALIZE MESSAGE HANDLER RX
    /*
    auto inputSignalRX = std::make_shared<Message>();
    auto outputSignalRX = std::make_shared<Binary>();
    DestinationTranslationTable destinationTranslationTableRX;
    destinationTranslationTableRX.add("destinationBlockRX", 0);
    InputTranslationTable inputDescriptionRX;
    inputDescriptionRX.add(0, {"destinationBlockRX", "sourceBlock", 5, 0});
    MessageHandler messageHandlerRX({inputSignalRX.get()}, {outputSignalRX.get()}, 
                                  destinationTranslationTableRX, FUNCTIONING_AS_RX, inputDescriptionRX);
    messageHandlerRX.initialize();
    
    
    SaveAscii saveAscii({outputSignalTX.get()}, {});
    saveAscii.setAsciiFileName("rx_raw");
    saveAscii.setAsciiFileNameExtension("dat");
    saveAscii.setAsciiFileNameTailNumber("0");
    saveAscii.setAsciiFolderName("key_files");
    saveAscii.setAsciiFileNameTailNumberModulos(0);
    saveAscii.setDataType(signal_value_type::t_binary);
    saveAscii.setDelimiterType(SaveAscii::delimiter_type::ConcatenatedValues);
    saveAscii.initialize();
    */
    
    int i = 0;
    while (true) {
        //buffer size 512
        if (outputSignalLoadAscii->space() == 0 || !loadAscii.runBlock()) {
        
            //ip tunnel
            messageHandlerTX.runBlock();
            //-> etsi
            i++;
            printf("i = %d\n", i);
            
            // Se não há espaço no buffer ou não há mais dados para ler, processar os dados
            outputSignalLoadAscii->reset();
            
            if (!loadAscii.runBlock()) {
                // Se runBlock() retorna falso, significa que não há mais dados para ler
                break;
            }
        }
    }
    std::cout << "Leitura concluída. Número de valores carregados: " << loadAscii.getNumberOfLoadedValues() << std::endl;
  
    
    

    return 0;
}
