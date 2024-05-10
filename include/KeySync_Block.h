#define PROGRAM_KEY_SYNC_BLOCK_H_

#include "netxpto_20200819.h"
#include "peer_comm.h"
#include "json.hpp"


class KeySyncBlock : public Block {

public: 

    KeySyncBlock(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

        void initialize(void);
        bool runBlock(void);

        void setSource(key_sync::URI source){
            this->source = source;
        }
        
        key_sync::URI getSource(){
            return source;
        }

        void setDestination(key_sync::URI destination){
            this->destination = destination;
        }

        key_sync::URI getDestination(){
            return destination;
        }

        void setQoS(unsigned int key_type = 0, unsigned int key_chunk_size = 0){        
            this->qos.key_type = key_type;
            this->qos.key_chunk_size = key_chunk_size;
        }

        key_sync::QoS getQoS() const{
            return qos;
        }

private:
    std::vector<t_message> receivedMessages = {};

    std::vector<t_string> receivedIndexes = {};

    std::vector<t_string> indexes;
    
    t_message m_index;
    t_integer ready;

    json msgJson;
    json msgCommand;
    json msgData;

    key_sync::URI source = "source";
    key_sync::URI destination = "destination";
    key_sync::QoS qos;

};