#pragma once

#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <fwpmu.h>

namespace wfpk {
class WfpError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class Engine
{
public:
    Engine();
    ~Engine();

public:
    auto handle() -> const HANDLE& { return _handle; }
    operator HANDLE() { return handle(); }

private:
    HANDLE _handle{};
};

class WfpContext
{
public:
    WfpContext()
    {}

public:
    bool process();

private:
    Engine _engine;
};

class FilterEnum
{
    enum { MaxFilterCount = 1024 };
public:
    FilterEnum(FWPM_FILTER_ENUM_TEMPLATE enumTemplate, HANDLE engineHandle);
    ~FilterEnum();

public:
    template <typename IterFuncT>
    void forEach(IterFuncT func) const
    {
        for(size_t i = 0; i < _numEntries; ++i)
        {
            func(*_filters[i]);
        }
    }

private:
    UINT32 _numEntries{0};
    FWPM_FILTER** _filters{NULL};
    HANDLE _engineHandle{};
    HANDLE _enumHandle{};
};
}
