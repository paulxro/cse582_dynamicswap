#pragma once

#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <optional>
#include <vector>
#include <thread>
#include <atomic>
#include <signal.h>
#include <unistd.h>
#include <cmath>
#include "array.hpp"
#include "device.hpp"
#include "helpers.hpp"

namespace DSwap {
    class ObjRef {
        public:
            uint64_t size;
            far_memory::GenericUniquePtr* ptr;
            ObjRef(far_memory::GenericUniquePtr* ptr, uint64_t size) {
                this->size = size;
                this->ptr  = ptr;
            }

            
    };
}



int register_object(far_memory::GenericUniquePtr *);

int dynamic_pager();
int dynamic_memory();

int init_dynamic_pager(uint64_t);
int stop_dynamic_pager();