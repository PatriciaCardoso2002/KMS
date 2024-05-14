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
    }

    key_stream_id = sessionInfoJson["key_stream_id"];
    keyType = sessionInfoJson["key_type"];
    key_chunk_size = sessionInfoJson["key_chunk_size"];

    unsigned int total_size = 0;

    switch(kType){
        case 0: // SYMMETRIC
            // Retrieve keys from db
            res = stmt->executeQuery("SELECT * FROM raw_key_store_symmetric WHERE sync = 1 AND size_used = 0 ORDER BY seq ASC");
            while (res->next() && total_size < key_chunk_size) {
                // Access data
                int seq = res->getInt("seq");
                int id = res->getInt("id");
                int size = res->getInt("size");
                int size_used = res->getInt("size_used");
                //check if the size of the key_material is less than key_chunk_size
                // Update size_used in the database
                std::string query = "UPDATE raw_key_store_symmetric SET size_used = size_used + " + std::to_string(size) + " WHERE seq = " + std::to_string(seq);
                stmt->execute(query);
                total_size += size;
                indexes[seq].push_back(id);   
                }
            }
            t_string msgDataSend = key_sync::NEW_KEY(source,destination,key_stream_id,keyType,key_chunk_size,indexes).dump();

            delete res;
            delete stmt;
            break;

        case 1: // OBLIVIOUS
            // Retrieve keys from db
            res = stmt->executeQuery("SELECT * FROM raw_key_store_oblivious WHERE sync = 1 ORDER BY seq ASC");
            while (res->next()) {
                // Access data
                std::string key_material = res->getString("key_material");
                key += key_material;
                // If the size of the key is equal to or greater than key_chunk_size, put it into the output buffer
                if (key.size() >= key_chunk_size) {
                    outputSignals[0]->bufferPut(key.substr(0, key_chunk_size));
                    key = key.substr(key_chunk_size);
                }
                // Update size_used in the database
                std::string query = "UPDATE raw_key_store_oblivious SET size_used = size_used + " + std::to_string(key_chunk_size) + " WHERE seq = " + std::to_string(res->getInt("seq"));
                stmt->execute(query);
            }
            delete res;
            delete stmt;
            break;
        default:
            break;
    }

    return true;
}