#include<vector>
#include<mutex>
#include<memory>



namespace threadSafe{
    template <typename Type>


    class biTree{
        struct node{
            std::mutex nodeLock;
            Type value;
            std::unique_ptr<Type> left;
            std::unique_ptr<Type> right;
        };
        private:
            node root;
        public:
            biTree(){}
            ~biTree(){}

            void add(){

            }
            void center(){

            }
            bool find(){

            }

            std::vector<Type> orderedList(){
                
            }

            void remove(Type remove){

            }



            

            
    };
}