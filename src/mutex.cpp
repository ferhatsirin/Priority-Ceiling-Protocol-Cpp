#include "mutex.h"
#include <iostream>
using namespace std;
namespace gtu {
    list<pair<ThreadId, gtu::mutex*> > mutex::lockedMutexes;
    std::mutex mutex::uniqMutex;
    std::mutex mutex::lockMutex;

    mutex::mutex()noexcept {

    }

    mutex::~mutex() {

    }

    pair<ThreadId, gtu::mutex*> mutex::getHighestCeilingMutex(ThreadId id) {
        lock_guard<std::mutex> g(lockMutex);
        int highest = -1;
        pair<ThreadId, gtu::mutex*> max = pair<ThreadId, gtu::mutex*>(0, nullptr);

        for (list<pair < ThreadId, gtu::mutex*>>::iterator iter = lockedMutexes.begin(); iter != lockedMutexes.end(); iter++) {
            if (!isEqual((*iter).first, id) && (*iter).second->getCeiling() > highest) {
                highest = (*iter).second->getCeiling();
                max = (*iter);
            }
        }

        return max;
    }

    void mutex::addLockedMutex(pair<ThreadId, gtu::mutex*> p) {
        lock_guard<std::mutex> g(lockMutex);
        lockedMutexes.push_back(p);
    }

    void mutex::removeLockedMutex(pair<ThreadId, gtu::mutex*> p) {
        lock_guard<std::mutex> g(lockMutex);
        lockedMutexes.remove_if([&p](pair<ThreadId, gtu::mutex*> e) {
            return isEqual(e.first, p.first) && (e.second == p.second);
        });
    }

    void mutex::registerThread() {
        lock_guard<std::mutex> g(regMutex);

        ThreadId threadId = getCurrentThread();
        registeredThreads.push_back(threadId);

        int priority = getThreadPriority(threadId);
        if (priority > ceiling) {
            ceiling = priority;
        }
    }

    void mutex::lock() {

        ThreadId id = getCurrentThread();

        if (isRegistered(id)) {
            unique_lock<std::mutex> lk(uniqMutex);

            int myPriority = getThreadPriority(id);
            pair<ThreadId, gtu::mutex*> highest = getHighestCeilingMutex(id);
            if (highest.second == nullptr || myPriority > highest.second->getCeiling()) {
                realMutex.lock();
                addLockedMutex(make_pair(id, this));
                lk.unlock();
            } else {
                lk.unlock();
                ThreadId highestThread = highest.first;
                gtu::mutex* highestMutex = highest.second;
                int lastP = getThreadPriority(highestThread);
                if (myPriority > lastP) {
                    setThreadPriority(highestThread, myPriority);
                }

                highestMutex->realMutex.lock();
                setThreadPriority(highestThread, lastP);
                highestMutex->realMutex.unlock();
                return lock();
            }
        } else {
            throw runtime_error("Thread is not registered!");
        }
    }

    bool mutex::try_lock() {
        ThreadId id = getCurrentThread();
        bool res = false;

        if (isRegistered(id)) {
            unique_lock<std::mutex> lk(uniqMutex);
            int priority = getThreadPriority(id);
            pair<ThreadId, gtu::mutex*> highest = getHighestCeilingMutex(id);
            if (highest.second == nullptr || priority > highest.second->getCeiling()) {
                res = realMutex.try_lock();
                if (res) {
                    addLockedMutex(make_pair(id, this));
                }
            }
            lk.unlock();
        } else {
            throw runtime_error("Thread is not registered!");
        }

        return res;
    }

    void mutex::unlock() {

        ThreadId id = getCurrentThread();
        removeLockedMutex(make_pair(id, this));
        realMutex.unlock();
    }

    bool mutex::isRegistered(ThreadId id) {
        lock_guard<std::mutex> g(regMutex);

        for (unsigned int i = 0; i < registeredThreads.size(); ++i) {
            if (isEqual(registeredThreads[i], id)) {
                return true;
            }
        }
        return false;
    }

    int mutex::getCeiling() {
        return ceiling;
    }

}
