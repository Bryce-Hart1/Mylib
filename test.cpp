#include "tsVector.hpp"
#include <vector>
#include <functional>


void vecPushBack(threadsafe::vec<int> x, int i){
    x.pushBack(i);
}


//test pushback, clear, shrink to fit and at features and compare the outputs to std::vector
bool Vector_testOne(threadsafe::vec<int>& x, std::vector<int> y){

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











int main(){
    if(testVector()){
        std::cout << "All Vector tests passed" << std::endl;
    }
}