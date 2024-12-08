

#include "hello.hpp"

std::atomic<bool> dynamic_manager_running;
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
    dynamic_manager_running.store(true);

    std::thread scheduler_thread(dynamic_scheduler);

    scheduler_thread.detach();

    return 0;
}

int dynamic_scheduler() {

    FILE* f = fopen("output", "w");
    setbuf(f, NULL);

    int i = 0;

    fprintf(f, "Starting scheduler... (PID = %d)\n", getpid());

    while (dynamic_manager_running) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));

        object_addrs_mut.lock();
        for (auto uptr : object_addrs) {
            if (!uptr->meta().is_present()) {
                fprintf(f, "%s\n", "Found non-present ptr object1...");
                far_memory::DerefScope scope;
                auto uptr_cast = reinterpret_cast<far_memory::UniquePtr<uint64_t> *>(uptr);
                uptr_cast->template deref<true>(scope);
                fprintf(f, "%s\n", "Brought it in...");
            }

            // fprintf(f, "local ?= %d\n", uptr->meta().is_present());

        }
        
        object_addrs_mut.unlock();
        break;
    }

    fprintf(f, "%s\n", "Stopping scheduler...");

    fclose(f);
    return 0;
}

int stop_dynamic_scheduler() {
    dynamic_manager_running = false;
    return 0;
}