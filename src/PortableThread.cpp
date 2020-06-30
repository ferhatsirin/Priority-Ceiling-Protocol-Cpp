#include "PortableThread.h"

#ifdef __linux__

namespace gtu {

    bool isEqual(ThreadId id1, ThreadId id2) {
        if (pthread_equal(id1, id2) != 0) {
            return true;
        } else {
            return false;
        }
    }

    ThreadId getCurrentThread() {
        return pthread_self();
    }

    int getThreadPriority(ThreadId id) {
        sched_param param;
        int policy;
        pthread_getschedparam(id, &policy, &param);

        return param.sched_priority;
    }

    void setThreadPriority(ThreadId id, int priority) {
        sched_param param;
        param.sched_priority = priority;
        pthread_setschedparam(id, SCHED_RR, &param);
    }

    ThreadId createThread(void* (*start_routine) (void*), void* arg) {
        pthread_t t;
        pthread_create(&t, nullptr, start_routine, arg);

        return t;
    }

    void threadJoin(ThreadId id) {
        pthread_join(id, nullptr);
    }

}

#elif _WIN32

namespace gtu {

    bool isEqual(ThreadId id1, ThreadId id2) {
        if (GetThreadId(id1) == GetThreadId(id2)) {
            return true;
        } else {
            return false;
        }
    }

    ThreadId getCurrentThread() {
        HANDLE h;
        DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),&h,DUPLICATE_SAME_ACCESS,true,DUPLICATE_SAME_ACCESS);
        return h;
    }

    int getThreadPriority(ThreadId id) {
        return GetThreadPriority(id);
    }

    void setThreadPriority(ThreadId id, int priority) {
        if (priority == 10) {

            priority = THREAD_PRIORITY_HIGHEST;
        } else if (priority == 9) {

            priority = THREAD_PRIORITY_ABOVE_NORMAL;
        } else {

            priority = THREAD_PRIORITY_NORMAL;
        }

        SetThreadPriority(id, priority);
    }

    ThreadId createThread(void* (*start_routine) (void*), void* arg) {
        HANDLE t;
        t = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) start_routine, (LPVOID) arg, 0, nullptr);
        return t;
    }

    void threadJoin(ThreadId id) {
        WaitForSingleObject(id, INFINITE);
    }

}

#endif
