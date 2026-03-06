#include "tsVector.hpp"
#include <string>
#include <functional>
#include <cstring>
#include <vector>
template <typename T>
using vec = threadsafe::vec<T>;
/**
 * @attention this class wraps over threadsafe vector 
 * 
 * 
 * 
 * 
 */

namespace threadsafe{

    template <typename key, typename value>
    class hashmap{
        friend class vec<T>;
        friend class std::vector;
        private:
        struct _set{
            std::string _hashedKey;
            std::vector<value> _valueArr;
        };

        vec<_set> _map;
        std::size_t _bucketCount;
        std::size_t _keySize;
        std::size_t _valueSize;

        template <typename T>
        std::size_t hashBytes(const T& value){
                static_assert(std::is_trivially_copyable_v<T> "T type (key) must be a parsable type");
                std::string_view bytes (reinterpret_cast<char*>(&value), sizeof(T));
            return std::hash<std::string_view>{} value;
        }

        public:

        T at(std::size_t index){

        }

        void clear(){

        }

        bool find(T key){

        }

        bool isEmpty(){

        }

        void insert(key k, value v){
            std::size_t 
        }


        std::size_t startInd(){

        }

        void swap(){

        }

    };











}