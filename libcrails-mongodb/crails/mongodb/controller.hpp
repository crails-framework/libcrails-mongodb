#ifndef  CRAILS_MONGODB_CONTROLLER_HPP
# define CRAILS_MONGODB_CONTROLLER_HPP

# include <crails/controller.hpp>
# include "connection.hpp"

namespace Crails
{
  namespace MongoDB
  {
    template<typename SUPER = Crails::Controller, class DATABASE = MongoDB::Connection>
    class Controller : public SUPER
    {
    protected:
      DATABASE database;

    public:
      Controller(Context& context) : SUPER(context)
      {
      }

      void finalize() override
      {
        SUPER::finalize();
        SUPER::params["response-time"]["mongodb"] = database.time;
      }
    };
  }
}

#endif
