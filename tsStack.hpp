#include "tsVector.hpp"

template <typename T>
using vec = threadsafe::vec<T>;


namespace threadsafe{

    template <typename T>
    class stack{
        friend class vec<T>;
        private:
        vec<T> _stack;
        







        public:
        void pop(){

        }

        void push(T value){

        }

        T front(){

        }


    };











}