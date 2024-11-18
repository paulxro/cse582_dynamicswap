

#include "hello.hpp"

std::atomic<bool> dynamic_manager_running = true;
std::mutex object_addrs_mut;
std::vector<far_memory::GenericArray*> object_addrs;


/*
    array = ...

    DerefScope scope;

    array.at(scope, 0);



*/

int register_object(far_memory::GenericArray* const ptr) {
    object_addrs_mut.lock();
    object_addrs.push_back(ptr);
    object_addrs_mut.unlock();

    return 0;
}

int init_dynamic_scheduler() {
    std::thread scheduler_thread(dynamic_scheduler);

    scheduler_thread.detach();

    stop_dynamic_scheduler();

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
            for (uint64_t i = 0; i < array_ptr->kNumItems_; i++) {
                const auto& uptr = array_ptr->ptrs_[i];
                if (!uptr.meta().is_present() && iptr.meta().is_hot()) {
                    far_memory::DerefScope scope;
                    (*((far_memory::Array<uint64_t, 4096>*)(array_ptr))).at(scope, i);
                }
            }

        }
        object_addrs_mut.unlock();
    }

    fprintf(f, "%s\n", "Stopping scheduler...");

    fclose(f);
    return 0;
}

int stop_dynamic_scheduler() {
    FILE* f = fopen("stop_output", "w");
    setbuf(f, NULL);
    fprintf(f, "%s\n", "Stopping scheduler...");
    fclose(f);
    dynamic_manager_running = false;
    return 0;
}