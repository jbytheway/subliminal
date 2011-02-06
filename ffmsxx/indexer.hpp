#ifndef FFMSXX__INDEXER_HPP
#define FFMSXX__INDEXER_HPP

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include <ffmsxx/do_indexing.hpp>
#include <ffmsxx/track_info.hpp>

class FFMS_Indexer;

namespace ffmsxx {

class indexer : boost::noncopyable {
  public:
    indexer(indexer&&);
    ~indexer();

    int num_tracks() const;
    track_info info(int track) const;
  private:
    friend class service;
    friend index do_indexing(indexer&&, track_mask const);

    indexer(boost::filesystem::path const& f);

    FFMS_Indexer* release();

    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__INDEXER_HPP

