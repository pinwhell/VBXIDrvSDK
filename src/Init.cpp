#include <cstdint>
#include <Kernel/Syms.h>
#include <TBS.hpp>

using namespace TBS;

// SDK Initialization

void TestTBSScan()
{
    TBS::UByte buff[] = {
        0xAA, 0xB1, 0x2C, 0xDD, 0xE2, 0xFF
    };

    TBS::State<2> state(buff, buff + sizeof(buff));

    auto& desc = state.AddPattern(
        state.PatternBuilder()
        .setPattern("AA B? ?C DD E? FF")
        .setUID("TestPattern")
        .AddTransformer([](TBS::Pattern::Description& desc, TBS::Pattern::Result result) -> TBS::Pattern::Result {
            KLOG_PRINT("TBS Middleware Working, Resuilt %llx!!!", result);
            // Middleware for results found
            return 0x1;
            })
        .Build()
                ).mDescriptionts[0];

            if (TBS::Scan(state))
                KLOG_PRINT("TBS Working!!!");

            Pattern::Result result = state["TestPattern"];
            KLOG_PRINT("TBS Result %llx!!!", result);
}

int SDKInit(uintptr_t entry)
{
    TestTBSScan();

	KLOG_PRINT("SDKInitialized()");

	return 0;
}