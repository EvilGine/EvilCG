#include <help/ecg_hasher.h>
#include <help/ecg_memory.h>

namespace ecg {
    size_t ecg_hasher::operator()(const ecg_mem_handler& handler) const {
        return std::hash<uint64_t>()(handler.get_descriptor());
    }

    bool ecg_equal::operator()(const ecg_mem_handler& lhs, const ecg_mem_handler& rhs) const {
        return lhs.get_descriptor() == rhs.get_descriptor();
    }
}