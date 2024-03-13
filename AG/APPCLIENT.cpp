#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ms_windows_console_output_common_20200819.h"
#include "load_request.h"
#include "receive_request.h"
//outputAPP1 tx tx -> iptunnel/ rx tx / rx rx / tx rx / tx tx -> ip tunnel
//message_handler() do get key; RX (input etsi(open_connect,get_key,close,status), output parametros)


    int main(int argc, char* argv[]) {
        // #####################################################################################################
        // ########################### 		 		RX						 ###################################
        // #####################################################################################################
        ConsoleOutputCommon::prepareWindowsConsole();
        ConsoleOutputCommon::clrScreen();
        Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
        Signal::t_header_type hType{ Signal::t_header_type::noHeader };
        
        Message Key_Rx{ "S10_Raw_Key_Tx.sgn", (t_unsigned_long) 512};
        Message Raw_Rx{ "aaaaaa.sgn", (t_unsigned_long) 512, hType, sWriteMode};
        Message MessagesToTx_Rx{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesToTx_Rx_{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };
        // ####################################################################
        Message MessagesFromTx{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesFromTx_{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };

        // ip tunnel_server_tx -> ip tunnel_client_rx
        IPTunnel IPTunnel_Client_Rx{ {&MessagesToTx_Rx_}, {} };
        IPTunnel_Client_Rx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachinePort(54001);
        IPTunnel_Client_Rx.setVerboseMode(false);

        // armazena o raw_material
        /* SaveAscii SaveAscii_Rx{ {&Key_Rx}, {} };
        SaveAscii_Rx.setAsciiFolderName("generated_keys");
        SaveAscii_Rx.setAsciiFileName("rx_key");
        SaveAscii_Rx.setAsciiFileNameTailNumber("0");
        SaveAscii_Rx.setAsciiFileNameTailNumberModulos(0); */

        /* LoadRequest LoadAscii_Rx{ {},{&Raw_Rx} };
        LoadAscii_Rx.setRequest("MMMMMMMMMMMMMMMMMMMMMMMM"); */



        // nao sei se é necessario utilizar o LoadAscii
        
        //envia
        // RX->TX
        InputTranslationTable ittRxTransmitter;
        ittRxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
        MessageHandler APPTransmitter_Rx_{ {&Raw_Rx},{&MessagesToTx_Rx_},FUNCTIONING_AS_TX,ittRxTransmitter};
        // mensage handler 

        //recebe
        // TX->RX
        DestinationTranslationTable dttRxReceiver;
        dttRxReceiver.add("Msg_Rx", 0);
        MessageHandler APPReceiver_Rx_{ {&MessagesFromTx_}, {&Key_Rx},dttRxReceiver,FUNCTIONING_AS_RX,};
        
        // ip tunnel_client_tx -> ip tunnel_server_rx
        IPTunnel IPTunnel_Server_Rx{ {},  {&MessagesFromTx_} };
        IPTunnel_Server_Rx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Rx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Rx.setLocalMachinePort(54000);
        IPTunnel_Server_Rx.setVerboseMode(false);

        // #####################################################################################################
        // ########################### System Declaration and Inicialization ###################################
        // #####################################################################################################
        System System_
            {
                {
                &LoadAscii_Rx,
                //&SaveAscii_Rx,
                &APPReceiver_Rx_,
                &APPTransmitter_Rx_,
                &IPTunnel_Client_Rx,
                &IPTunnel_Server_Rx
                }
            };
        
        Console Console_;
        /* BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
        Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
        Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int); */

       /*  BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Rx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
        Console_.addGetFunction("Local Machine Address", &Local_Machine_Address_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Local_Machine_Port_{ &IPTunnel_Server_Rx, std::mem_fn(&IPTunnel::getLocalMachinePort) };
        Console_.addGetFunction("Local Machine Receiving Port", &Local_Machine_Port_, value_type::t_int);

        BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Remote_Machine_Address_{ &IPTunnel_Client_Rx, std::mem_fn(&IPTunnel::getRemoteMachineIpAddress) };
        Console_.addGetFunction("Remote Machine Address", &Remote_Machine_Address_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<int (IPTunnel::*)() const>, IPTunnel, int> Remote_Machine_Port_{ &IPTunnel_Client_Rx, std::mem_fn(&IPTunnel::getRemoteMachinePort) };
        Console_.addGetFunction("Remote Machine Receiving Port", &Remote_Machine_Port_, value_type::t_int);
        

        System_.setConsole(&Console_);
            System_.setShowConsole(true);
            System_.setMsgOpenConsole("Rx System Console");
            System_.setShowConsolePeriodicity(100);

        
        System_.setLogValue(true);
        System_.setVerboseMode(false); */
        System_.run();
        System_.terminate();
        return 0;
}

    







        

    


