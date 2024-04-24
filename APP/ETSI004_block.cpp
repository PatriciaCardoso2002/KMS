#include "ETSI004_block.h"
#include "etsi_qkd_004.h"

// pull mode não está a ter em conta se recebe verdadeiramente um novo get_key


void ETSI004Block::initialize(void){
      
}

bool ETSI004Block::runBlock(void){

    bool alive = true;
    if (getTerminated()) return false;

    if (getFirstTime()){
        // get_keyResID = 0; // need to be reseted if multiple KSID
        // get_keyID = 0;   // need to be reseted if multiple KSID
        
        if (ID == "Tx"){
            //fazer pedido open_connect
            t_string msgDataSend = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id).dump();
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
        }
        setFirstTime(false);
    }

    if (getVerboseMode()){
        std::cout << "Previous Command: " << msgCommand << std::endl;
    }

    while(inputSignals[0]->ready() || msgCommand == "GET_KEY"){
    
        if (inputSignals[0]->ready()){
            t_message msgReceived;
            inputSignals[0]->bufferGet(&msgReceived);
            receivedMessages.push_back(msgReceived);
            msgJson = json::parse(msgReceived.getMessageData());
            msgCommand = msgJson["command"];
            msgData = msgJson["data"];
        } else if (mode == PULL){
            msgCommand = NULL;
        }

        // verificar comando e enviar resposta

        if (msgCommand == "OPEN_CONNECT"){
            if(getVerboseMode()){
                std::cout << "RECEIVED OPEN_CONNECT" << std::endl;
            }

            unsigned int key_type = msgData["qos"]["key_type"];
            unsigned int key_chunk_size = msgData["qos"]["key_chunk_size"];
            unsigned int max_bps = msgData["qos"]["max_bps"];
            unsigned int min_bps = msgData["qos"]["min_bps"];
            unsigned int jitter = msgData["qos"]["jitter"];
            unsigned int priority = msgData["qos"]["priority"];
            unsigned int timeout = msgData["qos"]["timeout"];
            unsigned int ttl = msgData["qos"]["ttl"];
            std::string metadata_mimetype = msgData["qos"]["metadata_mimetype"];
            setQoS(key_type, key_chunk_size, max_bps, min_bps, jitter, priority, timeout, ttl, metadata_mimetype);

            t_message keyType;
            keyType.setMessageData(std::to_string(key_type));
            std::cout << keyType << std::endl;
            outputSignals[1]->bufferPut(keyType);
            
            t_message msgSend;
            etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
            t_string msgDataSend = etsi_qkd_004::handle_open_connect(key_stream_id,getQoS(),status).dump();
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
            
            alive = true;

        } else if (msgCommand == "OPEN_CONNECT_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "RECEIVED OPEN_CONNECT_RESPONSE" << std::endl;
            }

            get_keyID = 0;
            t_message msgSend;
            t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,get_keyID++,metadata_client).dump();
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);

            alive = true;

        } else if (msgCommand == "GET_KEY"){
            if (mode == PULL){

                if(getVerboseMode()){
                    std::cout << "RECEIVED GET_KEY PULL MODE" << std::endl;
                }
                etsi_qkd_004::KeyBuffer keyBuffer;

                if(inputSignals[1]->ready()){
                    for(auto k = 0; k < getQoS().key_chunk_size ; k++){
                        t_binary kval{0};
                        inputSignals[1]->bufferGet(&kval);
                        keyBuffer.push_back(kval);
                    }
                }
                if (getVerboseMode()){
                    for (unsigned char c : keyBuffer) {
                    std::cout << static_cast<int>(c);
                    }
                    std::cout << std::endl;
                    std::cout << "INPUT SIGNAL 1: " << inputSignals[1]->ready() << std::endl;
                }

                etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
                t_string msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,get_keyResID++,metadata_server).dump();

                t_message msgSend;
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);

                alive = true;
            } else if (mode == PUSH){

                if(getVerboseMode()){
                std::cout << "RECEIVED GET_KEY PUSH MODE" << std::endl;
                }
                
                etsi_qkd_004::KeyBuffer keyBuffer;

                if (inputSignals[1]->ready() && outputSignals[0]->space() && get_keyResID < num_keys){
                    std::cout << "INPUT SIGNAL 1: " << inputSignals[1]->ready() << std::endl;

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
                    t_message msgSend;
                    msgSend.setMessageData(msgDataSend);
                    outputSignals[0]->bufferPut(msgSend);
                    std::cout << "RESPOSTA COLOCADA NO BUFFER" << std::endl;
                    alive = true;
                    break;
                } else {alive = true; break;}
            }  
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

            if (mode == PULL && get_keyID < num_keys){
                t_message msgSend;
                t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,get_keyID++,metadata_client).dump();
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);

            } else if ( get_key_index == num_keys-1){
                t_message msgSend;
                t_string msgDataSend = etsi_qkd_004::close(key_stream_id).dump();
                msgSend.setMessageData(msgDataSend);
                outputSignals[0]->bufferPut(msgSend);
                //setTerminated(true);
            }
            alive = true;
        } else if (msgCommand == "CLOSE"){
            etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
            t_message msgSend;
            t_string msgDataSend = etsi_qkd_004::handle_close(status).dump();
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);

            if(getVerboseMode()){
                std::cout << "msgJSON: " << msgJson << std::endl;
                std::cout << "msgCommand:  " << msgCommand << std::endl;
                std::cout << "msgData: " << msgData << std::endl;
                for(const auto& msg : receivedMessages){
                    std::cout << "MSG: " << i << " - " << msg << std::endl;
                    i++;
                } 
            }
            //setFirstTime(true);

            setTerminated(true);
            //alive = false;
        } else if (msgCommand == "CLOSE_RESPONSE"){
            if(getVerboseMode()){
                std::cout << "msgJSON: " << msgJson << std::endl;
                std::cout << "msgCommand:  " << msgCommand << std::endl;
                std::cout << "msgData: " << msgData << std::endl;
                for(const auto& msg : receivedMessages){
                    std::cout << "MSG: " << i << " - " << msg << std::endl;
                    i++;
                }
            }
            // aumentar KSID quando chega ao nr de chaves pedidas do anterior (testar)
            // int KSID = std::stoi(key_stream_id);
            // KSID++;
            // std::ostringstream aux;
            // aux << KSID;
            // key_stream_id = aux.str();
            // setFirstTime(true);

            setTerminated(true);
            // alive = false;
        } else {
            std::cout << "UNKNOWN COMMAND" << std::endl;
        }
    }
    
    return alive;
}