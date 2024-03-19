# define PROGRAM_RECEIVE_ETSI004_H_

#include "netxpto_20200819.h"
#include "etsi_qkd_004.h"
#include "json.hpp"


class ReceiveETSI004 : public Block {

public:

    ReceiveETSI004(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);

    void setID(t_string Id){ID = Id;};
    t_string getID(){return ID;};

    void setQoS(unsigned int key_chunk_size, unsigned int key_nr, unsigned int max_bps, unsigned int min_bps, unsigned int jitter, unsigned int priority, unsigned int timeout, unsigned int ttl, std::string metadata_mimetype){
        this->qos.key_chunk_size = key_chunk_size;
        this->qos.key_nr = key_nr;
        this->qos.max_bps = max_bps;
        this->qos.min_bps = min_bps;
        this->qos.jitter = jitter;
        this->qos.priority = priority;
        this->qos.timeout = timeout;
        this->qos.ttl = ttl;
        this->qos.metadata_mimetype = metadata_mimetype;
    }
    
    etsi_qkd_004::QoS getQoS() const{
        return qos;
    }

    void setKeyStreamId(const etsi_qkd_004::UUID& key_stream_id){
        this->key_stream_id = key_stream_id;
    }

    etsi_qkd_004::UUID getKeyStreamId() const{
        return key_stream_id;
    }

    void setMetadataClient(const etsi_qkd_004::Metadata& metadata, unsigned int size, std::string buffer){
        this->metadata_client = metadata;
        this->metadata_client.size = size;
        this->metadata_client.buffer = buffer;
    }

    void setMetadataServer(const etsi_qkd_004::Metadata& metadata, unsigned int size, std::string buffer){
        this->metadata_server= metadata;
        this->metadata_server.size = size;
        this->metadata_server.buffer = buffer;
    }

    etsi_qkd_004::Metadata getMetadataClient() const{
        return metadata_client;
    }

    etsi_qkd_004::Metadata getMetadataServer() const{
        return metadata_server;
    }

    void setSource(etsi_qkd_004::URI source){
        this->source = source;
    }

    etsi_qkd_004::URI getSource(){
        return source;
    }

    void setDestination(etsi_qkd_004::URI destination){
        this->destination = destination;
    }

    etsi_qkd_004::URI getDestination(){
        return destination;
    }

private:

std::vector<t_message> storedMessages = {};
CircularBuffer<t_message> messagesToSend{ 5000 };

t_string ID;
t_integer ready;
t_integer i;
unsigned int get_keyResID = 0;
unsigned int get_keyID = 0;

etsi_qkd_004::URI source = "source";
etsi_qkd_004::URI destination = "destination";
etsi_qkd_004::QoS qos = {0,0,0,0,0,0,0,0,"metadata"};
etsi_qkd_004::UUID key_stream_id = "1";
etsi_qkd_004::Metadata metadata_client = {32,"metadata about the key requested"};
etsi_qkd_004::Metadata metadata_server = {32,"metadata about the key sent"};

};