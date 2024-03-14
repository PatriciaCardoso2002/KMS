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
#include "receive_request.h"

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

    Message request{"Server_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"Server_request_.sgn",10,hType,sWriteMode};
    Message response{"Server_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Server_response_.sgn",10,hType,sWriteMode};

    IPTunnel IPTunnelServer_Server{{},{&request_}};
    IPTunnelServer_Server.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelServer_Server.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelServer_Server.setLocalMachinePort(54000);
    IPTunnelServer_Server.setVerboseMode(true);

    // LoadRequest LoadRequest_Rx{{},{&response}};
    // LoadRequest_Rx.setRequest("{\"command\":\"OPEN_CONNECT_RESPONSE\",\"data\":{\"source\":\"source\",\"destination\":\"destination\",\"qos\":{\"key_chunk_size\":3,\"max_bps\":5,\"min_bps\":1,\"jitter\":4,\"priority\":5,\"timeout\":0,\"ttl\":10,\"metadata_mimetype\":\"metadata\"},\"key_stream_id\":\"key_stream_id\"}}");

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Msg_Rx", 0);
    MessageHandler MessageHandlerServerRX{ {&request_},{&request},dttRxTransmitter,FUNCTIONING_AS_RX};

    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
    MessageHandler MessageHandlerServerTX{ {&response},{&response_},FUNCTIONING_AS_TX,ittTxTransmitter};
    
    ReceiveRequest receiveRequest_Rx{{&request}, {&response}};
    receiveRequest_Rx.setVerboseMode(true);

    IPTunnel IPTunnelServer_Client{{&response_},{}};
    IPTunnelServer_Client.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelServer_Client.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelServer_Client.setRemoteMachinePort(54001);
    IPTunnelServer_Client.setVerboseMode(false);

    

   


   
    
    // if (!request.getBufferEmpty()){
    //     t_message* msg = request.bufferGet();
    //     t_string data = msg->getMessageData();
    //     std::cout << "Request:" << data << std::endl;
    // }

    System System_
            {
                {
                //&LoadRequest_Rx,
                &MessageHandlerServerRX,
                &receiveRequest_Rx,
                &MessageHandlerServerTX,
                &IPTunnelServer_Client,
                &IPTunnelServer_Server,
                }
            };
    // System_.setSignalsFolderName("signals-server");
    // System_.setLogFileName("log_server.txt");

    //  Console Console_;
    // BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
    // Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
    // Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnelServer_Client, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
    // Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnelServer_Client, std::mem_fn(&IPTunnel::getLocalMachinePort) };
    // Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnelServer_Client, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
    // Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnelServer_Client, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
    // Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

    System_.run();
    System_.terminate();

    return 0;
}