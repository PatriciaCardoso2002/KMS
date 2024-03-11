#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "load_request.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"

int main(){
    
    // sinal do tipo HandlerMessage entra no Message Handler TX da app_client como input signal,
    // este sinal é uma conversão do json relativo a cada request ETSI 004
    // o output signal do message handler será o mesmo (?)
    // O sinal é enviado pelo IP Tunnel até ao Message Handler RX da app_server (input signal)
    // que o coloca como output signal para a app processar a mensagem (converte para json e dá handle ao pedido)
    // a resposta a este pedido é gerada, convertida para sinal do tipo HandlerMessage e colocado como input Signal
    // do MessageHandler TX da app_server. Será o mesmo output sinal e daí é enviado pelo IP Tunnel para o Message Handler RX da app_client
    // passa para output sinal e esse output sinal será processado como anteriormente.
    

    // o loadAscii vai abrir um ficheiro e ler do ficheiro, pensei em fazer um loadRequest que em vez de receber o nome do ficheiro, abrir e ler,
    // recebe o tipo do request ETSI, request esse que é construido com base na classe ETSI.

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Message open_connect{"Server_open_connect.sgn",10,hType,sWriteMode};
    HandlerMessage open_connect_{"Server_open_connect_.sgn",10,hType,sWriteMode};
    Message response{"Server_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Server_response_.sgn",10,hType,sWriteMode};

    LoadRequest LoadRequest_Rx{{},{&response}};
    LoadRequest_Rx.setRequest("RESPOSTA");

    IPTunnel IPTunnel_Server_Rx{{},{&open_connect_}};
    IPTunnel_Server_Rx.setLocalMachineIpAddress("127.0.0.1");
    IPTunnel_Server_Rx.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnel_Server_Rx.setLocalMachinePort(54000);
    IPTunnel_Server_Rx.setVerboseMode(true);

    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
    MessageHandler MessageHandlerTX{ {&response},{&response_},FUNCTIONING_AS_TX,ittTxTransmitter};
    
    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Msg_Rx", 0);
    MessageHandler MessageHandlerRX{ {&open_connect_},{&open_connect},dttRxTransmitter,FUNCTIONING_AS_RX};

    IPTunnel IPTunnel_Server_Tx{{&response_},{}};
    IPTunnel_Server_Tx.setLocalMachineIpAddress("127.0.0.1");
    IPTunnel_Server_Tx.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnel_Server_Tx.setRemoteMachinePort(54001);
    IPTunnel_Server_Tx.setVerboseMode(true);

   
    
    // if (!open_connect.getBufferEmpty()){
    //     t_message* msg = open_connect.bufferGet();
    //     t_string data = msg->getMessageData();
    //     std::cout << "Request:" << data << std::endl;
    // }

    System System_
            {
                {
                &LoadRequest_Rx,
                &MessageHandlerRX,
                &MessageHandlerTX,
                //&IPTunnel_Server_Tx,
                &IPTunnel_Server_Rx,
                }
            };
    System_.setSignalsFolderName("signals-server");
    System_.setLogFileName("log_server.txt");

    //  Console Console_;
    // BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
    // Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
    // Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
    // Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
    // Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
    // Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
    // Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

    System_.run();
    System_.terminate();

    return 0;
}