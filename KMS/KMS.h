#ifndef KMS_H
#define KMS_H
#include "netxpto_20200819.h"

Signal::t_write_mode sWriteMode{ Signal::t_write_mode::Ascii};
Signal::t_header_type hType{ Signal::t_header_type::fullHeader };

namespace SOUTH {

    extern Message request{"south_request.sgn",1000,hType,sWriteMode};
    extern HandlerMessage request_{"south_request_.sgn",1000,hType,sWriteMode};
    extern Message response{"south_response.sgn",1000,hType,sWriteMode};
    extern HandlerMessage response_{"south_response_.sgn",1000,hType,sWriteMode};
    extern Binary key{"south_key.sgn",1024,hType,sWriteMode};
    extern Message index{"south_index.sgn",5,hType,sWriteMode};

}

namespace NORTH {

    Message request{"north_request.sgn",10,hType,sWriteMode};
    HandlerMessage request_{"north_request_.sgn",10,hType,sWriteMode};
    Message response{"north_response.sgn",10,hType,sWriteMode};
    HandlerMessage response_{"north_response_.sgn",10,hType,sWriteMode};
    Binary key{"north_key.sgn",1024,hType,sWriteMode};
    Binary key_type{"north_key_type.sgn",5,hType,sWriteMode};

}

namespace KeySync {

    Message request{"sync_request.sgn",1000,hType,sWriteMode};
    HandlerMessage request_{"sync_request_.sgn",1000,hType,sWriteMode};
    Message response{"sync_response.sgn",1000,hType,sWriteMode};
    HandlerMessage response_{"sync_response_.sgn",1000,hType,sWriteMode};
    Message index{"south_index.sgn",5,hType,sWriteMode};
    Message discardIndex{"discard_index.sgn",5,hType,sWriteMode};

}

#endif