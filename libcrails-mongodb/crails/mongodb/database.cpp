#include "database.hpp"
#include <crails/any_cast.hpp>

using namespace Crails::MongoDB;

mongocxx::instance Database::driver;

Database::Database(const Crails::Databases::DatabaseSettings& settings) : Crails::Databases::Database(ClassType())
{
  database_name = defaults_to<const char*>(settings, "database", "crails");
  database_uri = mongocxx::uri(defaults_to<const char*>(settings, "url", "mongodb://localhost:27017"));
}

void Database::connect()
{
  client = mongocxx::client(database_uri);
  database = client[database_name];
}
