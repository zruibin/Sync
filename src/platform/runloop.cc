/*
 *
 * runloop.c
 *
 * Created by Ruibin.Chow on 2024/09/13.
 * Copyright (c) 2024年 Ruibin.Chow All rights reserved.
 */

#include "runloop.h"
#include <thread>
#include <chrono>
#include <unordered_map>

static std::unordered_map<RunLoopID, RunLoopRef> __runloopMap;
static UINT16 __mainID = 0;

RunLoopID RunLoopGetID(void) {
    return platform::thread_get_current_id();
}

RunLoopRef RunLoopGetCurrent(void) {
    return __runloopMap[RunLoopGetID()];
}

RunLoopRef RunLoopGetMain(void) {
    return __runloopMap[__mainID];
}

Boolean RunLoopIsWaiting(RunLoopRef rl) {
    return false;
}

void RunLoopWakeUp(RunLoopRef rl) {
    
}

void RunLoopStop(RunLoopRef rl) {
    
}

void RunLoopInitMain(void) {
    __mainID = platform::thread_get_current_id();
}

void RunLoopRun(void) {
    UINT16 threadId = platform::thread_get_current_id();
    if (__runloopMap.find(threadId) != __runloopMap.end()) {
        return;
    }
    RunLoopRef runloopRef = new RunLoop();
    runloopRef->id = threadId;
    __runloopMap.try_emplace(threadId, runloopRef);
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (runloopRef->isExit) {
            break;
        }
    }
}

