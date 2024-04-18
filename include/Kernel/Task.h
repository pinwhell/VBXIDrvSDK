#pragma once

#include <stdint.h>
#include <etl/string.h>
#include <etl/delegate.h>

using Task = uintptr_t;

using TaskComm = etl::string<17>;

Task TaskCurrentGet();
Task ProcessCurrentGet();
Task TaskByPidGet(size_t vpid);
Task TaskGroupLeaderGet(Task tsk);
Task TaskGroupLeaderParentGet(Task tsk);
void TaskForEachProcessAncestor(Task tsk, etl::delegate<bool(Task)> callback);
void TaskForEachTaskAncestor(Task tsk, etl::delegate<bool(Task)> callback);
TaskComm TaskCommGet(Task task);