#include <Kernel/Decls.h>
#include <etl/vector.h>
#include <Kernel/Syms.h>

using AtExitObjectCallback = void(*)(void*);
using Object = void*;

struct AtExitObjectCallbackDesc {

    AtExitObjectCallbackDesc(AtExitObjectCallback callback, Object object)
        : mCallback(callback)
        , mObject(object)
    {}

    AtExitObjectCallback mCallback;
    Object mObject;
};

etl::vector<AtExitObjectCallbackDesc, 128> gAtExitCallbacks;

EXPORT(int) __cxa_atexit(AtExitObjectCallback callback, Object obj, void*)
{
    gAtExitCallbacks.emplace_back(AtExitObjectCallbackDesc(callback, obj));
    return 0;
}

EXPORT(int) __cxa_finalize(void*)
{
    for (const auto& callback : gAtExitCallbacks)
        callback.mCallback(callback.mObject);

    return 0;
}

