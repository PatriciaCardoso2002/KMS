#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
#include "ms_windows_console_output_common_20200819.h"


namespace tx {
    int main(int argc, charw *argv[])
    {
            // #####################################################################################################
            // ########################### 		 		TX						 ###################################
            // #####################################################################################################
        ConsoleOutputCommon::prepareWindowsConsole();
        ConsoleOutputCommon::clrScreen();
        Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
        Signal::t_header_type hType{ Signal::t_header_type::noHeader };
        
        Binary Raw_Tx{ "S0_Raw_Key_Tx.sgn", (t_unsigned_long) 512, hType, sWriteMode};
        Binary Key_Tx{ "S10_Raw_Key_Tx.sgn", (t_unsigned_long) 512};
        Message MessagesToRx_Tx{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesToRx_Tx_{ "S14_MessagesToRx.sgn", 10, hType, sWriteMode };
        Message MessagesFromRx_Tx{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesFromRx_Tx_{ "S15_Tx_MessagesFromRx.sgn", 10, hType, sWriteMode };

        LoadAscii LoadAscii_Tx{ {},{&Raw_Tx} };
        LoadAscii_Tx.setAsciiFileName("raw_keys/tx_raw");
        LoadAscii_Tx.setAsciiFileNameTailNumber("0");
        LoadAscii_Tx.setAsciiFileNameExtension(".dat");
        LoadAscii_Tx.setAsciiFileNameTailNumberModulos(0);


        SaveAscii SaveAscii_Tx{ {&Key_Tx}, {} };
        SaveAscii_Tx.setAsciiFolderName("generated_keys");
        SaveAscii_Tx.setAsciiFileName("tx_key");
        SaveAscii_Tx.setAsciiFileNameTailNumber("0");
        SaveAscii_Tx.setAsciiFileNameTailNumberModulos(0);

        // TX_RX
        DestinationTranslationTable dttTxReceiver;
        dttTxReceiver.add("Msg_Tx", 0);
        MessageHandler APPReceiver_Tx_{ {&MessagesFromRx_Tx_}, {&MessagesFromRx_Tx},dttTxReceiver,FUNCTIONING_AS_RX,};
        // TX_TX
        InputTranslationTable ittTxTransmitter;
        ittTxTransmitter.add(0, {"Msg_Rx", "Msg_Tx"});
        MessageHandler APPTransmitter_Tx_{ {&MessagesToRx_Tx},{&MessagesToRx_Tx_},FUNCTIONING_AS_TX,ittTxTransmitter};
            
        IPTunnel IPTunnel_Client_Tx{ {&MessagesToRx_Tx_}, {} };
        IPTunnel_Client_Tx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Tx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Tx.setRemoteMachinePort(54000);
        IPTunnel_Client_Tx.setVerboseMode(false);

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
                &LoadAscii_Tx,
                &SaveAscii_Tx,
                &APPReceiver_Tx_,
                &APPTransmitter_Tx_,
                &IPTunnel_Client_Tx,
                &IPTunnel_Server_Tx
                }
            };
        
        Console Console_;
        BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
        Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Tx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
        Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

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
    
}

namespace rx
{
    int main(int argc, char *argv[])
    {
            // #####################################################################################################
            // ########################### 		 		RX					     ###################################
            // #####################################################################################################
        ConsoleOutputCommon::prepareWindowsConsole();
        ConsoleOutputCommon::clrScreen();
        Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
        Signal::t_header_type hType{ Signal::t_header_type::noHeader };
        
        Binary Raw_Rx{ "S0_TX_Raw_Key_Tx.sgn", (t_unsigned_long) 512, hType, sWriteMode};
        Binary Key_Rx{ "S10_Raw_Key_Tx.sgn", (t_unsigned_long) 512};
        Message MessagesToTx_Rx{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesToTx_Rx_{ "S4_MessagesFromRx.sgn", 10, hType, sWriteMode };
        // ####################################################################
        Message MessagesFromTx{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };
        HandlerMessage MessagesFromTx_{ "S0_MessagesFromTx.sgn", 10, hType, sWriteMode };

        IPTunnel IPTunnel_Server_Rx{ {},  {&MessagesFromTx_} };
        IPTunnel_Server_Rx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Rx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Server_Rx.setLocalMachinePort(54001);
        IPTunnel_Server_Rx.setVerboseMode(false);

        LoadAscii LoadAscii_Rx{ {},{&Raw_Rx} };
        LoadAscii_Rx.setAsciiFileName("raw_keys/rx_raw");
        LoadAscii_Rx.setAsciiFileNameTailNumber("0");
        LoadAscii_Rx.setAsciiFileNameExtension(".dat");
        LoadAscii_Rx.setAsciiFileNameTailNumberModulos(0);

        // RX_RX
        DestinationTranslationTable dttRxReceiver;
        dttRxReceiver.add("Msg_Rx", 0);
        MessageHandler APPReceiver_Rx_{ {&MessagesFromTx_}, {&MessagesFromTx},dttRxReceiver,FUNCTIONING_AS_RX,};
        // RX_TX
        InputTranslationTable ittRxTransmitter;
        ittRxTransmitter.add(0, {"Msg_Tx", "Msg_Rx"});
        MessageHandler APPTransmitter_Rx_{ {&MessagesToTx_Rx},{&MessagesToTx_Rx_},FUNCTIONING_AS_TX,ittRxTransmitter};
            
        IPTunnel IPTunnel_Client_Rx{ {&MessagesToTx_Rx_}, {} };
        IPTunnel_Client_Rx.setLocalMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachineIpAddress("127.0.0.1");
        IPTunnel_Client_Rx.setRemoteMachinePort(54000);
        IPTunnel_Client_Rx.setVerboseMode(false);

        SaveAscii SaveAscii_Rx{ {&Key_Rx}, {} };
        SaveAscii_Rx.setAsciiFolderName("generated_keys");
        SaveAscii_Rx.setAsciiFileName("rx_key");
        SaveAscii_Rx.setAsciiFileNameTailNumber("0");
        SaveAscii_Rx.setAsciiFileNameTailNumberModulos(0);
    // #####################################################################################################
    // ########################### System Declaration and Inicialization ###################################
    // #####################################################################################################
        System System_
            {
                {
                &LoadAscii_Rx,
                &SaveAscii_Rx,
                &APPReceiver_Rx_,
                &APPTransmitter_Rx_,
                &IPTunnel_Client_Rx,
                &IPTunnel_Server_Rx
                }
            };
        
        Console Console_;
        BlockGetFunction<std::_Mem_fn<std::string (LoadAscii::*)() const>, LoadAscii, std::string> Load_File_Name_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getAsciiFileFullName) };
        Console_.addGetFunction("Load File Name", &Load_File_Name_, value_type::t_string);

        BlockGetFunction<std::_Mem_fn<unsigned long int (LoadAscii::*)() const>, LoadAscii, unsigned long int> Number_Of_Loaded_Values_{ &LoadAscii_Rx, std::mem_fn(&LoadAscii::getNumberOfLoadedValues) };
        Console_.addGetFunction("Number of Loaded Values", &Number_Of_Loaded_Values_, value_type::t_unsigned_long_int);

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
    
} // namespace rx

int main(int argc, char* argv[])
{
	//tx::main(argc, argv);
	rx::main(argc, argv);

	return 0;
}
