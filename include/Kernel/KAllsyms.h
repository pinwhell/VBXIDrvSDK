#pragma once

#include <cstdint>

int KAllsymSymbolIndexLookupByEntry(uintptr_t entry);
size_t KAllsymSymbolSizeGet(uintptr_t symEntry, size_t defSize = 0x0);
size_t KAllsymSymbolSizeGet(const char* symbolName, size_t defSize = 0x0);