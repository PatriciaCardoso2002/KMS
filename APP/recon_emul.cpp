#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ETSI004_block.h"
#include "etsi_qkd_004.h"

int main(){
    
    // sinal do tipo HandlerMessage entra no Message Handler TX da app_client como input signal,
    // este sinal é uma conversão do json relativo a cada request ETSI 004
    // o output signal do message handler será o mesmo (?)
    // O sinal é enviado pelo IP Tunnel até ao Message Handler RX da app_server (input signal)
    // que o coloca como output signal para a app processar a mensagem (converte para json e dá handle ao pedido)
    // a resposta a este pedido é gerada, convertida para sinal do tipo HandlerMessage e colocado como input Signal
    // do MessageHandler TX da app_server. Será o mesmo output sinal e daí é enviado pelo IP Tunnel para o Message Handler RX da app_client
    // passa para output sinal e esse output sinal será processado como anteriormente.

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Message request{"Server_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"Server_request_.sgn",10,hType,sWriteMode};
    Message response{"Server_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Server_response_.sgn",10,hType,sWriteMode};
    Binary key{"Server_key.sgn",1024,hType,sWriteMode};

    LoadAscii readKeys({},{&key});
    readKeys.setAsciiFileName("recon_keys/rx_raw");
    readKeys.setAsciiFileNameTailNumber("0");
    //readKeys.setAsciiFileNameTailNumberModulos(5);


    IPTunnel IPTunnelServer_Server{{},{&request_}};
    IPTunnelServer_Server.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelServer_Server.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelServer_Server.setLocalMachinePort(54000);
    IPTunnelServer_Server.setVerboseMode(false);
    IPTunnelServer_Server.setTimeIntervalSeconds(10);

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Msg_Rx", 0);
    MessageHandler MessageHandlerServerRX{ {&request_},{&request},dttRxTransmitter,FUNCTIONING_AS_RX};
    
    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
    MessageHandler MessageHandlerServerTX{ {&response},{&response_},FUNCTIONING_AS_TX,ittTxTransmitter};

    IPTunnel IPTunnelServer_Client{{&response_},{}};
    IPTunnelServer_Client.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelServer_Client.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelServer_Client.setRemoteMachinePort(54001);
    IPTunnelServer_Client.setVerboseMode(false);
    IPTunnelServer_Client.setTimeIntervalSeconds(10);
    

    ETSI004Block ETSI004_RECON{{&request, &key}, {&response}};
    ETSI004_RECON.setID("Rx");
    ETSI004_RECON.setMode(ETSI004Block::PUSH);
    ETSI004_RECON.setVerboseMode(true);

    System System_
            {
                {
                &readKeys,
                &IPTunnelServer_Server,
                &ETSI004_RECON,
                &IPTunnelServer_Client,
                &MessageHandlerServerTX,
                &MessageHandlerServerRX,
                }
            };

    System_.run();
    System_.terminate();

    return 0;
}