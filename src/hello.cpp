

#include "hello.hpp"

std::atomic<bool> dynamic_manager_running = true;
std::mutex object_addrs_mut;
std::vector<far_memory::GenericUniquePtr*> object_addrs;

int register_object(far_memory::GenericUniquePtr * const ptr) {
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

        fprintf(f, "%d\n", i++);
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