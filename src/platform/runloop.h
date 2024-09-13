/*
 * runloop.h
 *
 * Created by Ruibin.Chow on 2024/09/13.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#ifndef RUNLOOP_H
#define RUNLOOP_H

#include "platform.h"

#define RunLoopID UINT32

typedef struct RunLoop {
    RunLoopID id;
    Boolean isExit;
} RunLoop;

typedef struct RunLoop* RunLoopRef;

RunLoopID RunLoopGetID(void);
RunLoopRef RunLoopGetCurrent(void);
RunLoopRef RunLoopGetMain(void);
Boolean RunLoopIsWaiting(RunLoopRef rl);
void RunLoopWakeUp(RunLoopRef rl);
void RunLoopStop(RunLoopRef rl);

void RunLoopInitMain(void);
void RunLoopRun(void);

#endif /* !RUNLOOP_H */
