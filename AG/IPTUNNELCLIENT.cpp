#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"    
    int main(int argc, char* argv[]) {
        // #####################################################################################################
        // ########################### 		 		RX						 ###################################
        // #####################################################################################################
        ConsoleOutputCommon::prepareWindowsConsole();
        ConsoleOutputCommon::clrScreen();
        Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
        Signal::t_header_type hType{ Signal::t_header_type::noHeader };
        
        Binary Key_Rx{ "S10_Raw_Key_Tx.sgn", (t_unsigned_long) 512};
        

        // ip tunnel_server_tx -> ip tunnel_client_rx
        IPTunnel IPTunnel_Client_Rx{ {&Key_Rx}, {} };
        IPTunnel_Client_Rx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachinePort(54001);
        IPTunnel_Client_Rx.setVerboseMode(false);

        // armazena o raw_material
        SaveAscii SaveAscii_Rx{ {&Key_Rx}, {} };
        SaveAscii_Rx.setAsciiFolderName("generated_keys");
        SaveAscii_Rx.setAsciiFileName("rx_key");
        SaveAscii_Rx.setAsciiFileNameTailNumber("0");
        SaveAscii_Rx.setAsciiFileNameTailNumberModulos(0);

       
        
        // ip tunnel_client_tx -> ip tunnel_server_rx
        IPTunnel IPTunnel_Server_Rx{ {},  {&Key_Rx} };
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
                &SaveAscii_Rx,
                &IPTunnel_Client_Rx,
                &IPTunnel_Server_Rx
                }
            };
        
        Console Console_;
        /* BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
        Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
        Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int); */

        BlockGetFunction<std::_Mem_fn<std::string(IPTunnel::*)() const>, IPTunnel, std::string> Local_Machine_Address_{ &IPTunnel_Server_Rx, std::mem_fn(&IPTunnel::getLocalMachineIpAddress) };
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
        System_.setVerboseMode(false);
        System_.run();
        System_.terminate();
        return 0;
    }