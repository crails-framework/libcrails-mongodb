#ifndef  CRAILS_MONGODB_DATABASE_HPP
# define CRAILS_MONGODB_DATABASE_HPP

# include <crails/databases.hpp>
# include <mongocxx/instance.hpp>
# include <mongocxx/database.hpp>
# include <mongocxx/client.hpp>

namespace Crails
{
  namespace MongoDB
  {
    class Database : public Crails::Databases::Database
    {
    public:
      Database(const Crails::Databases::DatabaseSettings& settings);

      static const std::string ClassType() { return "mongodb"; }

      void connect() override;

      mongocxx::database& get_database()
      {
        return database;
      }

    private:
      static mongocxx::instance driver;
      std::string        database_name;
      mongocxx::uri      database_uri;
      mongocxx::client   client;
      mongocxx::database database;
    };
  }
}

#endif
