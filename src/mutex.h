#ifndef MUTEX_H
#define MUTEX_H

#include "PortableThread.h"
#include <list>
#include <vector>
#include <mutex>

using namespace std;

namespace gtu {

    class mutex {
    public:
        mutex() noexcept;
        ~mutex();
        mutex(const mutex&) = delete;
        mutex& operator=(const mutex&) = delete;

        void lock();
        bool try_lock();
        void unlock();
        void registerThread();

    private:
        bool isRegistered(ThreadId id);
        int getCeiling();
        pair<ThreadId, gtu::mutex*> getHighestCeilingMutex(ThreadId id);
        void addLockedMutex(pair<ThreadId, gtu::mutex*> p);
        void removeLockedMutex(pair<ThreadId, gtu::mutex*> p);

        vector<ThreadId> registeredThreads;
        std::mutex regMutex;
        std::mutex realMutex;
        int ceiling = 0;
        static std::mutex uniqMutex;
        static std::mutex lockMutex;
        static list<pair<ThreadId, gtu::mutex*> > lockedMutexes;
    };

}

#endif /* MUTEX_H */

