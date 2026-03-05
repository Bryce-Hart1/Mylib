#include "tsVector.hpp"
#include <vector>

#include "tsQueue.hpp"
#include <queue>

#include "tsStack.hpp"
#include <stack>

#include <unordered_map>
#include "tsHashmap.hpp"

#include <functional>





/**
 * @attention testing of the non tree structures - hashmap, queue, stack, and vec
 * 
 */

void vecPushBack(threadsafe::vec<int> x, int i){
    x.pushBack(i);
}


//test pushback, clear, shrink to fit and at features and compare the outputs to std::vector
bool Vector_testOne(threadsafe::vec<int>& x, std::vector<int> y){
    threadsafe::vec<int> name;
    
    const int oneM = 1000000;
    int n = 0; //number pushed into both
    for(int i = 0; i < oneM; i++){
        std::thread t(vecPushBack, std::ref(x), n);
        t.join();
        n++;
    }
    if(x.size() != y.size()){
        return false;
    }else{
        for(int i = 0; i < oneM; i++){
            if(x.at(i) != y.at(i)){
                return false;
            }
        }
    }
    x.clear();
    y.clear();
    x.shrinkToFit();
    y.shrink_to_fit();
    if(x.size() != y.size()){
        return false;
    }


}





bool testVector(){
    using namespace std;
    string p = "In testVector: ";
    threadsafe::vec<int> tsVector;
    vector<int> vector;
    cout << "size of vector " << sizeof(vector);
    cout << "size of tsVector" << sizeof(tsVector);
    if(!Vector_testOne(tsVector, vector)){
        cout << p <<  "Failed Testone" << endl;
    }
    

}






bool testQueue(){
    std::queue<u_int16_t> x;
    threadsafe::queue<u_int16_t> y;
    
}




bool testStack(){
    std::stack<u_int16_t> x;
    threadsafe::stack<u_int16_t> y;

}


bool testMap(){
    std::unordered_map<char, u_int8_t> x;
    threadsafe::hashmap<char, u_int8_t> y;
    
}











int main(){
    if(testVector()){
        std::cout << "All Vector tests passed" << std::endl;
    }
    if(testQueue()){
        std::cout << "All Vector tests passed" << std::endl;
    }
    if(testStack()){
        std::cout << "All Stack tests passed" << std::endl;
    }
}