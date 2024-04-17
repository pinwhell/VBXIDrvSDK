#include <Kernel/Offsets.h>
#include <Kernel/KAllsyms.h>
#include <TBS.hpp>
#include <Kernel/Syms.h>

Offset KERNEL_TEXT;
Offset TASK_STACK_OFF;
Offset TASK_PARENT_OFF;
Offset TASK_COMM_OFF = 0x720;
Offset TASK_REAL_PARENT_OFF = 0x578;
Offset TASK_GROUP_LEADER_OFF = 0x5A8;
Offset TASK_STACK_REGS_BASE;
Offset FILE_FOFFSET;
Offset DENTRY_PARENT;
Offset DENTRY_INODE;

int SDKOffsetsInit()
{
	KERNEL_TEXT = KallsymLookupName<Offset>("_text");

	if (!KERNEL_TEXT)
		return 1;

	TBS::State<> state(KERNEL_TEXT, KERNEL_TEXT + 16 * 1000 * 1000);

	auto builderStopFirst = state
		.PatternBuilder()
		.stopOnFirstMatch();

	using Result = TBS::Pattern::Result;

	// Template
	/*if (auto proc = KallsymLookupName<uintptr_t>("walk_process_tree"))
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("TSGL")
			.setScanStart(proc)
			.setScanEnd(KAllsymSymbolSizeGet(proc), true);
	}
	else return 1;*/

	if (auto proc = KallsymLookupName<uintptr_t>("walk_process_tree"))
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("TSGL")
			.setScanStart(proc)
			.setScanEnd(KAllsymSymbolSizeGet(proc), true);

		state.AddPattern(
			builder
			.Clone()
			.setPattern("E8 ? ? ? ? 49 ? ? ? ? ? ? ? 49")
			.AddTransformer([](auto& _, Result r) {
				return *(uint32_t*)(r + 9);
				})
			.Build()
		);
	} else return 1;

	if (!TBS::Scan(state))
		return 2;

	return 0;
}