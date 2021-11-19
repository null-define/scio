//
// Created by omni on 11/10/2021.
//

#ifndef scio_FN_COST_H
#define scio_FN_COST_H

#include "scio/io/console.h"
#include <sys/resource.h>
#include <sys/time.h>
#include <thread>

namespace scio {

struct fn_cost_t {
  double cpu_time;
  double wall_time;
  double memory;
  double peak_memory;
  double peak_rss;
  double peak_vmsize;
  double peak_stack;
  double peak_heap;
  double peak_shmem;
  double peak_locked;
  double peak_swap;
  double peak_io;
  double peak_pf;
  double peak_pageins;
  double peak_pageouts;
  double peak_messages;
  double peak_syscalls;
  double peak_syscalls_read;
  double peak_syscalls_write;
  double peak_syscalls_other;
  double peak_syscalls_read_bytes;
  double peak_syscalls_write_bytes;
  double peak_syscalls_other_bytes;
  double peak_syscalls_read_time;
  double peak_syscalls_write_time;
  double peak_syscalls_other_time;
  double peak_syscalls_read_bytes_time;
  double peak_syscalls_write_bytes_time;
  double peak_syscalls_other_bytes_time;
  double peak_syscalls_read_time_time;
  double peak_syscalls_write_time_time;
  double peak_syscalls_other_time_time;
  double peak_syscalls_read_bytes_time_time;
  double peak_syscalls_write_bytes_time_time;
  double peak_syscalls_other_bytes_time_time;
  double peak_syscalls_read_time_time_time;
  double peak_syscalls_write_time_time_time;
};

/**
 * @brief get the memory and time cost of the function
 */
template <typename F> auto fn_cost(F &&f) {
  return ([&f]() {
    fn_cost_t cost;
    struct rusage usage1, usage2;
    struct timeval start, end;
    getrusage(RUSAGE_THREAD, &usage1);
    gettimeofday(&start, nullptr);
    f();
    gettimeofday(&end, nullptr);
    getrusage(RUSAGE_THREAD, &usage);
    cost.wall_time =
        end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
    cost.cpu_time = (usage2.ru_utime.tv_sec - usage1.ru_utime.tv_sec) +
                    (usage2.ru_stime.tv_sec - usage1.ru_stime.tv_sec) +
                    (usage2.ru_utime.tv_usec / 1000000.0 -
                     usage1.ru_utime.tv_usec / 1000000.0) +
                    (usage2.ru_stime.tv_usec / 1000000.0 -
                     usage1.ru_stime.tv_usec / 1000000.0);
    cost.memory = usage2.ru_maxrss - usage1.ru_maxrss;
    return cost;
  });
}

/**
 * @brief print the memory and time cost of the function
 */
constexpr auto print_fn_cost = [](const fn_cost_t &cost) {
  console::log("cpu_time: {}\n", cost.cpu_time);
  console::log("wall_time: {}\n", cost.wall_time);
  console::log("memory: {}\n", cost.memory);
  return 0;
};

} // namespace scio

#endif // scio_FN_COST_H