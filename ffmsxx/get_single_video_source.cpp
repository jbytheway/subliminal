#include <ffmsxx/get_single_video_source.hpp>

#include <ffmsxx/service.hpp>
#include <ffmsxx/track_mask.hpp>
#include <ffmsxx/do_indexing.hpp>
#include <ffmsxx/error.hpp>
#include <ffmsxx/seek.hpp>

namespace ffmsxx {

video_source get_single_video_source(
  service const& s,
  boost::filesystem::path const& file
) {
  auto indexer = s.create_indexer(file);
  int numTracks = indexer.num_tracks();
  int videoTrack = -1;
  for (int track = 0; track < numTracks; ++track) {
    auto info = indexer.info(track);
    if (info.is_video()) {
      if (videoTrack == -1) {
        videoTrack = track;
      } else {
        throw error("multiple video tracks");
      }
    }
  }
  if (videoTrack == -1) {
    throw error("no video track");
  }
  auto index = do_indexing(std::move(indexer), no_tracks);
  return video_source(file, index, videoTrack, seek::normal);
}

}

