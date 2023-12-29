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
    template<typename MODEL>
    class Result
    {
      std::optional<mongocxx::cursor::iterator> _begin, _end;
    public:
      typedef MODEL value_type;

      Result() {}
      Result(mongocxx::cursor cursor) : _begin(cursor.begin()), _end(cursor.end()) {}
      Result(const Result& result) : _begin(result._begin), _end(result._end) {}

      std::vector<MODEL> to_vector()
      {
        std::vector<MODEL> list;

        for (auto it = begin() ; it != end() ; ++it)
          list.push_back(**it);
        return list;
      }

      std::vector<std::shared_ptr<MODEL>> to_shared_vector()
      {
        std::vector<std::shared_ptr<MODEL>> list;

        for (auto it = begin() ; it != end() ; ++it)
          list.push_back(std::move(*it));
        return list;
      }

      class iterator
      {
        std::optional<mongocxx::cursor::iterator> it;
      public:
        iterator(mongocxx::cursor::iterator it) : it(it) {}
        iterator() {}

        bool operator==(const iterator& other) const
        {
          return (!it.has_value() && !other.it.has_value())
              || (it.has_value() && other.it.has_value() && it.value() == other.it.value());
        }

        inline bool operator!=(const iterator& other) const
        {
          return !(*this == other);
        }

        inline iterator& operator++()
        {
          if (it.has_value())
            it.value()++;
          return *this;
        }

        iterator& operator++(int i)
        {
          if (i > 0 && it.has_value())
          {
            while (i--)
              it.value()++;
          }
          return *this;
        }

        std::unique_ptr<MODEL> operator*()
        {
          DataTree data;
          const auto& document = *(it.value());
          auto model = std::make_unique<MODEL>();

          data.from_json(bsoncxx::to_json(document));
          model->edit(data);
          return std::move(model);
        }
      };

      iterator begin() { return _begin.has_value() ? iterator(_begin.value()) : end(); }
      iterator end() { return _end.has_value() ? iterator(_end.value()) : iterator(); }
    };
  }
}

#endif
