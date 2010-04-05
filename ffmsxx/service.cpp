#include <ffmsxx/service.hpp>

#include <cassert>

#include <ffms.h>

namespace ffmsxx {

static bool extant;

service::service()
{
  /** \bug On Windows, need to initialise COM. */
  /** \bug Not thread-safe */
  assert(!extant);
  extant = true;
  /** \todo CPU flags? */
  FFMS_Init(0);
}

service::~service()
{
  /** \bug On Windows, need to uninitialise COM. */
}

indexer service::create_indexer(boost::filesystem::path const& f) const
{
  return indexer(f);
}

}

