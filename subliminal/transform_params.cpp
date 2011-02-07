#include "transform_params.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/io.hpp>

namespace subliminal {

std::istream& operator>>(std::istream& i, transform_params& p)
{
  boost::fusion::vector<double, double, double, double, double, double> v;
  if (i >> v) {
    p = transform_params(v);
  }
  return i;
}

std::ostream& operator<<(std::ostream& o, transform_params const& p)
{
  boost::fusion::vector<double, double, double, double, double, double> v(p);
  o << v;
  return o;
}

}

