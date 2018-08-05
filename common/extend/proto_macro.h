#pragma once

#include <google/protobuf/message.h>

typedef google::protobuf::Message PBMsg;

#ifndef PARSE_PROTO_MSG
#define PARSE_PROTO_MSG(ClassName) \
    ClassName msg; \
    ret = msg.ParseFromArray(pkg, pkgLen); \
    if(ret == false) \
    {\
        LOG_ERROR("%s Parse Failed", #ClassName); \
    }
#endif
