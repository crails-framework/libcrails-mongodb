#include "bson_oid.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <limits.h>
#include <regex>

using namespace std;
using namespace Crails::MongoDB;

Oid::Oid()
{
  static thread_local unsigned short counter = 1;
  auto now = std::chrono::system_clock::now();
  boost::mt19937 rng;
  boost::uniform_int<> random_values(0, INT_MAX);
  boost::variate_generator<boost::mt19937, boost::uniform_int<> > die(rng, random_values);

  timestamp   = std::chrono::system_clock::to_time_t(now);
  randomValue = die();
  index       = counter++;
}

string Oid::to_string() const
{
  stringstream stream;

  stream << std::setfill('0') << std::setw(8)  << std::hex << timestamp;
  stream << std::setfill('0') << std::setw(10) << std::hex << randomValue;
  stream << std::setfill('0') << std::setw(6)  << std::hex << index;
  return stream.str();
}

bool Oid::is_valid(const string& uid)
{
  static const regex r("^[0-9a-f]{24}$", regex_constants::optimize);

  return sregex_iterator(uid.begin(), uid.end(), r) != sregex_iterator();
}

string Oid::shortened(const string& source)
{
  string result;

  if (is_valid(source))
  {
    result += source.substr(8, 3);
    result += source.substr(21, 3);
  }
  return result;
}

