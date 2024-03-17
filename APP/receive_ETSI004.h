# define PROGRAM_RECEIVE_ETSI004_H_

#include "netxpto_20200819.h"

class ReceiveETSI004 : public Block {

public:

    ReceiveETSI004(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);

    void setID(t_string Id){ID = Id;};
    t_string getID(){return ID;};

    std::vector<t_message> storedMessages = {};






private:

CircularBuffer<t_message> messagesToSend{ 5000 };

t_string ID;

t_integer ready;
t_integer i;

};