#include <ffmsxx/index.hpp>

#include <ffms.h>

namespace ffmsxx {

struct index::impl {
  ~impl() { FFMS_DestroyIndex(raw); }

  FFMS_Index* raw;
};

index::~index() = default;

index::index(FFMS_Index* raw) :
  impl_(new impl{raw})
{}

FFMS_Index* index::raw() const
{
  return impl_->raw;
}

}

