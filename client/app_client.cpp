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



    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Binary };
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };
    Binary open_connect{"requestin.sgn",10,hType,sWriteMode};
    HandlerMessage open_connect_{"requestout.sgn",10,hType,sWriteMode};

    // etsi_qkd_004::URI source = "source";
    // etsi_qkd_004::URI destination = "destination";
    // etsi_qkd_004::QoS qos;
    // qos.key_chunk_size = 3;
    // qos.max_bps = 5;
    // qos.min_bps = 1;
    // qos.jitter = 4;
    // qos.priority = 5;
    // qos.timeout = 0;
    // qos.ttl = 10;
    // qos.metadata_mimetype = "metadata";
    // etsi_qkd_004::UUID key_stream_id = "key_stream_id";

    // json open_connectJson = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id);
    // std::string open_connectString = open_connectJson.dump();
    // std::cout << open_connectString << std::endl;
    
    LoadRequest LoadRequest_Tx{ {},{&open_connect} };
    
    
    // RX
    // DestinationTranslationTable dttRxTransmitter;
    // dttRxTransmitter.add("Msg_Tx", 0);
    // MessageHandler MessageHandlerRX{ {&open_connect_},{&open_connect},dttRxTransmitter,FUNCTIONING_AS_RX};

    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
    MessageHandler MessageHandlerTX{ {&open_connect},{&open_connect_},FUNCTIONING_AS_TX,ittTxTransmitter};

    

    // IPTunnel IPTunnel_Server_Rx{ {},  {&MessagesFromTx_} };
    // IPTunnel_Server_Rx.setLocalMachineIpAddress("127.0.0.1");
    // IPTunnel_Server_Rx.setRemoteMachineIpAddress("127.0.0.1");
    // IPTunnel_Server_Rx.setLocalMachinePort(54000);
    // IPTunnel_Server_Rx.setVerboseMode(false);

    // IPTunnel IPTunnel_Server_Rx{ {},  {&MessagesFromTx_} };
    // IPTunnel_Server_Rx.setLocalMachineIpAddress("127.0.0.1");
    // IPTunnel_Server_Rx.setRemoteMachineIpAddress("127.0.0.1");
    // IPTunnel_Server_Rx.setLocalMachinePort(54000);
    // IPTunnel_Server_Rx.setVerboseMode(false);


    // std::cout << &open_connect;
    // SaveAscii SaveAsciiTx{{&open_connect}, {} };
    // SaveAsciiTx.setAsciiFolderName("received");
    // SaveAsciiTx.setAsciiFileName("open_connect.txt");
    // SaveAsciiTx.setAsciiFileNameTailNumber("0");
    // SaveAsciiTx.setAsciiFileNameTailNumberModulos(0);

    System System_
            {
                {
                &LoadRequest_Tx,
                &MessageHandlerTX
                //&MessageHandlerRX
                // &SaveAsciiTx,
                // &APPReceiver_Tx_,
                // &APPTransmitter_Tx_,
                // &IPTunnel_Client_Tx,
                // &IPTunnel_Server_Tx
                }
            };

    //  Console Console_;
    // BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
    // Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
    // Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
    // Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
    // Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

    // BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
    // Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

    // BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
    // Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);

    System_.run();
    System_.terminate();

    return 0;
}