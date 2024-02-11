

#ifndef ETSI_QKD_004_H
#define ETSI_QKD_004_H


#include <unistd.h>
#include <uuid/uuid.h>

#include <string>
#include <stdexcept>
#include "json.hpp"
#include <thread>
#include "message_handler.h"
#include "netxpto_20200819.h"


using json = nlohmann::ordered_json;

namespace etsi_qkd_004 {

    using URI = std::string;
    using UUID = std::string;
    using KeyBuffer = typename std::vector<unsigned char>;


    struct Metadata {
        unsigned int size;
        std::string buffer;
    };

    struct QoS {
        unsigned int key_chunk_size;
        unsigned int max_bps;
        unsigned int min_bps;
        unsigned int jitter;
        unsigned int priority;
        unsigned int timeout;
        unsigned int ttl;
        std::string metadata_mimetype;
    };

    enum Status : unsigned int {
        SUCCESSFUL = 0, PEER_NOT_CONNECTED = 1, GK_INSUFFICIENT_KEY_AVAILABLE = 2, GK_PEER_NOT_CONNECTED = 3,
        NO_QKD_CONNECTION_AVAILABLE = 4, OC_KSID_ALREADY_IN_USE = 5, TIMEOUT_ERROR = 6, OC_QOS_NOT_SATISFIABLE = 7,
        GK_METADATA_INSUFFICIENT_SIZE = 8
    };

    t_handler_message open_connectToSignal(const json& openConnectJson){
        std::string command = openConnectJson["command"];
        json data = openConnectJson["data"];
        URI source = data["source"];
        URI destination = data["destination"];
        QoS qos;
        qos.key_chunk_size = data["qos"]["key_chunk_size"];
        qos.max_bps = data["qos"]["max_bps"];
        qos.min_bps = data["qos"]["min_bps"];
        qos.jitter = data["qos"]["jitter"];
        qos.priority = data["qos"]["priority"];
        qos.timeout = data["qos"]["timeout"];
        qos.ttl = data["qos"]["ttl"];
        qos.metadata_mimetype = data["qos"]["metadata_mimetype"];

        t_handler_message message;

        //Não posso usar estas funções porque a primeira não está implementada e outras duas são da classe t_message
/*
        message.setHeader(destination, source, qos.priority, 0, command, 0, "0");

        message.setMessageType(command);

        message.setMessageData(data.dump());
*/

        return message;
    }

    struct OpenConnectResponse {
        Status status;
        std::string key_stream_id;
        QoS qos;
    };

    struct GetKeyResponse {
        Status status;
        KeyBuffer key_buffer;
        unsigned int index;
        Metadata metadata;
    };

    struct CloseResponse {
        Status status;
    };

    json open_connect(const URI &source, const URI &destination, const QoS &qos,
                 const UUID &key_stream_id = "") {
        // Request to json
        json open_connect_json = {
                {"command", "OPEN_CONNECT"},
                {"data",    {
                                    {"source", source},
                                    {"destination", destination},
                                    {"qos", {
                                                    {"key_chunk_size", qos.key_chunk_size},
                                                    {"max_bps", qos.max_bps},
                                                    {"min_bps", qos.min_bps},
                                                    {"jitter", qos.jitter},
                                                    {"priority", qos.priority},
                                                    {"timeout", qos.timeout},
                                                    {"ttl", qos.ttl},
                                            }
                                    }
                            }
                },
        };
        if (!qos.metadata_mimetype.empty()) open_connect_json["data"]["qos"]["metadata_mimetype"] = qos.metadata_mimetype;
        if (!key_stream_id.empty()) open_connect_json["data"]["key_stream_id"] = key_stream_id;

        return open_connect_json;

    }

    json get_key(const UUID &key_stream_id, unsigned int index, const Metadata &metadata = {}) {
        // Request to json
        json get_key_json = {
                {"command", "GET_KEY"},
                {"data",    {
                                    {"key_stream_id", key_stream_id},
                                    {"index", index},
                            }
                },
        };
        if (metadata.size > 0) {
            json metadata_json = {
                    {"size",   metadata.size},
                    {"buffer", metadata.buffer},
            };
            get_key_json["data"]["metadata"] = metadata_json;
        }

        return get_key_json;

    }

    json close(const UUID &key_stream_id) {
        // Request to json
        json close_json = {
                {"command", "CLOSE"},
                {"data",    {{"key_stream_id", key_stream_id},}},
        };

        return close_json;
      
    }
}

#endif //ETSI_QKD_004_H
