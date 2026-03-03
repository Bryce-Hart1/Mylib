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
namespace threadSafe{



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
    std::shared_mutex mtx;


    void unlock(){
        this->mtx.unlock();
    }


    void lock(){
        this->mtx.lock();
    }

};

struct spinLock{


    void unlock(){

    }


    void lock(){

    }


};

struct node {
    char value;
    bool isEndpoint;
    bool isRoot;
    std::size_t count;
    std::vector<std::unique_ptr<node>> childrenNodes;
    std::variant<mutexLock, spinLock> nodeLock;

struct lockGuard {
    std::variant<mutexLock, spinLock>& _lock;
    bool _owns;

    explicit lockGuard(std::variant<mutexLock, spinLock>& l) : _lock(l), _owns(true) {
        std::visit([](auto& lk) { lk.lock(); }, _lock);
    }

    ~lockGuard() {
        if (_owns){
            std::visit([](auto& lk) { lk.unlock(); }, _lock);
        }
    }

    lockGuard(const lockGuard&) = delete;
    lockGuard& operator=(const lockGuard&) = delete;

    lockGuard(lockGuard&& other) noexcept : _lock(other._lock), _owns(other._owns) {
        other._owns = false; // transfer ownership, prevent double unlock
    }
    lockGuard& operator=(lockGuard&&) = delete;
};

        std::unique_ptr<node> v_root;
        std::atomic<std::size_t> v_nodeCount;
        std::atomic<std::size_t> wordCount;


        void setEndPointTrue(){

        }
    public:

    //default constructor gets called on root
    Trie(){
        v_root->value = '*';
        v_root->isEndPoint = false;
    }

    Trie(std::string intial){
        v_root->value = '*';
        v_root->isEndPoint = false;
        add(intial);
    }

    bool getIsEndPoint() const{
        
    }


//returns a node at requested position, if not returns nullptr
    node* findChildNode(node n, char lookingFor){
        

        for(int i = 0; i < n.children.size(); i++){
            if(lookingFor == n.children.at(i)->value){
                return *n.children.at(i);
            }
        }
        return nullptr;
    }

unsigned int getChildCount() const{
    std::lock_guard<std::mutex> lock(mtx);
    return children.size();
}


        //adds entire word
        void add(std::string word){
            node* current;
    
            for(size_t i = 0; i < word.length(); i++){
                char ch = word[i];

        
        std::lock_guard<std::mutex> lock(current->mtx); //lock current
        
        node* thisChild = nullptr;
        
        for(auto& c : current->children){
            if(c->value == ch){
                thisChild = c.get();
                break;
            }
        }
        
        if(thisChild == nullptr){
            current->children.emplace_back(std::make_unique<atomicNode>(ch));
            thisChild = current->children.back().get();

        }
        
        current = thisChild;
    }
    
    //these need to be outside any lock since they acquire their own locks anyway
    current->setEndPointTrue();
    current->increment();
}

};

}