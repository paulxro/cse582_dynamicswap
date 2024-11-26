

#include "hello.hpp"

std::atomic<bool> dynamic_manager_running = true;
std::mutex object_addrs_mut;
std::vector<far_memory::GenericUniquePtr *> object_addrs;


/*
    array = ...

    DerefScope scope;

    array.at(scope, 0);



*/

int register_object(far_memory::GenericUniquePtr * const ptr) {
    // FILE* f = fopen("register_output", "a");
    // setbuf(f, NULL);

    // fprintf(f, "%s\n", "Registering object...");

    object_addrs_mut.lock();
    object_addrs.push_back(ptr);
    object_addrs_mut.unlock();
    
    // fclose(f);

    return 0;
}

int init_dynamic_scheduler() {
    std::thread scheduler_thread(dynamic_scheduler);

    scheduler_thread.detach();

    return 0;
}

int dynamic_scheduler() {

    FILE* f = fopen("output", "w");
    setbuf(f, NULL);

    int i = 0;

    fprintf(f, "%s\n", "Starting scheduler...");

    while (dynamic_manager_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        object_addrs_mut.lock();
        for (const auto& array_ptr : object_addrs) {
            // for (uint64_t i = 0; i < array_ptr->kNumItems_; i++) {
            //     const auto& uptr = array_ptr->ptrs_[i];
            //     if (!uptr.meta().is_present() && uptr.meta().is_hot()) {
            //         far_memory::DerefScope scope;
            //         (*((far_memory::Array<uint64_t, 4096>*)(array_ptr))).at(scope, i);
            //     }
            // }

        }
        object_addrs_mut.unlock();
    }

    fprintf(f, "%s\n", "Stopping scheduler...");

    fclose(f);
    return 0;
}

int stop_dynamic_scheduler() {
    dynamic_manager_running = false;
    return 0;
}