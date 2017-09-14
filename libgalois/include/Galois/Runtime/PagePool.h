/** heap building blocks -*- C++ -*-
 * @file
 * @section License
 *
 * This file is part of Galois.  Galoisis a framework to exploit
 * amorphous data-parallelism in irregular programs.
 *
 * Galois is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Galois is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Galois.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * @section Copyright
 *
 * Copyright (C) 2015, The University of Texas at Austin. All rights
 * reserved.
 *
 * @section Description
 *
 * Strongly inspired by heap layers:
 *  http://www.heaplayers.org/
 * FSB is modified from:
 *  http://warp.povusers.org/FSBAllocator/
 *
 * @author Andrew Lenharth <andrewl@lenharth.org>
 */
#ifndef GALOIS_RUNTIME_PAGEPOOL_H
#define GALOIS_RUNTIME_PAGEPOOL_H

#include "Galois/gIO.h"
#include "Galois/Substrate/SimpleLock.h"
#include "Galois/Substrate/PtrLock.h"
#include "Galois/Substrate/CacheLineStorage.h"
#include "Galois/Substrate/PageAlloc.h"
#include "Galois/Substrate/ThreadPool.h"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <numeric>
#include <deque>

#include <cstddef>

namespace Galois {
namespace Runtime {

//! Low level page pool (individual pages, use largeMalloc for large blocks)

void* pagePoolAlloc();
void pagePoolFree(void*);
void pagePoolPreAlloc(unsigned);

//Size of returned pages
size_t pagePoolSize();

//! Returns total large pages allocated by Galois memory management subsystem
int numPagePoolAllocTotal();
//! Returns total large pages allocated for thread by Galois memory management subsystem
int numPagePoolAllocForThread(unsigned tid);

namespace internal {

struct FreeNode {
  FreeNode* next;
};
 
typedef Galois::Substrate::PtrLock<FreeNode> HeadPtr;
typedef Galois::Substrate::CacheLineStorage<HeadPtr> HeadPtrStorage;

// Tracks pages allocated
template <typename _UNUSED=void>
class PageAllocState {  
  std::deque<std::atomic<int>> counts;
  std::vector<HeadPtrStorage> pool;
  std::unordered_map<void*, int> ownerMap;
  Galois::Substrate::SimpleLock mapLock;

  void* allocFromOS() {
    void* ptr = Galois::Substrate::allocPages(1, true);
    assert(ptr);
    auto tid = Galois::Substrate::ThreadPool::getTID();
    counts[tid] += 1;
    std::lock_guard<Galois::Substrate::SimpleLock> lg(mapLock);
    ownerMap[ptr] = tid;
    return ptr;
  }

public:
  PageAllocState() { 
    auto num = Galois::Substrate::getThreadPool().getMaxThreads();
    counts.resize(num);
    pool.resize(num);
  }

  int count(int tid) const {
    return counts[tid];
  }

  int countAll() const {
    return std::accumulate(counts.begin(), counts.end(), 0);
  }

  void* pageAlloc() {
    auto tid = Galois::Substrate::ThreadPool::getTID();
    HeadPtr& hp = pool[tid].data;
    if (hp.getValue()) {
      hp.lock();
      FreeNode* h = hp.getValue(); 
      if (h) {
        hp.unlock_and_set(h->next);
        return h;
      }
      hp.unlock();
    }
    return allocFromOS();
  }

  void pageFree(void* ptr) {
    assert(ptr);
    mapLock.lock();
    assert(ownerMap.count(ptr));
    int i = ownerMap[ptr];
    mapLock.unlock();
    HeadPtr& hp = pool[i].data;
    hp.lock();
    FreeNode* nh = reinterpret_cast<FreeNode*>(ptr);
    nh->next = hp.getValue();
    hp.unlock_and_set(nh);
  }

  void pagePreAlloc() {
    pageFree(allocFromOS());
  }
};

//! Initialize PagePool, used by Runtime::init();
void setPagePoolState(PageAllocState<>* pa);

} // end namespace internal

} // end namespace Runtime
} // end namespace Galois

#endif
