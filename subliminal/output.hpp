#ifndef SUBLIMINAL__OUTPUT_HPP
#define SUBLIMINAL__OUTPUT_HPP

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/rational.hpp>
#include <boost/gil/typedefs.hpp>

namespace subliminal {

class output : boost::noncopyable {
  public:
    output(boost::filesystem::path const& root);
    void save(
      boost::rational<int64_t> const& begin_time,
      boost::rational<int64_t> const& end_time,
      std::vector<boost::gil::gray8_image_t> const& images,
      std::string const& other
    );
  private:
    boost::filesystem::path root_;
    boost::filesystem::ofstream txt_out_;
    std::uint64_t index_;
};

}

#endif // SUBLIMINAL__OUTPUT_HPP

