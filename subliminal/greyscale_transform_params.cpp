#include "greyscale_transform_params.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/io.hpp>

namespace subliminal {

typedef boost::fusion::result_of::as_vector<
  greyscale_transform_params
>::type vector_type;

std::istream& operator>>(std::istream& i, greyscale_transform_params& p)
{
  vector_type v;
  if (i >> v) {
    p = greyscale_transform_params(v);
  }
  return i;
}

std::ostream& operator<<(std::ostream& o, greyscale_transform_params const& p)
{
  vector_type v(p);
  o << v;
  return o;
}

}

