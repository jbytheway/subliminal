#include <ffmsxx/indexer.hpp>

#include <ffms.h>

#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

struct indexer::impl {
 ~impl() { FFMS_CancelIndexing(raw); }
  FFMS_Indexer* raw;
};

indexer::~indexer() = default;

int indexer::num_tracks() const {
  return FFMS_GetNumTracksI(impl_->raw);
}

track_info indexer::info(int track) const {
  return track_info(
    track_type_from_raw(FFMS_GetTrackTypeI(impl_->raw, track))
  );
}

indexer::indexer(boost::filesystem::path const& f)
{
  detail::error_info e;
  auto raw_indexer = FFMS_CreateIndexer(f.c_str(), e.raw());
  e.throw_if_null(raw_indexer);
  impl_.reset(new impl{raw_indexer});
}

FFMS_Indexer* indexer::release()
{
  FFMS_Indexer* result = impl_->raw;
  impl_->raw = NULL;
  impl_.reset();
  return result;
}

}

