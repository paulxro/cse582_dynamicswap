#pragma once

#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <optional>
#include <vector>
#include "pointer.hpp"

int register_object(void*);
int dynamic_scheduler();