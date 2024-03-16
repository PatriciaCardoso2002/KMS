#include "receive_ETSI004.h"
#include "etsi_qkd_004.h"
#include "load_ascii_20200819.h"

void ReceiveETSI004::initialize(void){
    
    
}

bool ReceiveETSI004::runBlock(void){

    bool alive = true;

    if (getTerminated()) return false;

    if (getFirstTime() && getID() == "Tx"){
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
    }

    ready = inputSignals[0]->ready();
    json msgCommand;
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
        json msgData = msgJson["data"];

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
        alive = true;
        
    }
    

    if (msgCommand == "OPEN_CONNECT"){
        // código para verificar pedido e gerar resposta ao open_connect
        t_message msgSend;
        t_string msgDataSend = "{\"command\":\"OPEN_CONNECT_RESPONSE\",\"data\":{\"source\":\"source\",\"destination\":\"destination\",\"qos\":{\"key_chunk_size\":3,\"max_bps\":5,\"min_bps\":1,\"jitter\":4,\"priority\":5,\"timeout\":0,\"ttl\":10,\"metadata_mimetype\":\"metadata\"},\"1\":\"key_stream_id\"}}";
        msgSend.setMessageData(msgDataSend);
        
        

        if(getVerboseMode()){
            // std::cout << "Response data: " << msgDataSend << std::endl;
            // std::cout << "Response: " << msgSend << std::endl;
            std::cout << "RECEIVED OPEN_CONNECT" << std::endl;
        }

        outputSignals[0]->bufferPut(msgSend);
        
        alive = true;

    } else if (msgCommand == "OPEN_CONNECT_RESPONSE"){
        // código para gerar resposta ao open_connect_response
        // se estiver tudo bem é um GET_KEY

        etsi_qkd_004::UUID key_stream_id = "1";
        
        etsi_qkd_004::Metadata metadata = {32,"Metadata about key requested"};

        t_message msgSend;
        t_string msgDataSend = etsi_qkd_004::get_key(key_stream_id,index++,metadata).dump();
        msgSend.setMessageData(msgDataSend);

        if(getVerboseMode()){
            std::cout << "RECEIVED OPEN_CONNECT_RESPONSE" << std::endl;
        }

        outputSignals[0]->bufferPut(msgSend);

        alive = true;

    } else if (msgCommand == "GET_KEY"){
        // código para gerar resposta ao get_key
        if(getVerboseMode()){
            std::cout << "RECEIVED GET_KEY" << std::endl;
        }

        // while(inputSignals[1]->ready()){

        // }

        t_binary key;
        etsi_qkd_004::KeyBuffer keyBuffer;
        for (int i = 0; i < 1040; ++i) {
            inputSignals[1]->bufferGet(&key);
            keyBuffer.push_back(key);
        }
        
        etsi_qkd_004::Status status = etsi_qkd_004::SUCCESSFUL;
        etsi_qkd_004::Metadata metadata = {32,"Metadata about key sent"};
        t_string msgDataSend = etsi_qkd_004::handle_get_key(status,keyBuffer,index,metadata).dump();
        
        t_message msgSend;
        msgSend.setMessageData(msgDataSend);
        outputSignals[0]->bufferPut(msgSend);
        
        alive = false;

    } else if (msgCommand == "GET_KEY_RESPONSE"){
        // código para gerar resposta ao get_key


        if(getVerboseMode()){
            std::cout << "RECEIVED GET_KEY_RESPONSE" << std::endl;
        }

        alive = false;

    } else {
        if(getVerboseMode()){
            std::cout << "DID NOTHING" << std::endl;
        }
    }

	// auto space = outputSignals[0]->space();
	// auto process = std::min(ready, space);
    // alive = process ? process : alive;
	// for (; process--;)
	// 	outputSignals[0]->bufferPut(BasesFrom.bufferGet());

    return alive;
    
    // if(getFirstTime()){
    //     setFirstTime(false);
    //     ready = requestSize;
    // }
    // if (ready == 0){
    //     return false;
    // }
    

    // for (t_char c : request){
    //     ready--;
    //     outputSignals[0]->bufferPut(c);
    // }
    
}