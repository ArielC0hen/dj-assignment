#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    //: active_deck(0)
    : decks(), active_deck(0), auto_sync(false), bpm_tolerance(0) // active_deck(1)
{
    // Your implementation here
    decks[0] = nullptr;
    decks[1] = nullptr;
    auto_sync = false;
    bpm_tolerance = 0;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks.\n";
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cout << "[MixingEngineService] Cleaning up decks....\n";
    for(int i = 0; i < 2; i++) {
        if (decks[i]) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
    std::cout << "=== Loading Track to Deck ===\n" << std::endl;
    PointerWrapper<AudioTrack> cloneWrapper = track.clone();
    if (!cloneWrapper) {
        std::cerr << "[ERROR] Track: " << track.get_title() << " failed to clone\n" << std::endl;
        return -1;
    }
    int target = 1 - active_deck;
    std::cerr << "[Deck Switch] Target deck: " << target << std::endl;
    if (decks[target]) {
        delete decks[target];
        decks[target] = nullptr;
    }
    cloneWrapper->load();
    cloneWrapper->analyze_beatgrid();
    if (auto_sync && decks[active_deck]) {
        if (!can_mix_tracks(cloneWrapper)) {
            sync_bpm(cloneWrapper);
        }
    }
    decks[target] = cloneWrapper.release();
    std::cerr << "[Load Complete] " << decks[target]->get_title() <<" is now loaded on deck " << target << "\n" << std::endl;
    if (decks[active_deck] && active_deck !=  target) { // && not first
        std::cerr << "[Unload] Unloading previous deck " << active_deck <<" (" << decks[active_deck]->get_title() << ")\n" << std::endl;  
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    active_deck = target;
    std::cerr << "[Active Deck] switched to deck " << target <<"\n" << std::endl;
    return target;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if (!decks[active_deck] || !track) {
        return false;
    }
    int thisBpm = decks[active_deck]->get_bpm();
    int otherBpm = track->get_bpm();
    if(abs(thisBpm - otherBpm) <= bpm_tolerance) {
        return true;
    }
    return false;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if (decks[active_deck] && track) {
        int thisBpm = decks[active_deck]->get_bpm();
        int otherBpm = track->get_bpm();
        track->set_bpm((thisBpm + otherBpm) / 2);
        std::cerr << "[Sync BPM] Syncing BPM from " << decks[active_deck]->get_title()<< " to "<< track->get_title() <<"\n" << std::endl;
    }
}
