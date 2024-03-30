#include <format>
#include <windows.h>
#include <stdlib.h>
#include "wfp_killer.h"
#include "wfp_ostream_helpers.h"
#include "wfp_name_mapper.h"

namespace wfpk {

namespace
{
    const std::vector kPiaLayers = {
        FWPM_LAYER_ALE_AUTH_CONNECT_V4,
        FWPM_LAYER_ALE_AUTH_CONNECT_V6,
        FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
        FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
        FWPM_LAYER_ALE_BIND_REDIRECT_V4,
        FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4,
        FWPM_LAYER_ALE_CONNECT_REDIRECT_V4,
        FWPM_LAYER_INBOUND_IPPACKET_V4,
        FWPM_LAYER_OUTBOUND_IPPACKET_V4
    };
}

void WfpKiller::listFilters() const
{
    size_t filterCount{0};

    std::cout << "Showing all filters for PIA provider:\n";

    for(const auto &layerKey : kPiaLayers)
    {
        std::cout << std::format("\nLayer: {}\n", WfpNameMapper::getName(layerKey).rawName);
        _engine.enumerateFiltersForLayer(layerKey, [&](const auto &pFilter) {
            std::cout << *pFilter << std::endl;
            ++filterCount;
        });
    }

    std::cout << std::format("\nTotal number of filters: {}\n", filterCount);
}

void WfpKiller::deleteFilters(const std::vector<FilterId> &filterIds) const
{
    uint32_t deleteCount{0};

    if(filterIds.size() > 0)
    {
        std::cout << std::format("Will delete {} filters\n", filterIds.size());
        for(const auto &filterId : filterIds)
        {
            if(deleteSingleFilter(filterId))
                ++deleteCount;
        }
    }
    else
    {
        std::cout << "This action will delete ALL PIA filters\nAre you sure? (y/n)\n";
        char userDecision{};
        std::cin >> userDecision;
        if(userDecision == 'y')
        {
            _engine.enumerateFiltersForLayers(kPiaLayers, [&](const auto &pFilter) {
                if(deleteSingleFilter(pFilter->filterId))
                    ++deleteCount;
            });
        }
    }

    std::cout << std::format("Deleted {} filters.\n", deleteCount);
}

void CALLBACK foo(void *context, const FWPM_NET_EVENT *event)
{

/*
        uint64_t *ptr = reinterpret_cast<uint64_t*>(context);
        *ptr = 100;
        std::cout << "Some event occured" << std::endl;
        switch(event->type)
        {
        case FWPM_NET_EVENT_TYPE_CLASSIFY_DROP:
            std::cout << "Drop event" << std::endl;;
            break;
        case FWPM_NET_EVENT_TYPE_CLASSIFY_ALLOW:
            std::cout << "Allow event" << std::endl;;
            break;
        }
        */
}

void WfpKiller::monitor()
{
    _engine.monitorEvents([](void *context, const FWPM_NET_EVENT *event) {
        std::cout << *event << "\n";
        switch(event->type)
        {
        case FWPM_NET_EVENT_TYPE_CLASSIFY_DROP:
            std::cout << "drop event\n";
            break;
        case FWPM_NET_EVENT_TYPE_CLASSIFY_ALLOW:
            std::cout << "allow event\n";
            break;
        }
    });

    std::cin.get();

    std::cout << "Value of EventMonitor::context " << EventMonitor::context << std::endl;
}

bool WfpKiller::deleteSingleFilter(FilterId filterId) const
{
    DWORD result = _engine.deleteFilterById(filterId);
    if(result != ERROR_SUCCESS)
    {
        std::cerr << std::format("Error: Failed to delete filter with id {} Code: {}\n", filterId, result);
        return false;
    }
    else
    {
        std::cout << std::format("Successfully deleted filter with id {}.\n", filterId);
        return true;
    }
}
}