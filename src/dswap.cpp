#include "dswap.hpp"

const uint64_t PAGER_QUANTUM = 50; // microseconds
const uint64_t MEMORY_QUANTUM = 50; // microseconds

std::mutex object_addrs_mut;
std::vector<far_memory::GenericUniquePtr *> object_addrs;

std::atomic<bool> dynamic_manager_running;
std::atomic<uint64_t> available_mem;
std::atomic<uint64_t> swap_threshold;

int register_object(far_memory::GenericUniquePtr * const ptr) {
    object_addrs_mut.lock();
    object_addrs.push_back(ptr);
    object_addrs_mut.unlock();

    return 0;
}

int init_dynamic_pager(uint64_t cache_size) {

    FILE* f = fopen("init_output", "w");
    setbuf(f, NULL);

    // fprintf(f, "Manager is @ %p\n", FarMemManagerFactory::get());

    dynamic_manager_running.store(true);
    available_mem.store(cache_size);
    swap_threshold.store(std::ceil(cache_size * 0.1));

    std::thread scheduler_thread(dynamic_pager);
    std::thread memory_thread   (dynamic_memory);

    scheduler_thread.detach();
    memory_thread.detach();

    fclose(f);

    return 0;
}

int dynamic_pager() {

    FILE* f = fopen("output", "w");
    setbuf(f, NULL);

    int i = 0;

    fprintf(f, "Starting scheduler... (PID = %d)\n", getpid());

    while (dynamic_manager_running) {

        {
            std::lock_guard<std::mutex> guard{object_addrs_mut};

            for (auto uptr : object_addrs) {
                if (available_mem.load() < swap_threshold.load())
                    break;
                if (uptr->meta().is_present())
                    continue;
                fprintf(f, "Found non-local: %d\n", uptr->meta().is_present());
                uptr->swap_in(false);
            }
        }

        sleep:
            std::this_thread::sleep_for(std::chrono::microseconds(PAGER_QUANTUM));
    }

    fprintf(f, "%s\n", "Stopping scheduler...");

    fclose(f);
    return 0;
}

int stop_dynamic_pager() {
    dynamic_manager_running = false;
    return 0;
}

int dynamic_memory() {

    FILE* f = fopen("mem_output", "w");
    setbuf(f, NULL);

    while (dynamic_manager_running) {
        {
            std::lock_guard<std::mutex> guard{object_addrs_mut};
            uint64_t total = 0;

            for (auto uptr : object_addrs) {
                if (!uptr->meta().is_present())
                    continue;
                fprintf(f, "[MEM] Found local: %d\n", uptr->meta().is_present());
                // total += uptr->meta().get_object_size();
            }

            available_mem.store(swap_threshold.load() - total);

            sleep:
                std::this_thread::sleep_for(std::chrono::microseconds(MEMORY_QUANTUM));
        }

    }

    fprintf(f, "%s\n", "Stopping scheduler...");
    fclose(f);
    return 0;

}