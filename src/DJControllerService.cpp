#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    if (cache.contains(track.get_title())) {
        cache.get(track.get_title());
        return 1;
    }
    PointerWrapper<AudioTrack> cloneWrapper = track.clone();
    AudioTrack* clone = cloneWrapper.release();
    if (clone == nullptr) {
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -2;
    }
    clone->load();
    clone->analyze_beatgrid();
    PointerWrapper<AudioTrack> newTrack (clone);
    if (cache.put(std::move(newTrack))) { // transfer ownership
        return -1;  // eviction
    } else {
        return 0;
    }
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    return cache.get(track_title);
}
