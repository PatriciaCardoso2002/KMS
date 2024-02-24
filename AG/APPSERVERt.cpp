#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"


int main() {
    //outputAPP1 tx tx -> iptunnel/ rx tx / rx rx / tx rx / tx tx -> ip tunnel
    //message_handler() do get key; RX (input etsi(open_connect,get_key,close,status), output parametros)
    Binary outputSignalLoadAscii{ "S0_Raw_Key_Tx.sgn", (t_unsigned_long)512};
    auto inputSignalRX = std::make_shared<Message>();
    auto outputSignalRX = std::make_shared<HandlerMessage>();
    auto outputSignalTX = std::make_shared<Binary>();
    
    //INITIALIZE LOADASCII
    LoadAscii loadAscii({}, {&outputSignalLoadAscii});
    std::cout << "Iniciando..." << std::endl;
    loadAscii.setAsciiFileName("tx_raw");
    loadAscii.setAsciiFileNameExtension(".dat");
    loadAscii.setAsciiFileNameTailNumber("0");
    loadAscii.setSymbolPeriod(1);
    loadAscii.setSamplingPeriod(1);
    loadAscii.setDataType(signal_value_type::t_binary);
    loadAscii.setDelimiterType(LoadAscii::delimiter_type::ConcatenatedValues);
    
    
    //MESSAGE HANDLER RX TX
    DestinationTranslationTable destinationTranslationTableRX;
    destinationTranslationTableRX.add("destinationBlockRX", 0);
    InputTranslationTable inputDescriptionRX;
    inputDescriptionRX.add(0, {"destinationBlockRX", "sourceBlock", 5, 0});
    MessageHandler messageHandlerRX({inputSignalRX.get()}, {outputSignalRX.get()}, destinationTranslationTableRX, FUNCTIONING_AS_RX, inputDescriptionRX);
    messageHandlerRX.initialize();
    //MESSAGE HANDLER RX RX



    //MESSAGE HANDLER TX RX
    
    //MESSAGE HANDLER TX TX
    
    DestinationTranslationTable destinationTranslationTable;
    destinationTranslationTable.add("destinationBlockTX", 0);
    InputTranslationTable inputDescription;
    inputDescription.add(0, {"destinationBlockTX", "sourceBlockTX", 5, 0});
    MessageHandler messageHandlerTX({&outputSignalLoadAscii}, {outputSignalTX.get()}, destinationTranslationTable, FUNCTIONING_AS_TX, inputDescription);
    messageHandlerTX.initialize();
  
 
    
    //IP TUNNEL para a interface RX (recebe da interface TX para a sua propria interface TX que envia para a interface RX)
    IPTunnel IPTunnelServerRX({}, {outputSignalTX.get()});
    IPTunnelServerRX.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelServerRX.setRemoteMachinePort(54000);
    IPTunnelServerRX.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelServerRX.setLocalMachinePort(54001);

    IPTunnel IPTunnelClientTX({outputSignalRX.get()}, {});
    IPTunnelClientTX.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelClientTX.setRemoteMachinePort(54001);
    IPTunnelClientTX.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelClientTX.setLocalMachinePort(54000);
    


    /*
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
        if ( outputSignalLoadAscii.space() == 0 || !loadAscii.runBlock()) {
        
            //ip tunnel
            messageHandlerTX.runBlock();
            saveAscii.runBlock();
            //-> etsi
            i++;
            printf("i = %d\n", i);
            
            // Se não há espaço no buffer ou não há mais dados para ler, processar os dados
            outputSignalLoadAscii.reset();
            
            if (!loadAscii.runBlock()) {
                // Se runBlock() retorna falso, significa que não há mais dados para ler
                break;
            }
        }
    }
    std::cout << "Leitura concluída. Número de valores carregados: " << loadAscii.getNumberOfLoadedValues() << std::endl;
    BlockGetFunction<std::_Mem_fn<std::string(LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
		Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);
    
    

    return 0;
}
