#include "load_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_2024.h"
#include "ms_windows_console_output_common_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ETSI004_block.h"
#include "etsi_qkd_004.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"

int main(int argc, char *argv[]){
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " a/b\n";
        return 1;
    }

    std::string role = argv[1];

    DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
    if(role=="a"){
        param.setInputParametersFileName("input_keyProviderA.txt");
        param.readSystemInputParameters();
    } else if (role=="b"){
        param.setInputParametersFileName("input_keyProviderB.txt");
        param.readSystemInputParameters();
    }

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    Message request{"Server_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"Server_request_.sgn",10,hType,sWriteMode};
    Message response{"Server_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"Server_response_.sgn",10,hType,sWriteMode};
    Binary key{"Server_key.sgn",1024,hType,sWriteMode};
    Binary key_type{"key_type.sgn",10,hType,sWriteMode};


    LoadAscii readKeys({&key_type},{&key});
    readKeys.setProvider(param.provider);
    readKeys.setAsciiFileNameTailNumber(param.cntFirstVal);

    if(param.fileType){
        readKeys.setAsciiFileNameExtension(".b64");
    }

    IPTunnel IPTunnelServer_Server{{},{&request_}};
    IPTunnelServer_Server.setLocalMachineIpAddress(param.rxIpAddress);
    IPTunnelServer_Server.setRemoteMachineIpAddress(param.txIpAddress);
    IPTunnelServer_Server.setLocalMachinePort(param.rxReceivingPort);
    IPTunnelServer_Server.setVerboseMode(param.verboseMode);
    //IPTunnelServer_Server.setTimeIntervalSeconds(10);

    // RX
    DestinationTranslationTable dttRxTransmitter;
    dttRxTransmitter.add("Key_Provider", 0);
    MessageHandler MessageHandlerServerRX{ {&request_},{&request},dttRxTransmitter,FUNCTIONING_AS_RX};
    
    // TX
    InputTranslationTable ittTxTransmitter;
    ittTxTransmitter.add(0, {"KMS_South", "Key_Provider"});
    MessageHandler MessageHandlerServerTX{ {&response},{&response_},FUNCTIONING_AS_TX,ittTxTransmitter};

    IPTunnel IPTunnelServer_Client{{&response_},{}};
    IPTunnelServer_Client.setLocalMachineIpAddress(param.rxIpAddress);
    IPTunnelServer_Client.setRemoteMachineIpAddress(param.txIpAddress);
    IPTunnelServer_Client.setRemoteMachinePort(param.txReceivingPort);
    IPTunnelServer_Client.setVerboseMode(param.verboseMode);
    //IPTunnelServer_Client.setTimeIntervalSeconds(10);
    

    ETSI004Block ETSI004_RECON{{&request, &key}, {&response, &key_type}};
    ETSI004_RECON.setID("Rx");
    ETSI004_RECON.setMode(ETSI004Block::PUSH);
    ETSI004_RECON.setNumKeys((unsigned int) param.numKeys);
    ETSI004_RECON.setVerboseMode(param.verboseMode);

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