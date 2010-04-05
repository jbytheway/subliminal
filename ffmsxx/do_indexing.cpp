#include <ffmsxx/do_indexing.hpp>

#include <ffms.h>

#include <ffmsxx/indexer.hpp>
#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

namespace {

  int FFMS_CC progress_callback(
    int64_t /*Current*/, int64_t /*Total*/, void* /*ICPrivate*/
  )
  {
    return 0;
  }

}

index do_indexing(indexer&& i, track_mask const index_mask)
{
  detail::error_info e;
  FFMS_Index* raw_index = FFMS_DoIndexing(
    i.release(), index_mask.raw,
    0 /*dump mask*/, NULL /*audio name callback*/, NULL /*private data*/,
    false /*ignore decode errors*/, progress_callback,
    NULL /*private data*/, e.raw()
  );
  e.throw_if_null(raw_index);
  return index(raw_index);
}

}

