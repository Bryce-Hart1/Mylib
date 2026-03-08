#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include <shared_mutex>
#include <thread>
#include <syncstream>
#include <variant>
#include <cassert>
#include <type_traits>
#include <iomanip>
#include <exception>
#include <iostream>

using sizeT = std::size_t;
using string = std::string;


namespace threadsafe{

class Trie{
    public:
    /**
     * @brief node class is wrapped inside Trie class and not accessable by user
     * atomic bool to edit value, 
     * 
     * uses a hybrid design of spinlocks on deep nodes to save memory, and mutexes on high contention 
     * points in the tree. I have a factory in node to help determine this, with an option to set this within the
     * trie constructor and at what depth. I will pick a safe, default depth.
     * There is alot to this class so I will attempt to explain it the best I can:
     * @struct mutexLock a mutex wrapper, used for higher nodes 
     * @struct spinLock is a spinLock as the name suggest 
     */   
struct mutexLock{
    private:
    std::shared_mutex mtx;

    public:
    void unlock(){
        this->mtx.unlock();
    }


    void lock(){
        this->mtx.lock();
    }

};

struct spinLock{
    private:
    std::atomic_flag atomic_flag = ATOMIC_FLAG_INIT;

    public:
    void lock(){
        while(atomic_flag.test_and_set(std::memory_order_acquire)){/*spin infinitely*/}
    }

    void unlock(){
        atomic_flag.clear(std::memory_order_release);
    }


};
struct node {
    char value;
    bool isEndpoint;
    bool isRoot;
    sizeT count;
    std::vector<std::unique_ptr<node>> childrenNodes;
    std::variant<mutexLock, spinLock> nodeLock;
    node(char val) : value(val), isEndpoint(false), isRoot(false), count(0) {};

    struct lockGuard {
        std::variant<mutexLock, spinLock>& _lock;
        bool _owns;

        explicit lockGuard(std::variant<mutexLock, spinLock>& l) : _lock(l), _owns(true) {
            std::visit([](auto& lk) { lk.lock(); }, _lock);
        }

        ~lockGuard() {
            if (_owns){ //if owner
                std::visit([](auto& lk) { lk.unlock(); }, _lock);
            }
        }

        lockGuard(const lockGuard&) = delete;
        lockGuard& operator=(const lockGuard&) = delete;

        lockGuard(lockGuard&& other) noexcept : _lock(other._lock), _owns(other._owns) {
            other._owns = false; // transfer ownership, prevent double unlock
        }

            lockGuard& operator=(lockGuard&&) = delete;
            }; //end of lockguard
        };// end of node

    private:
        std::unique_ptr<node> v_root; 
        std::atomic<sizeT> v_nodeCount;
        std::atomic<sizeT> wordCount;
        sizeT v_mutexCutoff; 

        void setEndpointTrue(node& n){
            node::lockGuard guard(n.nodeLock);
            n.isEndpoint = true;
        }

        void increment(node& n){
            node::lockGuard guard(n.nodeLock);
            n.count++;
        }

        void getAllWords(std::vector<std::string>& put, std::string prefix, node* current){
            if(current == nullptr) return;
            if(current->isEndpoint){
                put.push_back(prefix + current->value);
            }
            for(auto& child : current->childrenNodes){
                getAllWords(put, prefix + current->value, child.get());
            }
        }


    public:
        //default constructor gets called on root
        Trie(){
            v_root = std::make_unique<node>();
            v_root->value = '*';
            v_root->isEndpoint = false;
        }

        Trie(string intial){
            v_root = std::make_unique<node>();
            v_root->value = '*';
            v_root->isEndpoint = false;
            add(intial);
        }

        Trie(sizeT HIGH_CONTENTION_CUTOFF){
            try{
                if(HIGH_CONTENTION_CUTOFF < 3){
                    throw std::invalid_argument("HIGH_CONTENTION_CUTOFF cannot be below 2,");
                }
                this->v_mutexCutoff = HIGH_CONTENTION_CUTOFF;
            }catch(const std::exception& error){
                std::cerr << error.what() << '\n';
                std::cout << "Trie creation failed" << '\n';
            }            
        }


        Trie(string intial, sizeT HIGH_CONTENTION_CUTOFF){
            try{
                if(HIGH_CONTENTION_CUTOFF < 3){
                    throw std::invalid_argument("HIGH_CONTENTION_CUTOFF cannot be below 2,");
                }
                this->v_mutexCutoff = HIGH_CONTENTION_CUTOFF;
            }catch(const std::exception& error){
                std::cerr << error.what() << '\n';
            }
            v_root = std::make_unique<node>();
            v_root->value = '*';
            v_root->isEndpoint = false;

        }

        bool getIsEndPoint(node thisNode) const{
            if(thisNode.isEndpoint){
                return true;
            }
            return false;
        }

    
        sizeT getWordCount(){
            return this->wordCount;
        }

    //returns a node at requested position, if not returns nullptr
    node* findChildNode(node n, char lookingFor){
        

        for(int i = 0; i < n.childrenNodes.size(); i++){
            if(lookingFor == n.childrenNodes.at(i)->value){
                return n.childrenNodes.at(i).get();
            }
        }
        return nullptr;
    }

    sizeT getChildCount(node n) const{
        //n.nodeLock;
        return n.childrenNodes.size();
    }


        //adds entire word to the trie. 
    void add(std::string word){
        node* current = v_root.get();
    
        for(sizeT i = 0; i < word.length(); i++){
            char ch = word[i];
        
            node::lockGuard guard(current->nodeLock);  // locks here
        
            node* thisChild = nullptr;
            for(auto& c : current->childrenNodes){
                if(c->value == ch){
                    thisChild = c.get();
                    break;
                }
            }
        if(thisChild == nullptr){
            current->childrenNodes.emplace_back(std::make_unique<node>(ch));
            thisChild = current->childrenNodes.back().get();
        }
        
        current = thisChild;
    }  // guard goes out of scope here it will unlock
    
    setEndpointTrue(*current);
    increment(*current);
    wordCount++;
}
    std::vector<string> getWords(){
        std::vector<string> r;
        r.reserve(this->getWordCount());
        for(auto& child : v_root->childrenNodes){
            getAllWords(r, "", child.get());
        }
        return r;
    }

    }; //end of trie

}