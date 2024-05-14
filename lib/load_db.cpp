#include "load_db.h"
#include <vector>
#include <string>
#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "peer_comm.h"


//IS0 -> SessionInfo(key_stream_id, keyType, key_chunk_size)
//IS1 -> NewKey_ACK
//OS0 -> NewKey
//OS1 -> Key

//Access the databse and go,in the right order starting in SEQ=0, select raw_key material and unify each SEQ to form a key
//This key is going to have the size of key_chunk_size
//After i get each SEQ, i need to subtract the key_chunk_size from the size of the key_material
//Each SEQ divided in indexes of size 32
//If the size of the key_material is less than key_chunk_size, i need to get the next SEQ
//I always need to specify the size_used of each index inside a SEQ



void LoadDB::initialize(void){
    setDBConnection(IP,"kms","YourStrongPassword","KeyStorage");
}

bool LoadDB::runBlock(void){


    stmt = connection->createStatement();
    std::string key; // string to store the unified key

    std::map<int,std::vector<int>>indexes;

    if (inputSignals[0]->ready()){
        t_message msg_sessionInfo;
        inputSignals[0]->bufferGet(&msg_sessionInfo);
        sessionInfoJson = json::parse(msg_sessionInfo.getMessageData());
        key_stream_id = sessionInfoJson["key_stream_id"];
        msg_index = sessionInfoJson["msg_index"];
        NEW_KEY_SENT new_key_sent;
        new_key_sent.key_type = sessionInfoJson["key_type"];
        new_key_sent.key_chunk_size = sessionInfoJson["key_chunk_size"];
        SentMessages[key_stream_id].push_back(new_key_sent);
    }
    unsigned int total_size = 0;

    unsigned int key_chunk_size = SentMessages[key_stream_id][msg_index].key_chunk_size;
    unsigned int kType = SentMessages[key_stream_id][msg_index].key_type;
    

    qos.key_chunk_size = key_chunk_size;
    qos.key_type = kType;

    switch(kType){
        case 0: // SYMMETRIC
            // Retrieve keys from db
            res = stmt->executeQuery("SELECT * FROM raw_key_store_symmetric WHERE sync = 1 AND size_used = 0 ORDER BY seq ASC");
            while (res->next() && total_size < SentMessages[key_stream_id][msg_index].key_chunk_size) {
                // Access data
                int seq = res->getInt("seq");
                int id = res->getInt("id");
                int size = res->getInt("size");
                int size_used = res->getInt("size_used");
                // Update size_used in the database
                std::string query = "UPDATE raw_key_store_symmetric SET size_used = size_used + " + std::to_string(size) + " WHERE seq = " + std::to_string(seq);
                stmt->execute(query);
                total_size += size;
                indexes[seq].push_back(id);   
            }
            SentMessages[key_stream_id][msg_index].indexes = indexes;
            msgDataSend = key_sync::NEW_KEY(source,destination,key_stream_id,msg_index,qos,indexes).dump();
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
            delete res;
            delete stmt;
            break;
        case 1: // OBLIVIOUS
            // Retrieve keys from db
            res = stmt->executeQuery("SELECT * FROM raw_key_store_oblivious WHERE sync = 1 AND size_used = 0 ORDER BY seq ASC");
            while (res->next() && total_size < qos.key_chunk_size) {
                // Access data
                int seq = res->getInt("seq");
                int id = res->getInt("id");
                int size = res->getInt("size");
                int size_used = res->getInt("size_used");
                // Update size_used in the database
                std::string query = "UPDATE raw_key_store_symmetric SET size_used = size_used + " + std::to_string(size) + " WHERE seq = " + std::to_string(seq);
                stmt->execute(query);
                total_size += size;
                indexes[seq].push_back(id);   
            }
            msgDataSend = key_sync::NEW_KEY(source,destination,key_stream_id,qos,indexes).dump();
            msgSend.setMessageData(msgDataSend);
            outputSignals[0]->bufferPut(msgSend);
            delete res;
            delete stmt;
            break;
        default:
            break;
    }    

        if (inputSignals[1]->ready()){
            t_message msg_newKeyAck;
            inputSignals[1]->bufferGet(&msg_newKeyAck);
            json newKeyAckJson = json::parse(msg_newKeyAck.getMessageData());
            msgCommand = newKeyAckJson["command"];
            msgData = newKeyAckJson["data"];
        }

        if (msgCommand == "NEW_KEY"){
            if(getVerboseMode()){
                std::cout << "NEW_KEY RECEIVED" << std::endl;
            }
            std::map<int,std::vector<int>>indexes_ACK;
            indexes_ACK = msgData["indexes"].get<std::map<int,std::vector<int>>>();

            t_string msgDataSend = key_sync::NEW_KEY_ACK(source,destination,key_stream_id).dump();
            t_message msgSend;
            msgSend.setMessageData(msgDataSend);
            outputSignals[1]->bufferPut(msgSend);
        }

        if (msgCommand == "NEW_KEY_ACK"){
            if(getVerboseMode()){
                std::cout << "NEW_KEY_ACK RECEIVED" << std::endl;
            }
            // Retrieve keys from db using the std::map<int,std::vector<int>>indexes
            switch(kType){
                case 0: // SYMMETRIC
                    for (auto const& [seq, ids] : indexes) {
                        for (auto const& id : ids) {
                            res = stmt->executeQuery("SELECT * FROM raw_key_store_symmetric WHERE id = " + std::to_string(id) + "AND seq = " + std::to_string(seq));
                            while (res->next()) {
                                // Access data
                                int id = res->getInt("id");
                                std::string key_material = res->getString("key_material");
                                int size = res->getInt("size");
                                key += key_material;
                            }
                        }
                    }
                    break;
                case 1: // OBLIVIOUS
                    for (auto const& [seq, ids] : indexes) {
                        for (auto const& id : ids) {
                            res = stmt->executeQuery("SELECT * FROM raw_key_store_oblivious WHERE id = " + std::to_string(id) + "AND seq = " + std::to_string(seq));
                            while (res->next()) {
                                // Access data
                                int id = res->getInt("id");
                                std::string key_material = res->getString("key_material");
                                int size = res->getInt("size");
                                key += key_material;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            outputSignals[2]->bufferPut(key);
        }


    return true;
}