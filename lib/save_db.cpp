#include "save_db.h"
#include <vector>
#include <string>
#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

void SaveDB::initialize(void){
    setDBConnection("172.17.0.2","kms","YourStrongPassword","KeyStorage");
}

bool SaveDB::runBlock(void){

    int ready = inputSignals[0]->ready(); //keys
    auto process = ready;
    stmt = connection->createStatement();

    std::string vals; // string to store keys

    switch(kType){
        case 0: // SYMMETRIC
            if (saveType) { // b64
                for(auto k = 0; k < process; k++)
                {
                    t_binary val{0};
                    inputSignals[0]->bufferGet(&val);
                    vals.push_back(static_cast<char>(val));
                }
            } else {    // ASCII   
                for(auto k = 0; k < process; k++)
                {
                    t_binary val{0};
                    inputSignals[0]->bufferGet(&val);
                    if (val == 0) vals.push_back('0');
                    if (val == 1) vals.push_back('1');
                    vals.push_back(val);
                }
            }

            // Insert key material and index into db
            if(inputSignals[1]->ready()){
                t_message id;
                inputSignals[1]->bufferGet(&id);
                stmt->execute("INSERT INTO raw_key_store_symmetric (key_material, id) VALUES ('" + vals + "', " + id.getMessageData() + " )");
            }

            // check if there are indexes to sync and change sync value
            if(inputSignals[2]->ready()){
                t_message sync;
                inputSignals[2]->bufferGet(&sync);
                json sync_json = json::parse(sync.getMessageData());
                std::vector<unsigned int> sync_indexes = sync_json["indexes"].get<std::vector<unsigned int>>();
                for (unsigned int index : sync_indexes) {
                    std::string query = "UPDATE raw_key_store_symmetric SET sync = 1 WHERE id = " + std::to_string(index);
                    stmt->execute(query);
                }
            }

            res = stmt->executeQuery("SELECT * FROM raw_key_store_symmetric");
            while (res->next()) {
                // Access data
                std::cout << "\tKey Store Symmetric Record:" << std::endl;
                std::cout << "\tksid: " << res->getInt("ksid") << std::endl;
                std::cout << "\tid: " << res->getInt("id") << std::endl;
                std::cout << "\thash: " << res->getString("hash") << std::endl;
                std::cout << "\texpiration_timestamp: " << res->getString("expiration_timestamp") << std::endl;
                std::cout << "\tsuspended: " << res->getBoolean("suspended") << std::endl;
                std::cout << "\tcreation_timestamp: " << res->getString("creation_timestamp") << std::endl;
                std::cout << "\tused: " << res->getBoolean("used") << std::endl;
                std::cout << "\tkey_material: " << res->getString("key_material") << std::endl;
            }
            delete res;
            delete stmt;
            
            break;

        case 1: // OBLIVIOUS
            if (saveType) { // b64
                for(auto k = 0; k < process; k++)
                {
                    t_binary val{0};
                    inputSignals[0]->bufferGet(&val);
                    vals.push_back(static_cast<char>(val));
                }
            } else {    // ASCII   
                for(auto k = 0; k < process; k++)
                {
                    t_binary val{0};
                    inputSignals[0]->bufferGet(&val);
                    if (val == 0) vals.push_back('0');
                    if (val == 1) vals.push_back('1');
                    vals.push_back(val);
                }
            }

            // Insert key material and index into db
            if(inputSignals[1]->ready()){
                t_message id;
                inputSignals[1]->bufferGet(&id);
                stmt->execute("INSERT INTO raw_key_store_oblivious (key_material, id) VALUES ('" + vals + "', " + id.getMessageData() + " )");
            }

            // check if there are indexes to sync and change sync value
            if(inputSignals[2]->ready()){
                t_message sync;
                inputSignals[2]->bufferGet(&sync);
                json sync_json = json::parse(sync.getMessageData());
                std::vector<unsigned int> sync_indexes = sync_json["indexes"].get<std::vector<unsigned int>>();
                for (unsigned int index : sync_indexes) {
                    std::string query = "UPDATE raw_key_store_symmetric SET sync = 1 WHERE id = " + std::to_string(index);
                    stmt->execute(query);
                }
            }

            res = stmt->executeQuery("SELECT * FROM raw_key_store_oblivious");
            while (res->next()) {
                // Access data
                std::cout << "\tKey Store Oblivious Record:" << std::endl;
                std::cout << "\tksid: " << res->getInt("ksid") << std::endl;
                std::cout << "\tid: " << res->getInt("id") << std::endl;
                std::cout << "\thash: " << res->getString("hash") << std::endl;
                std::cout << "\texpiration_timestamp: " << res->getString("expiration_timestamp") << std::endl;
                std::cout << "\tsuspended: " << res->getBoolean("suspended") << std::endl;
                std::cout << "\tcreation_timestamp: " << res->getString("creation_timestamp") << std::endl;
                std::cout << "\tused: " << res->getBoolean("used") << std::endl;
                std::cout << "\tkey_material: " << res->getString("key_material") << std::endl;
            }
            delete res;
            delete stmt;
            
            break;
        default:
            break;
    }

    return true;
}

// void SaveDB::setDBConnection(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
//     try {
//         // Initialize MySQL Connector/C++
//         driver = get_driver_instance();

//         // Create a connection
//         connection = driver->connect("tcp://" + host + ":3306", user, password);
//         connection->setSchema(database);
//     } catch (sql::SQLException& e) {
//         // Handle exceptions
//         std::cerr << "SQLException: " << e.what() << std::endl;
//     }
// }


// void SaveDB::setKeyType(unsigned int keyType){
//     kType = keyType;
// }