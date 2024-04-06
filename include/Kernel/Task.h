#pragma once

#include <stdint.h>
#include <etl/string.h>

using Task = uintptr_t;

using TaskComm = etl::string<16>;

Task TaskCurrentGet();
TaskComm TaskCommGet(Task task);