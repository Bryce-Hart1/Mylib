#pragma once

#include<mutex>
#include <thread>
#include <iostream>
#include <random>
#include <cstddef>
#include <shared_mutex>
#include "exceptions.hpp"

/** @attention complete vector implemenation
 * My goal with this project is to provide a completely safe multithreaded vector implemenation with all functions mostly from scratch
 *
 * 
 */


namespace threadsafe{

    /** @class vec
    * threadsafe vector implementation
    * 
    */
    template <typename T> class vec{

        private:
        std::atomic<T*> v_Data;
        std::atomic<size_t> v_Size; //also our end iterator
        std::size_t v_Capacity;
        std::atomic<std::size_t> v_StartIndex;
        mutable std::shared_mutex v_realloc_mutex;
        
        //checks if index is in range
        bool p_checkIndex(size_t index){
            if(index >= v_StartIndex && index <= v_Size){
                return true;
            }
            return false;
        }

        //overload checking if both indexes are in range
        bool p_checkIndex(std::size_t index1, std::size_t index2){
            if(v_StartIndex <= index1 && index1 <= v_Size && v_StartIndex <= index2 && index2 <= v_Size){
                return true;
            }
            return false;
        }

        T* p_returnCopy(std::size_t newSize){
            T* newBlock = new T[newSize]; 
                for (size_t i = 0; i < v_Size; i++){
                    newBlock[i] = v_Data[i];
                }
                delete[] v_Data; 
        }

        /**
         * @brief private member that moves 
         */
        void p_moveLeft(std::size_t moveOnto){
            for(int i = moveOnto; i < v_Size-1; i++){
                v_Data[i] = v_Data[i+1];
            }
        }

        //must already be resized to +1
        void p_moveRight(std::size_t startFrom){
            for(int i = 0; i < v_Size; i++){
                v_Data[i+1] = v_Data[i];
            }
        }


        public: 
        vec() : v_Data(new T[2]), v_Size(0) v_Capacity(2), v_StartIndex(0){}

        /*passing intial size allows for vec size to be set to this*/
        vec(std::size_t capacity) : v_Data(new T[capacity]), v_Size(0), v_Capacity(2), v_StartIndex(0){}

        ~vec(){
            delete[] T* v_Data;
        }

        /**
         * @brief append a value to the desired index and moves right the index in that position
         * 
         */
        void appendTo(std::size_t indexToAppend){
            try{
                if(checkIndex(indexToAppend)){
                    if(v_Size == v_Capacity){
                        v_Data = p_returnCopy(v_Capacity * 2);
                    }
                }
                throw std::out_of_range("index out of range");
            }catch(std::exception &e){
                std::cerr << e << '\n';
            }
        }

        T at(std::size_t indexAt){
            try{
                if(checkIndex(indexAt)){
                    return v_Data[indexAt];
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
            
        }

        std::size_t capacity(){
            return this->v_Capacity;
        }

        //clears all elements of vector but does not shrink size
        void clear(){
            //not done just move delete data and start over
        }

        T endElement(){
            try{
                if(size() != 0){
                    return data[v_Size];
                }else{
                    throw std::out_of_range("index out of range");
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
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
                throw std::out_of_range("index out of range");
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }

        /**
         * returns front element
         */
        T frontElement() const{
            try{
                if(size() != 0){
                    return data[v_StartIndex];
                }else{
                    throw std::out_of_range("index out of range");
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }


        bool isEmpty(){
            if(v_Size == 0){
                return true;
            }
            return false;
        }
        
        //remove last element
        void popBack(){
            std::shared_lock lock(v_realloc_mutex);
            v_Size.fetch_sub(1);
        }


        void pushBack(T value){
            std::shared_lock lock(v_realloc_mutex);
            if(v_Size == v_Capacity+1){
                size_t allocateNewSize = (v_Capacity * 2);
                v_Data = returnCopy();   
                v_Capacity = allocateNewSize;                
            }
            v_Data[v_Size] = value;
        }

        //removes from the index provided
        void remove(std::size_t IndexToRemove){
            std::unique_lock<std::mutex> v_Mutex;
            
        }
        //removes from all indexes from start to finish
        void remove(std::size_t startIndex, std::size_t endIndex){
            std::unique_lock<std::mutex> v_Mutex;
        }

        /**
         * @brief overwrites element at desired index
         */
        void replace(T value, std::size_t index){
            try{
                if(checkIndex(index)){
                    v_Data[index] = value;
                }
                throw std::out_of_range("Index is out of range");
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }

        void resize(std::size_t resizeToSize){
            std::shared_lock lock(v_realloc_mutex);
            v_Capacity = resizeToSize;
        }

        /**
         * @brief simular to shrink to fit for vector. Changes so the array takes up exactly the size in memory
         */
        void shrinkToFit(){
            std::unique_lock<std::mutex> lock(v_Mutex);
            this->_Capacity = this->_Size;
        }

        //returns size of this vec
        std::size_t size(){
            std::unique_lock<std::mutex> lock(v_Mutex);
            return this->v_Size;
        }


        // returns starting position
        std::size_t startItr(){
            return this->v_StartIndex;
        }

        
        void swap(T &dataA, T &dataB){
            std::unique_lock<std::mutex> lock(v_Mutex);
            T temp = dataA;
            dataA = dataB;
            dataB = temp;
        }

    };
}