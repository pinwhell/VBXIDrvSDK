#include <Kernel/KAllsyms.h>
#include <Kernel/Decls.h>
#include <etl/unordered_map.h>
#include <etl/string.h>
#include <BidirectionalMap.h>
#include <Kernel/Syms.h>

#define KSYM_NAME_LEN 128
#define KSOL_SYMBOL_COUNT_EDGE 105000

REGPARAMDECL(unsigned long) kallsyms_sym_address(int idx);

BidirectionalMap<int, uintptr_t, 4096> gOffsetEntryCache;

int KAllsymSymbolIndexLookupByEntry(uintptr_t entry) {

	if (gOffsetEntryCache.containsValue(entry))
		return gOffsetEntryCache.getKey(entry);

	// Perform binary search on sorted symbol addresses
	int low = 0;
	int high = KSOL_SYMBOL_COUNT_EDGE - 1;

	while (low <= high) {
		int mid = low + (high - low) / 2;
		uintptr_t currSym = kallsyms_sym_address(mid);

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

	auto result = kallsyms_sym_address(nextSymIndex) - symEntry;

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

int SDKKallsymsInit()
{return 0;}