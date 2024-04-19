#pragma once

#include <cstdint>
#include <Kernel/Decls.h>

extern size_t KERNEL_TEXT;

REGPARAMDECL(void*) kallsyms_lookup_name(const char* name);

int KAllsymSymbolIndexLookupByEntry(uintptr_t entry);
size_t KAllsymSymbolSizeGet(uintptr_t symEntry, size_t defSize = 0x0);
size_t KAllsymSymbolSizeGet(const char* symbolName, size_t defSize = 0x0);

template<typename T = void*>
T KallsymLookupName(const char* name)
{
	return (T)kallsyms_lookup_name(name);
}