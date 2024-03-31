#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <format>
#include <vector>
#include <optional>
#include <memory>
#include <guiddef.h>

// Allow GUID to be used as a key in a hash
namespace std
{
    template <>
    struct hash<GUID>
    {
        size_t operator()(const GUID& guid) const
        {
            const uint64_t *p = reinterpret_cast<const uint64_t*>(&guid);
            return hash<uint64_t>{}(p[0]) ^ hash<uint64_t>{}(p[1]);
        }
    };
}

namespace wfpk {
std::string ipToString(UINT32 ipAddress);
// Ensure we get an array of UINT8[16] - prevent decay to pointer
std::string ipToString(const UINT8 (&ipAddress)[16]);
std::string blobToString(const FWP_BYTE_BLOB &blob);
}

