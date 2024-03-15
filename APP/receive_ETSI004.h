# define PROGRAM_RECEIVE_ETSI004_H_

#include "netxpto_20200819.h"

class ReceiveETSI004 : public Block {

public:

    ReceiveETSI004(std::initializer_list<Signal *> InputSig, std::initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

    void initialize(void);
    bool runBlock(void);

    std::vector<t_message> storedMessages = {};






private:

t_integer ready;
t_integer i;

};