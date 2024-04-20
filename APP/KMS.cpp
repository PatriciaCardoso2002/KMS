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
    
    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };
    Message request{"Client_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"Client_request_.sgn",10,hType,sWriteMode};
    Message response{"Client_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Client_response_.sgn",10,hType,sWriteMode};
    Binary key{"Client_key.sgn",1024,hType,sWriteMode};
    
    SaveAscii saveKeys{{&key},{}};
    saveKeys.setAsciiFolderName("KMS_saved_keys");
    saveKeys.setAsciiFileName("saved_keys");
    saveKeys.setAsciiFileNameTailNumber("0");
    saveKeys.setAsciiFileNameTailNumberModulos(0);

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Msg_Tx", 0);
    MessageHandler MessageHandlerClientRX{ {&response_},{&response},dttRxTransmitter,FUNCTIONING_AS_RX};

    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
    MessageHandler MessageHandlerClientTX{ {&request},{&request_},FUNCTIONING_AS_TX,ittTxTransmitter};
    
    IPTunnel IPTunnelClient_Client{{&request_},{}};
    IPTunnelClient_Client.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelClient_Client.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelClient_Client.setRemoteMachinePort(54000);
    IPTunnelClient_Client.setVerboseMode(true);
    IPTunnelClient_Client.setTimeIntervalSeconds(10);

    IPTunnel IPTunnelClient_Server{{},{&response_}};
    IPTunnelClient_Server.setLocalMachineIpAddress("127.0.0.1");
    IPTunnelClient_Server.setRemoteMachineIpAddress("127.0.0.1");
    IPTunnelClient_Server.setLocalMachinePort(54001);
    IPTunnelClient_Server.setVerboseMode(true);
    IPTunnelClient_Server.setTimeIntervalSeconds(10);

    ETSI004Block ETSI004_KMS{{&response}, {&request, &key}};
    ETSI004_KMS.setID("Tx");
    ETSI004_KMS.setSource("KMS");
    ETSI004_KMS.setDestination("Reconciliation");
    ETSI004_KMS.setQoS(0,50);
    ETSI004_KMS.setMode(ETSI004Block::PUSH);
    ETSI004_KMS.setVerboseMode(true);

    System System_
            {
                {
                &saveKeys,
                &ETSI004_KMS,
                &IPTunnelClient_Client,
                &IPTunnelClient_Server,
                &MessageHandlerClientTX,
                &MessageHandlerClientRX,
                }
            };
    
    System_.run();
    System_.terminate();

    return 0;
}