#pragma once

#include <stdint.h>
#include <etl/string.h>
#include <etl/delegate.h>

using Task = uintptr_t;

using TaskComm = etl::string<17>;

Task TaskCurrentGet();
Task TaskCurrentGetParent();
TaskComm TaskCommGet(Task task);
Task TaskByPidGet(size_t vpid);