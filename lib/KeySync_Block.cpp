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

        // Update currentIndex with the last index received
        currentIndex = std::stoul(indexes.back());
        std::cout << "currentIndex: " << currentIndex << std::endl;
    }

    bool currentIndexMentioned = false;

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
            for (const auto& index : r_indexes) {
                if (std::stoul(index) <= currentIndex){
                    currentIndexMentioned = true;
                    std::cout << "CURRENT INDEX MENTIONED: " << index << std::endl;
                }
                if (currentIndexMentioned){
                    // Add the index to the receivedIndexes set
                    receivedIndexes.insert(index);
                    std::cout << "RECEIVED ID INSERTED: " << index << std::endl;
                }
                else{
                    std::cout << "RECEIVED ID NOT INSERTED: " << index << std::endl;
                }
            }
            r_indexes.clear();
        }
    }

    // check which of the received indexes were already sent
    for (const auto& index : receivedIndexes){
        if (sentIndexes.find(index) != sentIndexes.end()){
            sync_indexes.push_back(std::stoul(index));
        }
    }

    
    if(!sync_indexes.empty()){
        if (sync_indexes != prev_sync_indexes) {
            // There are new indexes in sync_indexes
            std::cout << "GOING TO SEND SYNC INDEX" << std::endl;
            // send signal to database to set sync true
            t_string msgDataSend = key_sync::SYNC_INDEX(sync_indexes).dump();
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[1]->bufferPut(msgSend);

            // Update prev_sync_indexes with the current state of sync_indexes
            prev_sync_indexes = sync_indexes;
        }
    }
    std::cout << "OUT" << std::endl;
    

   

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