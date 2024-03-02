#include "load_request.h"

void LoadRequest::initialize(void){
    outputSignals[0]->setSymbolPeriod(symbolPeriod);
    outputSignals[0]->setSamplingPeriod(samplingPeriod);
}

bool LoadRequest::runBlock(void){
    int space = outputSignals[0]->space();
    signal_value_type sTypeOut = outputSignals[0]->getValueType();
    t_string request = "{\"command\":\"OPEN_CONNECT\",\"data\":{\"source\":\"source\",\"destination\":\"destination\",\"qos\":{\"key_chunk_size\":3,\"max_bps\":5,\"min_bps\":1,\"jitter\":4,\"priority\":5,\"timeout\":0,\"ttl\":10,\"metadata_mimetype\":\"metadata\"},\"key_stream_id\":\"key_stream_id\"}";
    int requestSize= request.size();
    
    if(getFirstTime()){
        setFirstTime(false);
        ready = requestSize;
    }
    if (ready == 0){
        return true;
    }

    for (t_char c : request){
        ready--;
        outputSignals[0]->bufferPut(c);
    }
    return false;
}