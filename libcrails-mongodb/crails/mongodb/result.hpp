#ifndef  CRAILS_MONGODB_RESULT_HPP
# define CRAILS_MONGODB_RESULT_HPP

# include <mongocxx/cursor.hpp>
# include <bsoncxx/json.hpp>
# include <crails/datatree.hpp>
# include <memory>

namespace Crails
{
  namespace MongoDB
  {
    template<typename MODEL>
    class Result
    {
      mongocxx::cursor::iterator _begin, _end;
    public:
      typedef MODEL value_type;

      Result() {}
      Result(mongocxx::cursor& cursor) : _begin(cursor.begin()), _end(cursor.end()) {}
      Result(const Result& result) : _begin(result._begin), _end(result._end) {}

      std::vector<std::shared_ptr<MODEL>> to_vector()
      {
        std::vector<std::shared_ptr<MODEL>> list;

        for (auto it = begin() ; it != end() ; ++it)
          list.push_back(*it);
        return list;
      }

      class iterator
      {
        mongocxx::cursor::iterator it;
      public:
        iterator(mongocxx::cursor::iterator it) : it(it) {}

        iterator& operator++(int i)
        {
          if (i > 0)
          {
            while (i--)
              it++;
          }
          return *this;
        }

        std::shared_ptr<MODEL> operator*()
        {
          DataTree data;
          const auto& document = *it;
          auto model = std::make_shared<MODEL>();

          data.from_json(bsoncxx::to_json(document));
          model->edit(data);
          return model;
        }
      };

      iterator begin() { return iterator(_begin); }
      iterator end() { return iterator(_end); }
    };
  }
}

#endif
