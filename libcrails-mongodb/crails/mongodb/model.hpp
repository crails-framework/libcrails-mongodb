#pragma once
#include <crails/datatree.hpp>
#include <string_view>

namespace Crails
{
  namespace MongoDB
  {
    struct ModelInterface
    {
      virtual std::string_view get_database_name() const { return "mongo"; }
      virtual std::string get_id() const = 0;
      virtual void set_id(const std::string&) = 0;
      virtual std::string to_json() const = 0;
      virtual void edit(Data) = 0;
      virtual void after_destroy() {}
      bool is_persistent() const { return get_id().length() > 0; }
    };
  }
}
