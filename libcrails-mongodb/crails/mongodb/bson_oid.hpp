#ifndef  CRAILS_BSONID_H
# define CRAILS_BSONID_H

# include <string>

namespace Crails
{
  namespace MongoDB
  {
    class Oid
    {
    public:
      Oid();

      std::string to_string() const;

      static bool is_valid(const std::string& uid);
      static std::string shortened(const std::string&);

    private:
      long                  timestamp;
      unsigned long long    randomValue;
      unsigned short        index;
    };
  }
}


#endif

