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
                receivedIndexes.insert(index);
                std::cout << "RECEIVED ID INSERTED: " << index << std::endl;
                lastReceivedIndex = std::stoul(index);
            }
        
            for (const auto& index : r_indexes) {
                r_IndexesInt.insert(std::stoul(index));
            }

            // check if there are missing indexes
            auto it = r_IndexesInt.begin();
            auto next_it = std::next(it);

            while (next_it != r_IndexesInt.end()) { 
                int dif = *next_it - *it;
                if (dif != 1) {
                    std::cout << "Difference between " << *it << " and " << *next_it << " is not 1." << std::endl;
                    for (int i = 1; i < dif; i++)
                    {
                        unsigned int discardedIndex = *it + i;
                        NotFoundIndexes.insert(discardedIndex);
                        std::cout << "DISCARDED INDEX INSERTED: " << discardedIndex << std::endl;
                    }
                    
                }
                it = next_it;
                ++next_it;
            }

        }
    }

    // check which of the received indexes were already sent
    for (const auto& index : receivedIndexes){
        if (sentIndexes.find(index) != sentIndexes.end()){
            sync_indexes.push_back(std::stoul(index));
        }
        if (std::stoul(index) == currentIndex){
            currentIndexMentioned = true;
        }
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
        std::cout << "SENT KEY SYNC TO PEER" << std::endl;
    }

    if(!sync_indexes.empty()){
        // send signal to database to set sync true
        t_string msgDataSend = key_sync::SYNC_INDEX(sync_indexes).dump();
        std::cout << "SYNC INDEXES" << std::endl;
        t_message msgSend;
        msgSend.setMessageData(msgDataSend);
        outputSignals[1]->bufferPut(msgSend);
        sync_indexes.clear();
    }
    std::set<unsigned int> indexesToErase;

    // check which of the indexes to discard were already sent
    for (const auto& index : NotFoundIndexes){
        std::cout << "INDEX TO DISCARD: " << index << std::endl;
        if (sentIndexes.find(std::to_string(index)) != sentIndexes.end()){
            indexesToErase.insert(index);
            std::cout << "erase" << std::endl;
        }
    }

    std::cout << "fora do for" << std::endl;


    for (const auto& index : indexesToErase) {
        NotFoundIndexes.erase(index);
    }

    // send message to database to discard indexes
    for (const auto& index : NotFoundIndexes){
        std::cout << "antes de push_back" << std::endl;
        NotFoundIndexesVector.push_back(index);
        std::cout << "depois de push_back" << std::endl;
    }
    std::cout << "DISCARD INDEXES VECTOR: ";
    t_string msgDataSend = key_sync::DISCARD(NotFoundIndexesVector).dump();
    std::cout << "DISCARD INDEXES: " << msgDataSend << std::endl;
    t_message msgSend;
    msgSend.setMessageData(msgDataSend);
    outputSignals[2]->bufferPut(msgSend);

    return alive;

}

//////////////////////////// NOT USED //////////////////////////////////////


/*     // check if the last index was mentioned in the received indexes  
    if (!receivedIndexes.empty()){ 
        if (!currentIndexMentioned && lastReceivedIndex > currentIndex){
            std::cout << "DISCARDED INDEX: " << currentIndex << std::endl;
            t_string msgDataSend = std::to_string(currentIndex);
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[2]->bufferPut(msgSend);
        }
    } */