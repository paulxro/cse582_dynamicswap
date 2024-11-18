extern "C" {
#include <runtime/runtime.h>
}

#include "array.hpp"
#include "device.hpp"
#include "helpers.hpp"
#include "manager.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

extern int register_object(far_memory::Array<uint64_t, 4096>* const);

using namespace far_memory;
using namespace std;

static inline void fail_test(void) {
  cout << "Failed" << endl;
  exit(1);
}

static inline void pass_test(void) {
  cout << "Passed" << endl;
  exit(1);
}

extern std::vector<far_memory::GenericUniquePtr*> object_addrs;

constexpr static uint64_t kCacheSize = (128ULL << 20);
constexpr static uint64_t kFarMemSize = (4ULL << 30);
constexpr static uint32_t kNumGCThreads = 12;
constexpr static uint32_t kNumEntries =
    (8ULL << 20); // So the array size is larger than the local cache size.
constexpr static uint32_t kNumConnections = 300;

uint64_t raw_array_A[kNumEntries];
uint64_t raw_array_B[kNumEntries];
uint64_t raw_array_C[kNumEntries];

template <uint64_t N, typename T>
void copy_array(Array<T, N> *array, T *raw_array) {
  for (uint64_t i = 0; i < N; i++) {
    DerefScope scope;
    (*array).at_mut(scope, i) = raw_array[i];
  }
}

template <typename T, uint64_t N>
void add_array(Array<T, N> *array_C, Array<T, N> *array_A,
               Array<T, N> *array_B) {
  for (uint64_t i = 0; i < N; i++) {
    DerefScope scope;
    (*array_C).at_mut(scope, i) =
        (*array_A).at(scope, i) + (*array_B).at(scope, i);
  }
}

void gen_random_array(uint64_t num_entries, uint64_t *raw_array) {
  std::random_device rd;
  std::mt19937_64 eng(rd());
  std::uniform_int_distribution<uint64_t> distr;

  for (uint64_t i = 0; i < num_entries; i++) {
    raw_array[i] = distr(eng);
  }
}

void do_work(FarMemManager *manager) {
  auto array_A = manager->allocate_array<uint64_t, kNumEntries>();
  auto array_B = manager->allocate_array<uint64_t, kNumEntries>();
  auto array_C = manager->allocate_array<uint64_t, kNumEntries>();

  register_object

  array_A.disable_prefetch();
  array_B.disable_prefetch();
  array_C.disable_prefetch();

  FILE *f = fopen("output", "w");
  setbuf(f, NULL);


  int seen = -1;
  for (uint64_t i = 0; i < 3 * kNumEntries; i++) {
    // fprintf(f, "%lu : %d\n", i, object_addrs.at(i)->meta().is_present());
    if (i % 1000 == 0) {
        fprintf(f, "%lu : %d\n", i, object_addrs.at(i)->meta().get_object_size());
        seen = object_addrs.at(i)->meta().get_object_size();
    }
    // if (object_addrs.at(i)->meta().is_present())
    //     fail_test();
    // DerefScope scope;
    // array_A.at(scope, i);
  }

//   fprintf(f, "%s\n", "Passed A");

//   for (uint64_t i = kNumEntries; i < 2 * kNumEntries; i++) {
//     if (!object_addrs.at(i)->meta().is_present())
//         fail_test();
//     DerefScope scope;
//     array_B.at(scope, i - kNumEntries);
//   }

//   fprintf(f, "%s\n", "Passed B");

//   for (uint64_t i = 2 * kNumEntries; i < 3 * kNumEntries; i++) {
//     if (!object_addrs.at(i)->meta().is_present())
//         fail_test();
//     DerefScope scope;
//     array_C.at(scope, i - 2 * kNumEntries);
//   }

  pass_test();
}

int argc;
void _main(void *arg) {
  char **argv = static_cast<char **>(arg);
  std::string ip_addr_port(argv[1]);
  auto raddr = helpers::str_to_netaddr(ip_addr_port);
  std::unique_ptr<FarMemManager> manager =
      std::unique_ptr<FarMemManager>(FarMemManagerFactory::build(
          kCacheSize, kNumGCThreads,
          new TCPDevice(raddr, kNumConnections, kFarMemSize)));
  do_work(manager.get());
}

int main(int _argc, char *argv[]) {
  int ret;

  if (_argc < 3) {
    std::cerr << "usage: [cfg_file] [ip_addr:port]" << std::endl;
    return -EINVAL;
  }

  char conf_path[strlen(argv[1]) + 1];
  strcpy(conf_path, argv[1]);
  for (int i = 2; i < _argc; i++) {
    argv[i - 1] = argv[i];
  }
  argc = _argc - 1;

  ret = runtime_init(conf_path, _main, argv);
  if (ret) {
    std::cerr << "failed to start runtime" << std::endl;
    return ret;
  }

  return 0;
}
