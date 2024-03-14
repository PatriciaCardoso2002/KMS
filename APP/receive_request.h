# define PROGRAM_RECEIVE_REQUEST_H_

#include "netxpto_20200819.h"

class ReceiveRequest : public Block {

public:

    ReceiveRequest(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);




private:

std::streampos position = 0;

//unsigned long int index = 0;

int ready{ 1 };
double samplingPeriod{ 1 };
double symbolPeriod{ 1 };
double auxMultiply = 1;
signal_value_type dataType{ signal_value_type::t_char };

t_string request = "{\"command\":\"OPEN_CONNECT\",\"data\":{\"source\":\"source\",\"destination\":\"destination\",\"qos\":{\"key_chunk_size\":3,\"max_bps\":5,\"min_bps\":1,\"jitter\":4,\"priority\":5,\"timeout\":0,\"ttl\":10,\"metadata_mimetype\":\"metadata\"},\"key_stream_id\":\"key_stream_id\"}";


bool cycleModulos = true; // mandates if the program should read files on loop (module)


unsigned long int startColumn = 1;
unsigned long int startLine = 1;
unsigned long int auxPosition = 0;

unsigned long int numberOfLoadedValues{ 0 };
};