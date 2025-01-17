#include <winsock2.h>
#include <ws2tcpip.h>
#include <fwpmu.h>
#include <utils.h>

namespace wfpk {
auto splitString(const std::string &str, char delim) -> std::vector<std::string>
{
    auto parts = str | std::ranges::views::split(delim);
    std::vector<std::string> splitVec;

    for(auto &&part : parts)
        splitVec.emplace_back(part.begin(), part.end());

    return splitVec;
}

// Ipv4
std::string ipToString(UINT32 ipAddress)
{
    char str[INET_ADDRSTRLEN]{};
    InetNtopA(AF_INET, &ipAddress, str, INET_ADDRSTRLEN);

    return std::string{str};
}

// Ipv6
std::string ipToString(const UINT8 (&ipAddress)[16])
{
    char str[INET6_ADDRSTRLEN]{};
    InetNtopA(AF_INET6, ipAddress, str, INET6_ADDRSTRLEN);

    return str;
}

bool isIpv4(const std::string &ipAddress)
{
    struct in_addr ipv4Addr;
    return InetPtonA(AF_INET, ipAddress.c_str(), &ipv4Addr) == 1;
}

bool isIpv6(const std::string &ipAddress)
{
    struct in6_addr ipv6Addr;
    return InetPtonA(AF_INET6, ipAddress.c_str(), &ipv6Addr) == 1;
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
    std::ranges::transform(wstr, std::back_inserter(str), [] (wchar_t c) {
        return static_cast<char>(c);
    });

    return str;
}

std::string toLowercase(const std::string &str)
{
    std::string ret;
    ret.reserve(str.size());
    for(unsigned char c : str)
        ret.push_back(std::tolower(c));

        return ret;
}

std::string getErrorString(DWORD errorCode)
{
    LPSTR errMsg = nullptr;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    DWORD messageLength = FormatMessageA(
        flags,                 // Use system message tables to retrieve error text
        nullptr,               // No module handle is necessary
        errorCode,             // Pass in the error code to look up
        0,                     // Automatically choose the correct language
        (LPSTR)&errMsg,        // Buffer that will receive the error message
        0,                     // Minimum number of characters to allocate for errMsg
        nullptr                // No varargs for additional error-specific inserts
    );

    if(messageLength == 0)
    {
        // Handle the case where there is no error message found
        return "Unknown error code: " + std::to_string(errorCode);
    }

    std::string errorMessage(errMsg);

    // Free the buffer allocated by FormatMessage
    LocalFree(errMsg);

    return errorMessage;
}

}
