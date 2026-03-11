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
        T* v_Data;
        std::size_t v_Size; //also our end iterator
        std::size_t v_Capacity;
        std::size_t v_StartIndex;
        mutable std::shared_mutex v_mutex;
        

        /**
         * @attention if adding private members, do NOT add locks. Public member is already locking.
         * 
         */

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
                return newBlock;
        }

        /**
         * @brief private member that moves ontop of index provided. 
         * ex: [1] [2] [3]
         * provide 2
         * : [1] [3] [3]
         * must condense size by one to compensate for this
         */
        void p_moveLeft(std::size_t moveOnto){
            for(int i = moveOnto; i < v_Size-1; i++){
                v_Data[i] = v_Data[i+1];
            }
        }


        /**
         * @brief overload helper for remove, removes all elements from onto to the end of the 
         * gap. This overload helps us do one loop instead of several
         */
        void p_removeOverloadHelper(std::size_t start, std::size_t end){
            //still needs implemented
        }

        //must already be resized to +1, and the startFrom index will exist twice, once at i = 0, and i = 1
        void p_moveRight(std::size_t startFrom){
            for(int i = 0; i < v_Size; i++){
                v_Data[i+1] = v_Data[i];
            }
        }


        public: 
        vec() : v_Data(new T[2]), v_Size(0), v_Capacity(2), v_StartIndex(0){}

        /*passing intial size allows for vec size to be set to this*/
        vec(std::size_t capacity) : v_Data(new T[capacity]), v_Size(0), v_Capacity(2), v_StartIndex(0){}

        ~vec(){ delete[] T* v_Data.load(); }

        /**
         * @brief append a value to the desired index and moves right the index in that position
         * 
         */
        void appendTo(std::size_t indexToAppend, T value){
            std::shared_lock lock(v_mutex);
            try{
                if(checkIndex(indexToAppend)){
                    if(v_Size == v_Capacity){
                        v_Data = p_returnCopy(v_Capacity * 2);
                        v_Capacity *= 2;
                    }
                    p_moveRight(indexToAppend);
                    v_data[indexToAppend] = value;
                    return;
                }
                throw std::out_of_range("index out of range");
            }catch(std::exception &e){
                std::cerr << e << '\n';
            }
        }

        T at(std::size_t indexAt){
            std::shared_lock lock(v_mutex);
            try{
                if(checkIndex(indexAt)){
                    return v_Data[indexAt];
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
            
        }

        //return current possible capacity of vec
        std::size_t capacity(){
            std::shared_mutex lock(v_mutex);
            return this->v_Capacity;
        }

        //clears all elements of vector but does not shrink size
        void clear(){
            std::shared_mutex lock(v_mutex);
            v_Data = new T[capacity];
            v_Size = v_StartIndex = 0;

        }

        //I assume this would not be the defaulted wanted behavior, but it is an option that I will provide

        //clears all elements and resets size
        void clear(bool RESET_CAPACITY){
            std::shared_mutex lock(v_mutex);
            v_Capacity = 2;
            clear();

        }


        //retrieve last element in vector
        T endElement(){
            std::shared_mutex lock(v_mutex);
            try{
                if(size() != 0){
                    return v_Data[v_Size];
                }else{
                    throw std::out_of_range("index out of range");
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }


        std::size_t endItr(){
            return v_Size.load(); //pull back at current moment
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
                    return v_Data[v_StartIndex];
                }else{
                    throw std::out_of_range("index out of range");
                }
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }


        bool isEmpty(){
            std::shared_lock lock(v_mutex);
            if(v_Size == 0){
                return true;
            }
            return false;
        }
        
        //remove last element
        void popBack(){
            std::shared_lock lock(v_mutex);
            v_Size.fetch_sub(1);
        }

        
        /**
         * @details pushBack
         * Push value of vec type or push back a vec of same type
         * @param value - value or values to pushback
         */
        void pushBack(T value){
            std::shared_lock lock(v_mutex);
            if(v_Size == v_Capacity+1){
                size_t allocateNewSize = (v_Capacity * 2);
                v_Data = p_returnCopy(allocateNewSize);   
                v_Capacity = allocateNewSize;                
            }
            v_Data[v_Size] = value;
            v_Size++;
        }


        //removes from the index provided
        void remove(std::size_t IndexToRemove){
            std::shared_lock lock(v_mutex);
            p_moveLeft(IndexToRemove);
        }
        //removes from all indexes from starting index to ending index 
        void remove(std::size_t startIndex, std::size_t endIndex){
            std::shared_lock lock(v_mutex);
            try{
                if(p_checkIndex(startIndex, endIndex)){

                }
                throw std::out_of_range("Index is out of range in Remove(start, end)");
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }

        /**
         * @brief overwrites element at desired index
         */
        void replace(T value, std::size_t index){
            std::shared_lock lock(v_mutex);
            try{
                if(checkIndex(index)){
                    v_Data[index] = value;
                }
                throw std::out_of_range("Index is out of range");
            }catch(const std::exception &e){
                std::cerr << e << '\n';
            }
        }

        //Capacity of vec becomes resizeToSize
        void resize(std::size_t resizeToSize){
            std::shared_lock lock(v_mutex);
            v_Capacity = resizeToSize;
        }

        /**
         * @brief simular to shrink to fit for vector. Changes so the array takes up exactly the size in memory
         */
        void shrinkToFit(){
            std::shared_lock lock(v_mutex);
            this->v_Capacity = this->v_Size;
        }

        //returns size of this vec object
        std::size_t size(){
            std::shared_lock lock(v_mutex);
            return this->v_Size;
        }


        // returns starting iterator position
        std::size_t startItr(){
            std::shared_lock lock(v_mutex);
            return this->v_StartIndex;
        }

        
        void swap(T &dataA, T &dataB){
            std::shared_lock lock(v_mutex);
            T temp = dataA;
            dataA = dataB;
            dataB = temp;
        }

    };
}