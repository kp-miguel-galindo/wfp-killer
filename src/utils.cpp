#include <winsock2.h>
#include <ws2tcpip.h>
#include <fwpmu.h>
#include "utils.h"

namespace wfpk {
std::string ipToString(UINT32 ipAddress)
{
    char str[INET_ADDRSTRLEN]{};
    InetNtopA(AF_INET, &ipAddress, str, INET_ADDRSTRLEN);

    return std::string{str};
}

std::string ipToString(const UINT8 (&ipAddress)[16])
{
    char str[INET6_ADDRSTRLEN]{};
    InetNtopA(AF_INET6, ipAddress, str, INET6_ADDRSTRLEN);

    return str;
}

std::string blobToString(const FWP_BYTE_BLOB &blob)
{
    UINT8* data = blob.data;
    size_t numChars = blob.size / sizeof(wchar_t) - 1;

    std::wstring wstr(reinterpret_cast<const wchar_t*>(data), numChars);

    return wideStringToString(wstr);
}

std::string guidToString(const GUID& guid) {
    constexpr size_t strSize = 64;

    wchar_t wszGuid[strSize] = {};
    StringFromGUID2(guid, wszGuid, strSize);
    std::wstring result(wszGuid);

    // Convert the std::wstring to a std::string
    return wideStringToString(result);
}

std::string wideStringToString(const std::wstring &wstr)
{
    std::string str;

    // hack to convert wide strings to strings
    std::transform(wstr.begin(), wstr.end(), std::back_inserter(str), [] (wchar_t c) {
        return static_cast<char>(c);
    });

    return str;
}
}
