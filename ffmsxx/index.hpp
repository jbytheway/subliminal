#ifndef FFMSXX__INDEX_HPP
#define FFMSXX__INDEX_HPP

#include <memory>

#include <boost/noncopyable.hpp>

#include <ffmsxx/fwd.hpp>
#include <ffmsxx/track_mask.hpp>

struct FFMS_Index;

namespace ffmsxx {

class index : boost::noncopyable {
  public:
    index(index&&);
    ~index();
  private:
    friend class video_source;
    friend index do_indexing(indexer&& i, track_mask const index_mask);

    index(FFMS_Index*);
    FFMS_Index* raw() const;

    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__INDEX_HPP

