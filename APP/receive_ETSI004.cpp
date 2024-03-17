#include "receive_ETSI004.h"
#include "etsi_qkd_004.h"
#include "load_ascii_20200819.h"

void ReceiveETSI004::initialize(void){
    
    
}

bool ReceiveETSI004::runBlock(void){

    bool alive = true;

    if (getTerminated()) return false;

    if (ID == "Tx" && getFirstTime()){
        setFirstTime(false);

        etsi_qkd_004::URI source = "app_client";
        etsi_qkd_004::URI destination = "app_server";

        etsi_qkd_004::QoS qos;
        qos.key_chunk_size = 3;
        qos.max_bps = 5;
        qos.min_bps = 1;
        qos.jitter = 4;
        qos.priority = 5;
        qos.timeout = 0;
        qos.ttl = 10;
        qos.metadata_mimetype = "metadata";

        etsi_qkd_004::UUID key_stream_id = "1";

        //fazer pedido open_connect
        t_string msgDataSend = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id).dump();
        t_message msgSend;
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);

        //return alive;
    }

    ready = inputSignals[0]->ready();

    json msgCommand;
    json msgData;
    unsigned int index = 0;

    if(getVerboseMode()){
        std::cout << "ready: " << ready << std::endl;
    }
    
    for(auto k=0; k < ready; ++k){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        t_string msgString = msgReceived.getMessageData();
        storedMessages.push_back(msgReceived);

        json msgJson = json::parse(msgString);
        msgCommand = msgJson["command"];
        msgData = msgJson["data"];

        if(getVerboseMode()){
            // std::cout << "msgString: " << msgString << std::endl;
            // std::cout << "msgJSON: " << msgJson << std::endl;
            // std::cout << "msgCommand:  " << msgCommand << std::endl;
            // std::cout << "msgData: " << msgData << std::endl;
            i = 0;
            for(const auto& msg : storedMessages){
                std::cout << "MSG: " << i << " - " << msg << std::endl;
                i++;
            }
            
        }
        if (msgCommand == "OPEN_CONNECT_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "RECEIVED OPEN_CONNECT_RESPONSE" << std::endl;
            }

            etsi_qkd_004::UUID key_stream_id = "1";
        
            etsi_qkd_004::Metadata metadata = {32,"Metadata about key requested"};

            t_message msgSend;
            t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,index,metadata).dump();
            msgSend.setMessageData(msgDataSend);

            

            outputSignals[0]->bufferPut(msgSend);

            alive = true;

        } else if (msgCommand == "GET_KEY_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "RECEIVED GET_KEY_RESPONSE" << std::endl;
            }

            etsi_qkd_004::UUID key_stream_id = "1";
            t_string keyBuffer = msgData["key_buffer"].dump();
            t_integer space = outputSignals[1]->space();
            std::cout << "espaco: " << space << std::endl;

            // Assuming keyBuffer is already filled
            for (int i = 0; i < keyBuffer.length(); i++) {
                if (keyBuffer[i] == '0') {
                    outputSignals[1]->bufferPut(t_binary(0));
                    space--;
                }
                if (keyBuffer[i] == '1') {
                    outputSignals[1]->bufferPut(t_binary(1));
                    space--;
                } 
            }

            alive = true;

        }
        
    }
    

    if (msgCommand == "OPEN_CONNECT"){
        // código para verificar pedido e gerar resposta ao open_connect
        if(getVerboseMode()){
            std::cout << "RECEIVED OPEN_CONNECT" << std::endl;
        }
        etsi_qkd_004::QoS qos;
        qos.key_chunk_size = msgData["qos"]["key_chunk_size"];
        qos.max_bps = msgData["qos"]["max_bps"];
        qos.min_bps = msgData["qos"]["min_bps"];
        qos.jitter = msgData["qos"]["jitter"];
        qos.priority = msgData["qos"]["priority"];
        qos.timeout = msgData["qos"]["timeout"];
        qos.ttl = msgData["qos"]["ttl"];
        qos.metadata_mimetype = msgData["qos"]["metadata_mimetype"];

        etsi_qkd_004::UUID key_stream_id = msgData["key_stream_id"];

        t_message msgSend;
        etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
        t_string msgDataSend = etsi_qkd_004::handle_open_connect(key_stream_id,qos,status).dump();
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);
        
        alive = true;

    } else if (msgCommand == "GET_KEY"){
        // código para gerar resposta ao get_key
        if(getVerboseMode()){
            std::cout << "RECEIVED GET_KEY" << std::endl;
        }

        while(inputSignals[1]->ready()){
            t_binary key;
            etsi_qkd_004::KeyBuffer keyBuffer;

            for (int i = 0; i < 180; ++i) {
                inputSignals[1]->bufferGet(&key);
                keyBuffer.push_back(key);
            }

            //std::cout << "depois for" << std::endl;

            etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
            etsi_qkd_004::Metadata metadata = {32,"Metadata about key sent"};
            t_string msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,index++,metadata).dump();
            
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            //std::cout << "Message created" << std::endl;
            if(messagesToSend.space()){
                messagesToSend.bufferPut(msgSend);
            } else if (getVerboseMode()){
                std::cout << "NO SPACE IN INTERNAL BUFFER" << std::endl;
            }
        }

        alive = true;

    } else if (msgCommand == "CLOSE"){
        etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;

        t_message msgSend;
        t_string msgDataSend = etsi_qkd_004::handle_close(status).dump();
        msgSend.setMessageData(msgDataSend);

    } else if (ID == "Rx" && messagesToSend.ready() && !inputSignals[0]->ready()){
        if(outputSignals[0]->space()){
            t_message msg = messagesToSend.bufferGet();
            std::cout << "MESSAGE TO OUTPUT: " << msg << std::endl;
            outputSignals[0]->bufferPut(msg);
            std::cout << "MESSAGE OUTPUTTED" << std::endl;

        } else {
            if(getVerboseMode()){
                std::cout << "OUTPUT BUFFER IS FULL" << std::endl;
            }
        }
    } else {
        if(getVerboseMode()){
            std::cout << "DID NOTHING" << std::endl;
        }
    }

    //return alive;
    
}