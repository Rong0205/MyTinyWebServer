#include "threadpool.h"

int main(){
    std::cout<< "111"<<std::endl;
    ThreadPool pool(4);

    for(int i = 0; i < 10; i++){
        pool.AddTask([i]{
            std::cout << "task "<<i<<"is running" <<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "task "<<i<<"is done"<<std::endl; 
        });
    }
}