#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <omnetpp.h>

// Helper to add the filename, function name and line number to EV log statements
#define HERE __FILE__ << "::" << __func__ << ":" << __LINE__ << " (" << getFullName() << "):"

#define tryHandleMessage(msg, T, gateId, callback) \
if (dynamic_cast<T>(msg) != nullptr) \
{ \
    if (msg->arrivedOn(gateId)) \
    { \
        EV << HERE << "INFO: " << msg->getName() << "arrived successfully." << std::endl; \
        callback((T)msg); \
        return; \
    } \
    else \
    { \
        EV << HERE << "ERROR: " << msg->getName() << "arrived on an unexpected gate." << std::endl; \
        delete msg; \
        return; \
    } \
}

#define tryHandleMessageAnyGate(msg, T, callback) \
if (dynamic_cast<T>(msg) != nullptr) \
{ \
    EV << HERE << "INFO: " << msg->getName() << "arrived successfully." << std::endl; \
    callback((T)msg); \
    return; \
}


#endif /* UTILS_H_ */
