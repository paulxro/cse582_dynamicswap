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
#include "pointer.hpp"

int register_object(far_memory::GenericUniquePtr*);

int dynamic_scheduler();

int init_dynamic_scheduler();
int stop_dynamic_scheduler();