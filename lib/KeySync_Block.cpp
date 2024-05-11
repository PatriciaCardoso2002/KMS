#include "KeySync_Block.h"
#include "peer_comm.h"
#include "netxpto_20200819.h"

void KeySyncBlock::initialize(void) {

}

bool KeySyncBlock::runBlock(void){
    
    bool alive = true;
    if (getTerminated()) return false;

    std::cout << "KeySyncBlock running\n";

    // fill the indexes vector with own indexes
    ready = inputSignals[1]->ready();
    if (ready){
        inputSignals[1]->bufferRead(&m_index);
        indexes.push_back(m_index.getMessageData());
        std::cout << "Index: ";
        std::cout << indexes.back() << std::endl;
    }

    // check peer message
    if (inputSignals[0]->ready()){
        t_message msgReceived;
        inputSignals[0]->bufferGet(&msgReceived);
        receivedMessages.push_back(msgReceived);
        msgJson = json::parse(msgReceived.getMessageData());
        msgCommand = msgJson["command"];
        msgData = msgJson["data"];

        if (msgCommand == "KEY_SYNC"){
            if(getVerboseMode()){
                std::cout << "RECEIVED KEY_SYNC" << std::endl;
            }
            //save the received indexes
            key_sync::IndexBuffer r_indexes = msgData["indexBuffer"].get<key_sync::IndexBuffer>();
            std::cout << "SIZE: " << r_indexes.size();
            for (int i = 0; i < msgData["indexBuffer"].size(); i++){
                //if index is not in receivedIndexes
                if (receivedIndexes.find(msgData["indexBuffer"][i].dump()) == receivedIndexes.end()) //index not found
                receivedIndexes.insert(msgData["indexBuffer"][i].dump());
                std::cout << "RECEIVED ID INSERTED: " << msgData["indexBuffer"][i].dump() << std::endl;
            }
            
        }
    }

    

    // check which of the received indexes were already sent
    for (const auto& index : receivedIndexes){
        if (sentIndexes.find(index) != sentIndexes.end()){
            std::cout << "CARALO" << std::endl;
            sync_indexes.push_back(std::stoul(index));
        }
    }

    
    if(!sync_indexes.empty()){
        std::cout << "Sync indexes: ";
        for (const auto& index : sync_indexes) {
        std::cout << index << ' ';
        }
        std::cout << '\n';

         // send signal to database to set sync true
        t_string msgDataSend = key_sync::SYNC_INDEX(sync_indexes).dump();
        t_message msgSend;
        msgSend.setMessageData(msgDataSend);
        outputSignals[1]->bufferPut(msgSend);
    }
    

   

    // send message to peer
    if (indexes.size() == 10){
        key_sync::Status status = key_sync::Status::SUCCESSFUL;

        t_string msgDataSend = key_sync::KEY_SYNC(source,destination,status,indexes,qos).dump();
        t_message msgSend;
        msgSend.setMessageData(msgDataSend);
        std::cout << "message" << msgDataSend << std::endl;
        outputSignals[0]->bufferPut(msgSend);
        sentIndexes.insert(indexes.begin(), indexes.end());
        indexes.clear();
    }

    return alive;

}