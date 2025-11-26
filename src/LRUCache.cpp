#include "LRUCache.h"
#include <iostream>

LRUCache::LRUCache(size_t capacity)
    : slots(capacity), max_size(capacity), access_counter(0) {}

bool LRUCache::contains(const std::string& track_id) const {
    return findSlot(track_id) != max_size;
}

AudioTrack* LRUCache::get(const std::string& track_id) {
    size_t idx = findSlot(track_id);
    if (idx == max_size) return nullptr;
    return slots[idx].access(++access_counter);
}

/**
 * TODO: Implement the put() method for LRUCache
 */
bool LRUCache::put(PointerWrapper<AudioTrack> track) {
    if (!track) {
        return false;
    }
    bool isFull = true;
    for (CacheSlot& c : slots) {
        if (c.isOccupied()) {
            AudioTrack* t = c.getTrack();
            if (t && t->get_title() == track->get_title()) {
                std::cout << "=========================CASE 4=========================" << std::endl;
                access_counter++;
                c.access(access_counter);
                return false;
            }
        } else {
            isFull = false;
        }
    }
    bool evict = false;
    if (isFull) {
        evict = evictLRU();
    }
    size_t emptySlotIndex = findEmptySlot();
    access_counter++;
    slots[emptySlotIndex].store(std::move(track), access_counter); // store sets occupied to true
    return evict;
}

bool LRUCache::evictLRU() {
    size_t lru = findLRUSlot();
    if (lru == max_size || !slots[lru].isOccupied()) return false;
    slots[lru].clear();
    return true;
}

size_t LRUCache::size() const {
    size_t count = 0;
    for (const auto& slot : slots) if (slot.isOccupied()) ++count;
    return count;
}

void LRUCache::clear() {
    for (auto& slot : slots) {
        slot.clear();
    }
}

void LRUCache::displayStatus() const {
    std::cout << "[LRUCache] Status: " << size() << "/" << max_size << " slots used\n";
    for (size_t i = 0; i < max_size; ++i) {
        if(slots[i].isOccupied()){
            std::cout << "  Slot " << i << ": " << slots[i].getTrack()->get_title()
                      << " (last access: " << slots[i].getLastAccessTime() << ")\n";
        } else {
            std::cout << "  Slot " << i << ": [EMPTY]\n";
        }
    }
}

size_t LRUCache::findSlot(const std::string& track_id) const {
    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied() && slots[i].getTrack()->get_title() == track_id) return i;
    }
    return max_size;

}

/**
 * TODO: Implement the findLRUSlot() method for LRUCache
 */
size_t LRUCache::findLRUSlot() const {
    uint64_t lastAccess = UINT64_MAX;
    size_t lruIndex = max_size;
    for (size_t i = 0; i < slots.size(); i++) {
        const CacheSlot& c = slots[i];
        if(c.isOccupied()) {
            if (c.getLastAccessTime() < lastAccess) {
                lruIndex = i;
                lastAccess = c.getLastAccessTime();
            }
        }
    }
    return lruIndex;
}

size_t LRUCache::findEmptySlot() const {
    for (size_t i = 0; i < max_size; ++i) {
        if (!slots[i].isOccupied()) return i;
    }
    return max_size;
}

void LRUCache::set_capacity(size_t capacity){
    if (max_size == capacity)
        return;
    //udpate max size
    max_size = capacity;
    //update the slots vector
    slots.resize(capacity);
}