#include "receive_request.h"
#include "etsi_qkd_004.h"

void ReceiveRequest::initialize(void){
    
}

bool ReceiveRequest::runBlock(void){

    bool alive = false;

    if (getTerminated()) return false;

    ready = inputSignals[0]->ready();

    if(getVerboseMode()){
        std::cout << "ready: " << ready << std::endl;
    }
    
    for(auto k=0; k < ready; ++k){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        t_string msgString = msgReceived.getMessageData();

        json msgJson = nlohmann::json::parse(msgString);
        json msgCommand = msgJson["command"];
        json msgData = msgJson["data"];

        if(getVerboseMode()){
            std::cout << "msgString: " << msgString << std::endl;
            std::cout << "msgJSON: " << msgJson << std::endl;
            std::cout << "msgCommand:  " << msgCommand << std::endl;
            std::cout << "msgData: " << msgData << std::endl;
        }

        if (msgCommand == "OPEN_CONNECT"){
            // código para verificar pedido e gerar resposta ao open_connect
            t_message msgSend;
            t_string msgDataSend = "{\"command\":\"OPEN_CONNECT_RESPONSE\",\"data\":{\"source\":\"source\",\"destination\":\"destination\",\"qos\":{\"key_chunk_size\":3,\"max_bps\":5,\"min_bps\":1,\"jitter\":4,\"priority\":5,\"timeout\":0,\"ttl\":10,\"metadata_mimetype\":\"metadata\"},\"key_stream_id\":\"key_stream_id\"}}";
            msgSend.setMessageData(msgDataSend);

            if(getVerboseMode()){
                std::cout << "Response data: " << msgDataSend << std::endl;
                std::cout << "Response: " << msgSend << std::endl;
            }
            outputSignals[0]->bufferPut(&msgSend);

        } else if (msgCommand == "OPEN_CONNECT_RESPONSE"){
            // código para gerar resposta ao open_connect_response
            // se estiver tudo bem é um GET_KEY
            // if(getVerboseMode()){
            //     continue;
            // }
        } else if (msgCommand == "GET_KEY"){
            // código para gerar resposta ao get_key
        }
        
    }

    //fazer pedido open_connect
    //json open_connectJson = etsi_qkd_004::open_connect(source,destination,qos,key_stream_id);
    if(getVerboseMode()){
        std::cout << "NOTHING" << std::endl;
    }

    
    return false;
    
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