#include <Kernel/Offsets.h>
#include <Kernel/KAllsyms.h>
#include <TBS.hpp>
#include <Kernel/Syms.h>

Offset KERNEL_TEXT;
Offset TASK_STACK_OFF;
Offset TASK_COMM_OFF;
Offset TASK_REAL_PARENT_OFF;
Offset TASK_GROUP_LEADER_OFF;
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
	/*if (auto proc = KallsymLookupName<uintptr_t>("....."))
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("...")
			.setScanStart(proc)
			.setScanEnd(KAllsymSymbolSizeGet(proc), true);
	}
	else return 1;*/

	if (auto proc = KallsymLookupName<uintptr_t>("walk_process_tree"))
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("TGLO")
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

	auto getTaskComm = KallsymLookupName<uintptr_t>("__get_task_comm");
	if(!getTaskComm)
		getTaskComm = KallsymLookupName<uintptr_t>("get_task_comm");

	if (auto proc = getTaskComm)
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("TCO")
			.setScanStart(proc)
			.setScanEnd(KAllsymSymbolSizeGet(proc), true);

		state.AddPattern(
			builder
			.Clone()
			.setPattern("49 ? ? ? ? ? ? 4C")
			.AddTransformer([](auto& _, Result r) {
				return *(uint32_t*)(r + 3);
				})
			.Build()
		);
	}
	else return 1;

	if (auto proc = KallsymLookupName<uintptr_t>("ptrace_traceme"))
	{
		auto builder = builderStopFirst
			.Clone()
			.setUID("TRPO")
			.setScanStart(proc)
			.setScanEnd(KAllsymSymbolSizeGet(proc), true);

		state.AddPattern(
			builder
			.Clone()
			.setPattern("48 ? ? ? ? ? ? f6 ? ? ? 75")
			.AddTransformer([](auto& _, Result r) {
				return *(uint32_t*)(r + 3);
				})
			.Build()
		);
	}	

	if (!TBS::Scan(state))
		return 2;

	TASK_GROUP_LEADER_OFF = state["TGLO"];
	TASK_COMM_OFF = state["TCO"];
	TASK_REAL_PARENT_OFF = state["TRPO"];

	return 0;
}