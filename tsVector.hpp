#pragma once
#include <mutex>
#include <thread>
#include <iostream>
#include <random>
#include <cstddef>
#include "exceptions.hpp"

/** @attention complete vector implemenation
 * 
 * 
 * 
 */


namespace threadsafe{
    template <typename T>
    /** @class vec
    * threadsafe vector implementation
    * 
    *
    *
    */
    class vec{
        private:
        T* v_Data;
        std::size_t v_Size; //also our end iterator
        std::size_t v_Capacity;
        std::size_t v_StartIndex;
        std::mutex v_Mutex;
        
        //checks if index is in range
        bool checkIndex(size_t index){
            if(index >= v_StartIndex && index <= v_Size){
                return true;
            }
            return false;
        }

        /**
         * @brief private member that moves 
         */
        void moveLeft(size_t startFrom){
            std::unique_lock<std::mutex> v_Mutex;
            //it will erase this front element, that is the startFrom index.
            for(int i = startFrom; i < this->endItr-1; i++){
                v_data[i] = v_data[i+1]
            }
        }
        std::size_t getRandom(std::size_t startInd, std::size_t endInd){
                std::random_device rd;
                std::mt19937_64 gen(rd());
                std::uniform_int_distribution<std::size_t> distrib(startInd, endInd);
                std::size_t random_size = distrib(gen);
                return random_size;
        }



        public: 
        vec(){
            v_Capacity = 2;
        }
        /*passing intial size allows for vec size to be set to this*/
        vec(size_t intial){
            v_Capacity = intial;
        }
        ~vec(){
            delete T*[];


        }


        void appendTo(std::size_t indexToAppend){
            try{
                if(indexToAppend)
            }
        }

        std::size_t endItr(){
            return v_Size();
        }

        /** @brief a better named erase() from std::vector
         * moves down elements at that position
         * 
         */
        void eraseAt(std::size_t iteratorPosition){
            try{
                if(checkIndex(iteratorPosition)){

                }
                throw new std::exception();
            }catch(const std::exception &e){
                
            }
        }

        /**
         * returns front element
         */
        T frontElement(){
            try{
                if(size() != 0){
                    return data[v_StartIndex];
                }else{
                    throw new //range exception
                }
            }catch(const std::exception &e){

            }
        }
        
        //remove last element
        void popBack(){
            std::unique_lock<std::mutex> v_Mutex;
            this->_Size--;
        }


        void pushBack(T value){
            std::unique_lock<std::mutex> v_Mutex;
            if(v_Size == v_Capacity+1){
                //create new
            }
            v_Data[v_Size] = value;
        }

        //removes from the index provided
        void remove(std::size_t IndexToRemove){
            std::unique_lock<std::mutex> v_Mutex;

        }
        //removes from all indexes from start to finish
        void remove(std::size_t startIndex, std::size_t endIndex){
            
        }

        void resize(std::size_t resizeToSize){
            std::unique_lock<v_Mutex> lock;
            v_Capacity = resizeToSize;
        }

        /**
         * @brief simular to shrink to fit for vector. Changes so the array takes up exactly the size in memory
         */
        void shrinkToFit(){
            this->_Capacity = this->_Size;
        }

        std::size_t size(){
            return this->v_Size;
        }

        /**
         * @brief sorts list starting at startIndex, and ending at EndIndex
         * 
         */
        void sort(std::size_t startIndex, std::size_t endIndex){

        }

        void sort(std::size_t startIndex, std::size_t endIndex, std::function<T(T, T)> function){

        }

        // returns starting position
        std::size_t startItr(){
            return this->v_StartIndex;
        }

        
        void swap(T &dataA, T &dataB){
            T temp = dataA;
            dataA = dataB;
            dataB = temp;
        }

    };



}