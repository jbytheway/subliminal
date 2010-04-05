#ifndef FFMSXX__SERVICE_HPP
#define FFMSXX__SERVICE_HPP

#include <boost/filesystem/path.hpp>

#include <ffmsxx/indexer.hpp>

namespace ffmsxx {

class service : boost::noncopyable {
  public:
    service();
    ~service();

    indexer create_indexer(boost::filesystem::path const&) const;
};

}

#endif // FFMSXX__SERVICE_HPP

