#include "connection.hpp"
#include "database.hpp"
#include <crails/databases.hpp>

using namespace Crails::MongoDB;

Crails::MongoDB::Connection::Connection()
{
}

Crails::MongoDB::Connection::~Connection()
{
}

void Crails::MongoDB::Connection::require_database(const std::string_view name)
{
  static const std::string default_configuration_name = "mongodb";

  if (name != database_name)
  {
    if (name == "default")
      database = &(CRAILS_DATABASE(MongoDB, default_configuration_name).get_database());
    else
      database = &(CRAILS_DATABASE(MongoDB, std::string(name.data(), name.length())).get_database());
    database_name = name;
  }
}



