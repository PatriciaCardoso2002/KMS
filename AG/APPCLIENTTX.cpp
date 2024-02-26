#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ms_windows_console_output_common_20200819.h"

int main(int argc, char* argv[]) {
        // #####################################################################################################
        // ########################### 		 		TX						 ###################################
        // #####################################################################################################
        ConsoleOutputCommon::prepareWindowsConsole();
        ConsoleOutputCommon::clrScreen();
        Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
        Signal::t_header_type hType{ Signal::t_header_type::noHeader };

        Message MessagesToRx_Tx{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesToRx_Tx_{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };
        Message MessagesFromRx_Tx{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesFromRx_Tx_{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };


        // nao sei se é necessario utilizar o LoadAscii e o SaveAscii

        // enviar
        // TX->RX  
        InputTranslationTable ittTxTransmitter;
        ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
        MessageHandler APPTransmitter_Tx_{ {&MessagesToRx_Tx},{&MessagesToRx_Tx_},FUNCTIONING_AS_TX,ittTxTransmitter};


        // recetor
        // RX->TX
        DestinationTranslationTable dttTxReceiver;
        dttTxReceiver.add("Msg_Tx", 0);
        MessageHandler APPReceiver_Tx_{ {&MessagesFromRx_Tx_}, {&MessagesFromRx_Tx},dttTxReceiver,FUNCTIONING_AS_RX,};


        // ip tunnel_client_Tx -> ip tunnel_server_Rx
        IPTunnel IPTunnel_Client_Tx{ {&MessagesToRx_Tx_}, {} };
        IPTunnel_Client_Tx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Tx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Tx.setRemoteMachinePort(54000);
        IPTunnel_Client_Tx.setVerboseMode(false);

        // ip tunnel_server_Rx -> ip tunnel_client_Tx
        IPTunnel IPTunnel_Server_Tx{ {}, {&MessagesFromRx_Tx_} };
        IPTunnel_Server_Tx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Tx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Tx.setLocalMachinePort(54001);
        IPTunnel_Server_Tx.setVerboseMode(false);

        // #####################################################################################################
    // ########################### System Declaration and Inicialization ###################################
    // #####################################################################################################
        System System_
            {
                {
                &APPReceiver_Tx_,
                &APPTransmitter_Tx_,
                &IPTunnel_Client_Tx,
                &IPTunnel_Server_Tx
                }
            };
        
        Console Console_;

        BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
        Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Tx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
        Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

        BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
        Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Tx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
        Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);
        

        System_.setConsole(&Console_);
            System_.setShowConsole(true);
            System_.setMsgOpenConsole("Tx System Console");
            System_.setShowConsolePeriodicity(100);

        
        System_.setLogValue(true);
        System_.setVerboseMode(false);
        System_.run();
        System_.terminate();
    return 0;
    }