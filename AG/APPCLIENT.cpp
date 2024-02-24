#include "load_ascii_20200819.h"
#include "netxpto_20200819.h"
#include <iostream>
#include <memory>
#include "message_handler.h"
#include "save_ascii_20200819.h"
#include "ip_tunnel_ms_windows_20200819.h"
//outputAPP1 tx tx -> iptunnel/ rx tx / rx rx / tx rx / tx tx -> ip tunnel
//message_handler() do get key; RX (input etsi(open_connect,get_key,close,status), output parametros)

int main(){
    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii };
    Signal::t_header_type hType{ Signal::t_header_type::noHeader };
    
    Binary Key_Tx{ "S0_Key_Tx.sgn", (t_unsigned_long)512,sWriteMode,hType};
    
    SaveAscii saveAscii({}, {});
    saveAscii.setAsciiFileName("rx_key");
    saveAscii.setAsciiFileNameExtension("dat");
    saveAscii.setAsciiFileNameTailNumber("0");
    saveAscii.setAsciiFolderName("key_files");
    saveAscii.setAsciiFileNameTailNumberModulos(0);
    saveAscii.setDataType(signal_value_type::t_binary);
    saveAscii.setDelimiterType(SaveAscii::delimiter_type::ConcatenatedValues);
    saveAscii.initialize();
    
}