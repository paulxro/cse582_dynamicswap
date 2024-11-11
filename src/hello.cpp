

#include "hello.hpp"

std::mutex object_addrs_mut;
std::vector<void*> object_addrs;




int register_object(void* addr) {
    object_addrs_mut.lock();
    object_addrs.push_back(addr);

    far_memory::GenericUniquePtr ptr;
    

    FILE* f = fopen("output", "w");
    setbuf(f, NULL);

    for (size_t i = 0; i < object_addrs.size(); i++) {
        fprintf(f, "%p\n", (void*)object_addrs[i]);
    }


    fclose(f);

    object_addrs_mut.unlock();


    return 0;
}


int dynamic_scheduler() {

    FILE* f = fopen("output", "w");
    setbuf(f, NULL);

    fprintf(f, "%s %d\n", "Running dynamic", getpid());

    fclose(f);
    

    return 0;
}