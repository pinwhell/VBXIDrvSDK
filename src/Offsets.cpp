#include <Kernel/Offsets.h>
#include <Kernel/KAllsyms.h>
#include <TBS.hpp>
#include <Kernel/Syms.h>

Offset TASK_STACK_OFF;
Offset TASK_COMM_OFF;
Offset TASK_REAL_PARENT_OFF;
Offset TASK_GROUP_LEADER_OFF;
Offset TASK_STACK_REGS_BASE;
Offset FILE_FOFFSET;
Offset DENTRY_PARENT;
Offset DENTRY_INODE;

template<typename DescBuilderT>
bool TBSDescBulderSymScanRangeSet(DescBuilderT& builder, const char* symName)
{
	auto proc = KallsymLookupName<uintptr_t>(symName);

	if (!proc)
		return false;

	builder
		.setScanStart(proc)
		.setScanEnd(KAllsymSymbolSizeGet(proc), true);

	return true;
}

int SDKOffsetsInit()
{
	static TBS::State<> state(KERNEL_TEXT, KERNEL_TEXT + 16 * 1024 * 1024);
	
	static auto builderStopFirst = state
		.PatternBuilder()
		.stopOnFirstMatch();

	using Result = TBS::Pattern::Result;

	{
		static auto builder = builderStopFirst
			.Clone()
			.setUID("TGLO");

		if (TBSDescBulderSymScanRangeSet(builder, "do_notify_parent"))
		{
			state.AddPattern(
				builder
				.Clone()
				.setPattern("48 ? ? ? ? ? ? 75 ? 48")
				.AddTransformer([](auto& _, Result r) {
					return *(uint32_t*)(r + 3);
					})
				.Build()
			);

			state.AddPattern(
				builder
				.Clone()
				.setPattern("3b ? ? ? ? ? 75 ? 8b")
				.AddTransformer([](auto& _, Result r) {
					return *(uint32_t*)(r + 2);
					})
				.Build()
			);
		}
		else return 1;
	}

	{
		static auto builder = builderStopFirst
			.Clone()
			.setUID("TCO");

		if (TBSDescBulderSymScanRangeSet(builder, "get_task_comm") ||
			TBSDescBulderSymScanRangeSet(builder, "__get_task_comm"))
		{
			state.AddPattern(
				builder
				.Clone()
				.setPattern("49 ? ? ? ? ? ? 4C")
				.AddTransformer([](auto& _, Result r) {
					return *(uint32_t*)(r + 3);
					})
				.Build()
						);

			state.AddPattern(
				builder
				.Clone()
				.setPattern("8d ? ? ? ? ? b9")
				.AddTransformer([](auto& _, Result r) {
					return *(uint32_t*)(r + 2);
					})
				.Build()
						);
		}
		else return 1;
	}

	{
		static auto builder = builderStopFirst
			.Clone()
			.setUID("TRPO");

		int finding = 0;

		if (TBSDescBulderSymScanRangeSet(builder, "ptrace_traceme"))
		{
			finding++;
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

		if (TBSDescBulderSymScanRangeSet(builder, "sys_getppid"))
		{
			finding++;
			state.AddPattern(
				builder
				.Clone()
				.setPattern("64 ? ? ? ? ? 8b ? ? ? ? ? 8b")
				.AddTransformer([](auto& _, Result r) {
					return *(uint32_t*)(r + 8);
					})
				.Build()
			);
		}

		if (!finding)
			return false;
	}

	if (!TBS::Scan(state))
		return 2;

	TASK_GROUP_LEADER_OFF = state["TGLO"];
	TASK_COMM_OFF = state["TCO"];
	TASK_REAL_PARENT_OFF = state["TRPO"];

	return 0;
}