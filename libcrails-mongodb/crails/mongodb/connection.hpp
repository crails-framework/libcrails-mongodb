#ifndef  CRAILS_MONGODB_CONNECTION_HPP
# define CRAILS_MONGODB_CONNECTION_HPP

# include <crails/utils/backtrace.hpp>
# include <crails/utils/timer.hpp>
# include <crails/datatree.hpp>
# include <mongocxx/database.hpp>
# include <bsoncxx/builder/stream/document.hpp>
# include "bson_oid.hpp"
# include "result.hpp"

namespace Crails
{
  namespace MongoDB
  {
    class Connection
    {
    public:
      Connection();
      virtual ~Connection();

      float time = 0.f;

      template<typename MODEL>
      void start_connection_for()
      {
        require_database(MODEL().get_database_name());
      }

      template<typename MODEL>
      void start_connection_for(const MODEL& model)
      {
        require_database(model.get_database_name());
      }

      template<typename MODEL>
      mongocxx::collection collection_for()
      {
        if (database == nullptr) throw boost_ext::runtime_error("MongoDB::Connection::collection_for called without a database.");
        return (*database)[MODEL::collection_name];
      }

      template<typename MODEL>
      unsigned long count(bsoncxx::document::view_or_value query = {})
      {
        Utils::TimeGuard timer(time);
        start_connection_for<MODEL>();
        return collection_for<MODEL>().count_documents(query);
      }

      template<typename MODEL_PTR>
      bool find_one(MODEL_PTR& model, bsoncxx::document::view_or_value query)
      {
        typedef typename MODEL_PTR::element_type MODEL;
        Utils::TimeGuard timer(time);
        start_connection_for<MODEL>();
        auto result = collection_for<MODEL>().find_one(query);
        if (result)
        {
          DataTree model_data;

          model_data.from_json(bsoncxx::to_json(*result));
          model = std::make_shared<MODEL>();
          model->set_id(model_data["_id"]["$oid"].defaults_to<std::string>(""));
          model->edit(model_data);
          return true;
	}
        return false;
      }

      template<typename MODEL_PTR>
      bool find_one(MODEL_PTR& model, const std::string& id)
      {
        if (Oid::is_valid(id))
        {
          Utils::TimeGuard timer(time);
          bsoncxx::oid oid(id);

          return find_one(model, bsoncxx::builder::stream::document{}
            << "_id" << oid
            << bsoncxx::builder::stream::finalize);
        }
        return false;
      }

      template<typename MODEL>
      std::shared_ptr<MODEL> last_entry()
      {
        Utils::TimeGuard timer(time);
        start_connection_for<MODEL>();
        std::shared_ptr<MODEL> model;
        auto order = bsoncxx::builder::stream::document{} << "$natural" << -1 << bsoncxx::builder::stream::finalize;
        auto options = mongocxx::options::find{};
        options.sort(order.view());
        auto result = collection_for<MODEL>().find_one({}, options);

        if (result)
        {
          model = std::make_shared<MODEL>();
          model->edit(bson_to_data(*result));
        }
        return model;
      }

      template<typename MODEL>
      bool find(Result<MODEL>& result, bsoncxx::document::view_or_value query = {}, mongocxx::options::find options = {})
      {
        Utils::TimeGuard timer(time);

        start_connection_for<MODEL>();
        result = Result(collection_for<MODEL>().find(query, options));
        return result.begin() != result.end();
      }

      template<typename MODEL>
      void save(MODEL& model)
      {
        if (Oid::is_valid(model.get_id()))
          update_one(model);
        else
          insert_one(model);
      }

      template<typename MODEL>
      bool update_one(MODEL& model)
      {
        bsoncxx::document::view_or_value query = bsoncxx::from_json(model.to_json());
        return update_one(query);
      }

      template<typename MODEL>
      bool update_one(MODEL& model, bsoncxx::document::view_or_value query)
      {
        Utils::TimeGuard timer(time);
        bsoncxx::oid oid(model.get_id());
        start_connection_for<MODEL>();
        auto result = collection_for<MODEL>().update_one(
          bsoncxx::builder::stream::document{} << "_id" << oid << bsoncxx::builder::stream::finalize,
          query
        );
        return result;
      }

      template<typename MODEL>
      bool insert_one(MODEL& model)
      {
        bsoncxx::document::view_or_value query = bsoncxx::from_json(model.to_json());
        return insert_one(query);
      }

      template<typename MODEL>
      bool insert_one(MODEL& model, bsoncxx::document::view_or_value query)
      {
        Utils::TimeGuard timer(time);
        start_connection_for<MODEL>();
        auto result = collection_for<MODEL>().insert_one(query);

        if (result)
        {
          bsoncxx::oid oid = result->inserted_id().get_oid().value;
          model.set_id(oid.to_string());
        }
        return false;
      }

      template<typename MODEL>
      void destroy(MODEL& model)
      {
        if (Oid::is_valid(model.get_id()))
        {
          Utils::TimeGuard timer(time);
          start_connection_for<MODEL>();
          bsoncxx::oid oid(model.get_id());
          auto result = collection_for<MODEL>().delete_one(
            bsoncxx::builder::stream::document{} << "_id" << oid << bsoncxx::builder::stream::finalize
          );
          model.after_destroy();
        }
      }

      template<typename MODEL>
      void destroy(bsoncxx::document::view_or_value query = {})
      {
        Utils::TimeGuard timer(time);
        start_connection_for<MODEL>();
        collection_for<MODEL>().delete_many(query);
      }

    protected:
      void require_database(const std::string&);

      std::string database_name;
      mongocxx::database* database = nullptr;
    };
  }
}

#endif
