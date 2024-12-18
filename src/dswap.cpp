#include "dswap.hpp"

const uint64_t PAGER_QUANTUM = 50; // microseconds
const uint64_t MEMORY_QUANTUM = 50; // microseconds

uint64_t cache_size = 0;

std::mutex object_addrs_mut;
std::vector<DSwap::ObjRef> object_refs;
std::vector<far_memory::GenericUniquePtr*> object_addrs;

std::atomic<bool> dynamic_manager_running;
std::atomic<uint64_t> available_mem;
std::atomic<uint64_t> swap_threshold;

int register_object(far_memory::GenericUniquePtr * const ptr, uint64_t size) {
    object_addrs_mut.lock();
    object_refs.emplace_back(ptr, size);
    object_addrs.push_back(ptr);
    object_addrs_mut.unlock();

    return 0;
}

int init_dynamic_pager(uint64_t c_size) {

    FILE* f = fopen("init_output", "w");
    setbuf(f, NULL);

    dynamic_manager_running.store(true);
    cache_size = c_size;
    swap_threshold.store(std::ceil(cache_size * 0.1));

    std::thread scheduler_thread(dynamic_pager);
    std::thread memory_thread   (dynamic_memory);

    scheduler_thread.detach();
    memory_thread.detach();

    fclose(f);

    return 0;
}

int dynamic_pager() {

    FILE* f = fopen("dswap_log", "w");
    setbuf(f, NULL);

    fprintf(f, "Starting pager block... (PID = %d)\n", getpid());

    while (dynamic_manager_running) {
        {
            std::lock_guard<std::mutex> guard{object_addrs_mut};

            for (const auto& ref : object_refs) {
                if (available_mem.load() < swap_threshold.load())
                    break;
                if (ref.ptr->meta().is_present())
                    continue;
                ref.ptr->swap_in(false);
            }
        }

        sleep:
            std::this_thread::sleep_for(
                std::chrono::microseconds(PAGER_QUANTUM)
            );
    }

    fprintf(f, "%s\n", "Stopping pager block...");

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

    fprintf(f, "%s\n", "Starting memory block...");

    while (dynamic_manager_running) {
        {
            std::lock_guard<std::mutex> guard{object_addrs_mut};
            uint64_t total = 0;

            for (const auto& ref : object_refs) {
                if (!ref.ptr->meta().is_present())
                    continue;
                total += ref.size;
            }

            available_mem.store(cache_size - total);
        }

        std::this_thread::sleep_for(
            std::chrono::microseconds(MEMORY_QUANTUM)
        );

    }

    fprintf(f, "%s\n", "Stopping memory block...");
    fclose(f);

    return 0;

}