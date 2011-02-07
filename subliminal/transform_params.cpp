#include "transform_params.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/io.hpp>

namespace subliminal {

typedef boost::fusion::result_of::as_vector<
  transform_params
>::type vector_type;

std::istream& operator>>(std::istream& i, transform_params& p)
{
  vector_type v;
  if (i >> v) {
    p = transform_params(v);
  }
  return i;
}

std::ostream& operator<<(std::ostream& o, transform_params const& p)
{
  vector_type v(p);
  o << v;
  return o;
}

}

