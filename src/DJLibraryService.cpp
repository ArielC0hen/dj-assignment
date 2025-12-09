#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    //: playlist(playlist) {}
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */

DJLibraryService::~DJLibraryService() {
    for (auto track: library) {
        delete track;
    }
    library.clear();
}

void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method
    for (const auto& info: library_tracks) {
        AudioTrack* track = nullptr;
        if (info.type == "MP3") {
            track = new MP3Track(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2);
            std::cout << "MP3Track created: " << info.extra_param1 << "kbps\n";
        } else if (info.type == "WAV") {
            track = new WAVTrack(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2);   
            std::cout << "WAVTrack created: " << info.extra_param1 << "Hz\n";
        } else {
            std::cout << "[Error]: Unrecognized track type.\n";
            continue;
        }
        library.push_back(track);
    }
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds\n" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    // For now, add a placeholder to fix the linker error
    std::cout << "[INFO] Loading playlist: " << playlist_name << "\n" << std::endl;
    playlist = Playlist(playlist_name);
    for (const auto& i : track_indices) {
        if (i > library.size()) {
            std::cout << "[WARNING] Invalid track index: " << i << "\n" << std::endl;
            continue;            
        }
        AudioTrack* track = library[i-1];
        PointerWrapper<AudioTrack> trackWrapper = track->clone();
        if (!trackWrapper) {
            std::cout << "[ERROR] Failed to clone: " << track->get_title() << "\n"<<std::endl;
            continue;
        }
        trackWrapper->load();
        trackWrapper->analyze_beatgrid();
        playlist.add_track(trackWrapper.release());
        //std::cout << "Added " << track->get_title() << " to playlist "<< playlist.get_name() << "\n"<<std::endl;        
    }    
    //(void)playlist_name;  // Suppress unused parameter warning
    //(void)track_indices;  // Suppress unused parameter warning
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    std::vector<std::string> trackNames;
    for (const auto& track : playlist.getTracks()) {
        trackNames.push_back(track->get_title());
    }    
    return trackNames;
}
