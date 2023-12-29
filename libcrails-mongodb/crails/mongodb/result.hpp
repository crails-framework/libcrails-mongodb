#ifndef  CRAILS_MONGODB_RESULT_HPP
# define CRAILS_MONGODB_RESULT_HPP

# include <mongocxx/cursor.hpp>
# include <bsoncxx/json.hpp>
# include <crails/datatree.hpp>
# include <memory>
# include <optional>

namespace Crails
{
  namespace MongoDB
  {
    struct ResultBase
    {
      ResultBase()
      {
      }

      ResultBase(mongocxx::cursor c) : cursor(std::move(c))
      {
      }

      ResultBase(ResultBase&& base) : cursor(std::move(base.cursor))
      {
      }

      ResultBase& operator=(mongocxx::cursor c)
      {
        cursor = std::move(c);
        return *this;
      }

      ResultBase& operator=(ResultBase&& base)
      {
        if (base.valid())
          cursor = std::move(base.cursor);
        return *this;
      }

      bool valid() const
      {
        return cursor.has_value();
      }

      class iterator
      {
      protected:
        mongocxx::cursor::iterator source;

        template<typename MODEL>
        void load_into(MODEL& model) const
        {
          DataTree data;

          data.from_json(json());
          model.set_id(data["_id"]["$oid"].as<std::string>());
          model.edit(data);
        }
      public:
        iterator(mongocxx::cursor::iterator it) : source(it)
        {
        }

        bool operator!=(const iterator& it) const { return it.source != source; }
        bool operator==(const iterator& it) const { return it.source == source; }

        iterator& operator++()
        {
          source++;
          return *this;
        }

        std::string json() const
        {
          return bsoncxx::to_json(*source);
        }

        template<typename MODEL>
        MODEL value() const
        {
          MODEL model;

          load_into(model);
          return model;
        }
      };

      iterator begin() { return cursor->begin(); }
      iterator end() { return cursor->end(); }

      std::optional<mongocxx::cursor> cursor;
    };

    template<typename MODEL>
    struct Result : public ResultBase
    {
      Result(mongocxx::cursor c) : ResultBase(std::move(c))
      {
      }

      Result(Result&& base) : ResultBase(std::move(base))
      {
      }

      Result()
      {
      }

      Result& operator=(Result&& base)
      {
        if (valid())
          cursor = std::move(base.cursor);
        return *this;
      }

      class iterator : public ResultBase::iterator
      {
        void load_into(MODEL& model) const
        {
          DataTree data;

          data.from_json(json());
          model.set_id(data["_id"]["$oid"].as<std::string>());
          model.edit(data);
        }
      public:
        iterator(mongocxx::cursor::iterator it) : ResultBase::iterator(it)
        {
        }

        MODEL value() const
        {
          return ResultBase::iterator::value<MODEL>();
        }

        std::unique_ptr<MODEL> operator*() const
        {
          auto model = std::make_unique<MODEL>();

          load_into(*model);
          return std::move(model);
        }
      };

      iterator begin() { return cursor->begin(); }
      iterator end() { return cursor->end(); }

      std::vector<MODEL> to_vector()
      {
        std::vector<MODEL> result;
        auto last = end();

        for (auto it = begin() ; it != last ; ++it)
          result.push_back(it.value());
        return result;
      }

      std::vector<std::unique_ptr<MODEL>> to_unique_vector()
      {
        std::vector<std::unique_ptr<MODEL>> result;

        pour_into_container(result);
        return result;
      }

      std::vector<std::shared_ptr<MODEL>> to_shared_vector()
      {
        std::vector<std::shared_ptr<MODEL>> result;

        pour_into_container(result);
        return result;
      }

      template<typename CONTAINER_TYPE>
      void pour_into_container(CONTAINER_TYPE& container)
      {
        auto last = end();

        for (auto it = begin() ; it != last ; ++it)
          container.push_back(std::move(*it));
      }
    };
  }
}

#endif
