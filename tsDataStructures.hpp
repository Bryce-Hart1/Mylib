#pragma once
#include <mutex>



namespace threadsafe{
    template <typename T>
    /* @class vec
    * threadsafe vector implementation
    * 
    *
    *
    */
    class vec{
        T* _Data;
        size_t _Size;
        size_t _Capacity;

        vec(){
            
        }
        /*passing intial allows for vec size to be set to this*/
        vec(size_t intial){

        }

    };

}