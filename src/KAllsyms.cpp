#include <Kernel/KAllsyms.h>
#include <Kernel/Decls.h>
#include <etl/unordered_map.h>
#include <etl/string.h>
#include <BidirectionalMap.h>
#include <Kernel/Syms.h>
#include <TBS.hpp>

size_t KERNEL_TEXT;
uint32_t* gSymbolTable;

#define KSYM_NAME_LEN 128
#define KSOL_SYMBOL_COUNT_EDGE 105000

REGPARAMDECL(uintptr_t) kallsyms_sym_address(int idx);

uintptr_t KAllsymSymbolAddress(int index)
{
	if (&kallsyms_sym_address)
		return kallsyms_sym_address(index);

	if (gSymbolTable == nullptr)
		return 0;

	return KERNEL_TEXT + gSymbolTable[index];
}

BidirectionalMap<int, uintptr_t, 4096> gOffsetEntryCache;

int KAllsymSymbolIndexLookupByEntry(uintptr_t entry) {

	if (gOffsetEntryCache.containsValue(entry))
		return gOffsetEntryCache.getKey(entry);

	// Perform binary search on sorted symbol addresses
	int low = 0;
	int high = KSOL_SYMBOL_COUNT_EDGE - 1;

	while (low <= high) {
		int mid = low + (high - low) / 2;
		uintptr_t currSym = KAllsymSymbolAddress(mid);

		if (currSym == entry) {
			if (gOffsetEntryCache.forward.capacity() > 0) {
				gOffsetEntryCache.insert(mid, entry);
			}
			return mid;
		}
		else if (currSym < entry)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return -1; // Entry not found
}

etl::unordered_map<uintptr_t, size_t, 4096> gSymSizes;

size_t KAllsymSymbolSizeGet(uintptr_t symEntry, size_t defSize)
{
	if (gSymSizes.find(symEntry) != gSymSizes.end())
		return gSymSizes[symEntry];

	int currSymIndex = KAllsymSymbolIndexLookupByEntry(symEntry);

	if (currSymIndex < 0)
		return defSize;

	int nextSymIndex = currSymIndex + 1;

	if (gOffsetEntryCache.containsKey(nextSymIndex))
	{
		auto result = gOffsetEntryCache.getValue(nextSymIndex) - symEntry;

		if (gSymSizes.available() > 0)
			gSymSizes[symEntry] = result;

		return  result;

	}

	auto result = KAllsymSymbolAddress(nextSymIndex) - symEntry;

	if (gSymSizes.available() > 0)
		gSymSizes[symEntry] = result;

	return result;
}

size_t KAllsymSymbolSizeGet(const char* symbolName, size_t defSize)
{
	uintptr_t entry = (uintptr_t)kallsyms_lookup_name(symbolName);

	if (!entry)
		return defSize;

	return KAllsymSymbolSizeGet(entry, defSize);
}

bool LookupNameInitialize()
{
	if (&kallsyms_sym_address != nullptr)
		return true;

	auto stateStrg = etl::unique_ptr<TBS::State<>>(new TBS::State<>(KERNEL_TEXT, KERNEL_TEXT + 16 * 1024 * 1024));
	auto& state = *stateStrg;

	state.AddPattern(
		state.PatternBuilder()
		.setUID("LNST") // Lookup Name Symbol Table
		.setPattern("03 04 ? ? ? ? ? 83 ec ? 5b 5e 5f 5d")
		.AddTransformer([](auto& _, auto result) {
			return *(uint32_t*)(result + 3);
			})
		.stopOnFirstMatch()
		.Build()
	);

	if (!TBS::Scan(state))
		return false;

	gSymbolTable = (uint32_t*)(TBS::Pattern::Result)state["LNST"];

	return true;
}

int SDKKallsymsInit()
{
	KERNEL_TEXT = KallsymLookupName<size_t>("_text");

	if (!KERNEL_TEXT)
		return 1;

	if (!LookupNameInitialize())
		return 2;
	
	return 0;
}