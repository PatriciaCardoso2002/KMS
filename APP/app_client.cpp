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
    Message open_connect{"Client_open_connect.sgn",10,hType,sWriteMode};
    HandlerMessage open_connect_{"Client_open_connect_.sgn",10,hType,sWriteMode};
    Message response{"Client_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Client_response_.sgn",10,hType,sWriteMode};
    
    LoadRequest LoadRequest_Tx{ {},{&open_connect} };
    LoadRequest_Tx.setRequest("REQUEST");

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Msg_Tx", 0);
    MessageHandler MessageHandlerClientRX{ {&response_},{&response},dttRxTransmitter,FUNCTIONING_AS_RX};
    
    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
    MessageHandler MessageHandlerClientTX{ {&open_connect},{&open_connect_},FUNCTIONING_AS_TX,ittTxTransmitter};
    
   

    // std::cout << "Empty? " << open_connect_.getBufferEmpty() << std::endl;

    // if (!open_connect_.getBufferEmpty()){
    //     t_handler_message* msg = open_connect_.bufferGet();
    //     t_string data = msg->getMessageData();
    //     std::cout << "Request:" << data << std::endl;
    // }

    IPTunnel IPTunnelClient_Client{{&open_connect_},{}};
    IPTunnelClient_Client.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelClient_Client.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelClient_Client.setRemoteMachinePort(54000);
    IPTunnelClient_Client.setVerboseMode(true);

    IPTunnel IPTunnelClient_Server{{},{&response_}};
    IPTunnelClient_Server.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelClient_Server.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelClient_Server.setLocalMachinePort(54001);
    IPTunnelClient_Server.setVerboseMode(true);

    System System_
            {
                {
                &LoadRequest_Tx,
                &MessageHandlerClientRX,
                &MessageHandlerClientTX,
                &IPTunnelClient_Client,
                &IPTunnelClient_Server,
                }
            };
    // System_.setLogFileName("log_client.txt");

    //  Console Console_;
    // BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
    // Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
    // Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
    // Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
    // Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Client, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
    // Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Client, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
    // Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

    System_.run();
    System_.terminate();

    return 0;
}