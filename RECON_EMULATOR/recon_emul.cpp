#include "save_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "ms_windows_console_output_common_20200819.h"
#include "key_generator.h"
#include "cv_qokd_ldpc_multi_machine_sdf.h"


int main(){
    

    DvQkdLdpcInputParameters param = DvQkdLdpcInputParameters();
    param.setInputParametersFileName("input_parameters_RECON_EMULATOR.txt");
    param.readSystemInputParameters();

    Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

    
    Binary key_generated_RX{"Rx_key.sgn",32,hType,sWriteMode};
    Binary key_generated_TX{"Tx_key.sgn",32,hType,sWriteMode};

    KeyGenerator KEY_GENERATOR{{},{&key_generated_RX,&key_generated_TX}};
    KEY_GENERATOR.setKey_type(param.keyType);
    printf("Key type: %d\n", KEY_GENERATOR.getKey_type());
    KEY_GENERATOR.setseed(param.seed);
    printf("Seed: %d\n", KEY_GENERATOR.getseed());
    KEY_GENERATOR.initialize();

    SaveAscii saveRX{{&key_generated_RX},{}};
    saveRX.setFile_type(param.fileType);
    saveRX.setAsciiFolderName("../generated_keys_RX");
    saveRX.setAsciiFileNameTailNumber("0");
    saveRX.setAsciiFileNameTailNumberModulos(param.asciiFileNameTailNumberModulos);

    if(saveRX.getFile_type() == 1 && KEY_GENERATOR.getKey_type() == 0){
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("b64");
    }
    else if(saveRX.getFile_type()== 1 && KEY_GENERATOR.getKey_type() == 1 )
    {
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("b64");
    }
    else if(saveRX.getFile_type()== 0 && KEY_GENERATOR.getKey_type() == 0)
    {
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("dat");
    }
    else if(saveRX.getFile_type()== 0 && KEY_GENERATOR.getKey_type() == 1)
    {
        saveRX.setAsciiFileName(param.rxFileName);
        saveRX.setAsciiFileNameExtension("dat");
    }
    
    SaveAscii saveTX{{&key_generated_TX},{}};
    saveTX.setAsciiFolderName("../generated_keys_TX");
    saveTX.setFile_type(param.fileType);
    saveTX.setAsciiFileNameTailNumber("0");
    saveTX.setAsciiFileNameTailNumberModulos(param.asciiFileNameTailNumberModulos);

    if (saveTX.getFile_type() == 1 && KEY_GENERATOR.getKey_type() == 0) {
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("b64");
    }
    else if(saveTX.getFile_type()== 1 && KEY_GENERATOR.getKey_type() == 1){
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("b64");
    }
    else if(saveTX.getFile_type()== 0 && KEY_GENERATOR.getKey_type() == 0){
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("dat");
    }
    else if(saveTX.getFile_type()== 0 && KEY_GENERATOR.getKey_type() == 1){
        saveTX.setAsciiFileName(param.txFileName);
        saveTX.setAsciiFileNameExtension("dat");
    }
    
    System System_
            {
                {
                &KEY_GENERATOR,
                &saveRX,
                &saveTX,
                }
            };

    System_.run();
    System_.terminate();

    return 0;
}
