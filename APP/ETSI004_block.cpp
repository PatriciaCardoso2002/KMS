#include "ETSI004_block.h"
#include "etsi_qkd_004.h"
#include "load_ascii_20200819.h"

void ETSI004Block::initialize(void){
      
}

bool ETSI004Block::runBlock(void){

    bool alive = true;

    if (getTerminated()) return false;

    ready = inputSignals[0]->ready();
    json lastrcv_msgCommand;
    json lastrcv_msg;
    json msgJson;
    json msgCommand;
    json msgData;

    if (getFirstTime()){
        if (ID == "Tx"){
            //fazer pedido open_connect
            t_string msgDataSend = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id).dump();
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
        }
        setFirstTime(false);
    }

    if(getVerboseMode()){
        std::cout << "Messages Arrived: " << ready << std::endl;
    }
    
    for(auto k=0; k < ready; ++k){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        t_string msgString = msgReceived.getMessageData();
        receivedMessages.push_back(msgReceived);

        msgJson = json::parse(msgString);
        msgCommand = msgJson["command"];
        msgData = msgJson["data"];

        if(getVerboseMode()){
            // std::cout << "msgString: " << msgString << std::endl;
            // std::cout << "msgJSON: " << msgJson << std::endl;
            // std::cout << "msgCommand:  " << msgCommand << std::endl;
            // std::cout << "msgData: " << msgData << std::endl;
            // i = 0;
            // for(const auto& msg : receivedMessages){
            //     std::cout << "MSG: " << i << " - " << msg << std::endl;
            //     i++;
            // } 
        }
        
        if (msgCommand == "OPEN_CONNECT_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "RECEIVED OPEN_CONNECT_RESPONSE" << std::endl;
            }
            get_keyID = 0;

            t_message msgSend;
            t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,get_keyID++,metadata_client).dump();
            msgSend.setMessageData(msgDataSend);

            outputSignals[0]->bufferPut(msgSend);

            alive = true;

        } else if (msgCommand == "GET_KEY_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "RECEIVED GET_KEY_RESPONSE" << std::endl;
            }

            t_string keyBuffer = msgData["key_buffer"].dump();
            unsigned int get_key_index = msgData["index"].get<unsigned int>();
            metadata_server.size = msgData["metadata"]["size"];
            metadata_server.buffer = msgData["metadata"]["buffer"];


            // Assuming keyBuffer is already filled
            for (int i = 0; i < keyBuffer.length(); i++) {
                if (keyBuffer[i] == '0') {
                    outputSignals[1]->bufferPut((t_binary)0);
                }
                if (keyBuffer[i] == '1') {
                    outputSignals[1]->bufferPut((t_binary)1);
                } 
            }
            // for (int i = 0; i < metadata_server.size; i++) {
            //     outputSignals[1]->bufferPut(metadata_server.buffer[i]);
            // }

            if (mode == PULL && get_keyID < qos.key_nr){
                t_message msgSend;
                t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,get_keyID++,metadata_client).dump();
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);

            } else if ( get_key_index == qos.key_nr-1){
                t_message msgSend;
                t_string msgDataSend = etsi_qkd_004::close(key_stream_id).dump();
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);
                //setTerminated(true);
            }
            alive = true;
        }

        if (mode == PULL && msgCommand == "GET_KEY"){
            if(getVerboseMode()){
            std::cout << "RECEIVED GET_KEY" << std::endl;
            }
            etsi_qkd_004::KeyBuffer keyBuffer;

            while(inputSignals[1]->ready()){
                for(auto k = 0; k < getQoS().key_chunk_size ; k++){
                    t_binary kval{0};
                    inputSignals[1]->bufferGet(&kval);
                    keyBuffer.push_back(kval);
                }

                if (getVerboseMode()){
                    for (unsigned char c : keyBuffer) {
                    std::cout << static_cast<int>(c);
                    }
                    std::cout << std::endl;
                }

                etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
                t_string msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,get_keyResID++,metadata_server).dump();
                keyBuffer.clear();

                t_message msgSend;
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);

                alive = true;
                return alive;
            }
        }
        
    }

    if (!receivedMessages.empty()){
        lastrcv_msg = json::parse(receivedMessages.back().getMessageData());
        lastrcv_msgCommand = lastrcv_msg["command"];
    }
    
    if (msgCommand == "OPEN_CONNECT"){
        // código para verificar pedido e gerar resposta ao open_connect
        if(getVerboseMode()){
            std::cout << "RECEIVED OPEN_CONNECT" << std::endl;
        }
        
        unsigned int key_chunk_size = msgData["qos"]["key_chunk_size"];
        unsigned int key_nr = msgData["qos"]["key_nr"];
        unsigned int max_bps = msgData["qos"]["max_bps"];
        unsigned int min_bps = msgData["qos"]["min_bps"];
        unsigned int jitter = msgData["qos"]["jitter"];
        unsigned int priority = msgData["qos"]["priority"];
        unsigned int timeout = msgData["qos"]["timeout"];
        unsigned int ttl = msgData["qos"]["ttl"];
        std::string metadata_mimetype = msgData["qos"]["metadata_mimetype"];
        setQoS(key_chunk_size, key_nr, max_bps, min_bps, jitter, priority, timeout, ttl, metadata_mimetype);

        t_message msgSend;
        etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
        t_string msgDataSend = etsi_qkd_004::handle_open_connect(key_stream_id,getQoS(),status).dump();
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);
        
        alive = true;

    } else if (msgCommand == "CLOSE"){
        etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
        t_message msgSend;
        t_string msgDataSend = etsi_qkd_004::handle_close(status).dump();
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);
        setTerminated(true);
    }

    if (mode == PUSH){

        if (msgCommand == "GET_KEY" || lastrcv_msgCommand == "GET_KEY"){
            // código para gerar resposta ao get_key
            if(getVerboseMode()){
                std::cout << "RECEIVED GET_KEY" << std::endl;
            }
            
            etsi_qkd_004::KeyBuffer keyBuffer;

            while(inputSignals[1]->ready() && messagesToSend.space() && get_keyResID < getQoS().key_nr){

                for(auto k = 0; k < getQoS().key_chunk_size ; k++){
                    t_binary kval{0};
                    inputSignals[1]->bufferGet(&kval);
                    keyBuffer.push_back(kval);
                }

                if (getVerboseMode()){
                    for (unsigned char c : keyBuffer) {
                    std::cout << static_cast<int>(c);
                    }
                    std::cout << std::endl;
                }
                
                etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
                t_string msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,get_keyResID++,metadata_server).dump();
                keyBuffer.clear();

                t_message msgSend;
                msgSend.setMessageData(msgDataSend);

                if(messagesToSend.space()){
                    messagesToSend.bufferPut(msgSend);
                } else if (getVerboseMode()){
                    std::cout << "NO SPACE IN INTERNAL BUFFER" << std::endl;
                }
            }

            alive = true;

        } 

        if (ID == "Rx" && messagesToSend.ready() && !inputSignals[0]->ready()){
            if(outputSignals[0]->space()){
                t_message msgSend = messagesToSend.bufferGet();
                outputSignals[0]->bufferPut(msgSend);
                if(getVerboseMode()){
                    std::cout << "MESSAGE OUTPUTTED" << std::endl;
                }
            } else {
                if(getVerboseMode()){
                    std::cout << "OUTPUT BUFFER IS FULL" << std::endl;
                }
            }
        }

    } else {
        if(getVerboseMode()){
            std::cout << "DID NOTHING" << std::endl;
        }
    }

    return alive;
}