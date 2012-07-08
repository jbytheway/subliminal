#include "find_best_offset_and_transform.hpp"

#include <boost/next_prior.hpp>

#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/video_source.hpp>

#include "fatal.hpp"
#include "nearby_frame_finder.hpp"
#include "find_best_greyscale_transform.hpp"

namespace subliminal {

frame_greyscale_transform find_best_offset_and_transform(
  ffmsxx::video_source const& raw,
  ffmsxx::video_source const& subs,
  boost::rational<int64_t>& sync_bottom,
  boost::rational<int64_t>& sync_top,
  int const alignment_frame,
  greyscale_transform_params const& start_params,
  visual_feedback& feedback
)
{
  auto const subs_time_base = subs.time_base();
  auto const raw_time_base = raw.time_base();

  nearby_frame_finder raw_finder{raw};

  auto subs_frame = subs.frame(alignment_frame);
  auto subs_time = subs_frame.time(subs_time_base);

  auto const& potential_raw_frames = raw_finder.get_frames(
    subs_time+sync_bottom, subs_time+sync_top
  );

  feedback.messagef(
    boost::format("searching for good transform through %d frames near %s")
      % potential_raw_frames.size() % subs_time
  );

  if (potential_raw_frames.empty()) {
    SUBLIMINAL_FATAL("videos seem to be of wildly different lengths");
  }

  boost::optional<frame_greyscale_transform> best_transform;
  double best_score = 1.0/0.0;
  auto best_it = potential_raw_frames.begin();
  for (auto it = best_it; it != potential_raw_frames.end(); ++it) {
    auto const& raw_frame = *it;
    auto const raw_time = raw_frame.time(raw_time_base);
    feedback.messagef(
      boost::format("testing raw frame at %s") % raw_time
    );
    boost::optional<frame_greyscale_transform> transform;
    double score;
    std::tie(transform, score) = find_best_greyscale_transform(
      raw_frame, subs_frame, start_params, feedback
    );
    if (score < best_score) {
      best_score = score;
      best_transform = transform;
      best_it = it;
    }
  }

  // We have the best transform, but at the same time we need to store the
  // bounds we get which will inform later sync guesses
  if (best_it != potential_raw_frames.begin()) {
    sync_bottom = boost::prior(best_it)->time(raw_time_base) - subs_time;
  }
  if (boost::next(best_it) != potential_raw_frames.end()) {
    sync_top = boost::next(best_it)->time(raw_time_base) - subs_time;
  }

  feedback.messagef(
    boost::format("best frame at %s scored %f.  Sync bounds (%s, %s)") %
      best_it->time(raw_time_base) % best_score % sync_bottom % sync_top
  );

  if (!best_transform) {
    SUBLIMINAL_FATAL("No candidate frames for determining correct transform");
  }
  return *best_transform;
}

}
