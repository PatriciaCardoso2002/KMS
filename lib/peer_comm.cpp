#include "peer_comm.h"

json key_sync::KEY_SYNC(const URI &source, const URI &destination, const Status status, IndexBuffer &indexes,const QoS &qos){
    json key_sync_json ={
        {"command", "KEY_SYNC"},
        {"data", {
            {"source", source},
            {"destination", destination},
            {"qos", {
                {"key_type", qos.key_type},
                {"key_chunk_size", qos.key_chunk_size},
            }
        },
        },
        },
        {"status", status},
        {"indexBuffer", indexes},

    };
    return key_sync_json;
}

json key_sync::SYNC_INDEX(const std::vector<unsigned int> &sync_indexes){
    json sync_index_json={
        {"indexes", sync_indexes},
    };
    return sync_index_json;
}

json key_sync::NEW_KEY(const URI &source, const URI &destination, const Status status, const UUID &key_stream_id, IndexBuffer &indexes){
    json new_key_json={
        {"command", "NEW_KEY"},
        {"data", {
            {"source", source},
            {"destination", destination},
        },
        },
        {"status", status},
        {"index", indexes},
    };
    return new_key_json;
}