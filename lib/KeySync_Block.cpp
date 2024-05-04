#include "KeySync_Block.h"
#include "key_sync.h"
#include "netxpto_20200819.h"

void KeySyncBlock::initialize(void) {

}

bool KeySyncBlock::runBlock(void){
    
    bool alive = true;
    if (getTerminated()) return false;

    if (inputSignals[0]->ready()){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        receivedMessages.push_back(msgReceived);
        msgJson = json::parse(msgReceived.getMessageData());
        msgCommand = msgJson["command"];
        msgData = msgJson["data"];
    }

    ready = inputSignals[1]->ready();
    
    std::cout << "KeySyncBlock running\n";

    key_sync::Status status = key_sync::Status::SUCCESSFUL;

    if (ready != 0){
        inputSignals[1]->bufferRead(&m_index);
        indexes.push_back(m_index.getMessageData());
        std::cout << "Index: ";
        std::cout << indexes.back() << std::endl;
    }

    t_string msgDataSend = key_sync::key_sync(source,destination,status,indexes,qos).dump();
    t_message msgSend;
    msgSend.setMessageData(msgDataSend);
    std::cout << "message" << msgDataSend << std::endl;
    outputSignals[0]->bufferPut(msgSend);


    if (msgCommand == "KEY_SYNC"){
        if(getVerboseMode()){
            std::cout << "RECEIVED KEY_SYNC" << std::endl;
        }

        //save the received indexes
        for (int i = 0; i < msgData["indexes"].size(); i++){
            //if index is not in receivedIndexes
            if (std::find(receivedIndexes.begin(), receivedIndexes.end(), msgData["indexes"][i].dump()) == receivedIndexes.end()) //index not found
                receivedIndexes.push_back(msgData["indexes"][i].dump());
            else{
                std::cout << "Index already received\n";
            }
        }


    }
    return alive;

}