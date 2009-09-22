#ifndef RECEIVER_H
#define RECEIVER_H

#include "ipmessage.h"

int receiverThreadStart();

void receiverRun(void);

IPMessage* receiverAccept();


#endif

