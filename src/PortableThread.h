#ifndef PORTABLETHREAD_H
#define PORTABLETHREAD_H

#ifdef __linux__

#include <pthread.h>
namespace gtu {
    using ThreadId = pthread_t;
}

#elif _WIN32

#include <windows.h>
namespace gtu {
    using ThreadId = HANDLE;
}

#endif


namespace gtu {
    ThreadId createThread(void* (*start_routine) (void*), void* arg);
    bool isEqual(ThreadId id1, ThreadId id2);
    ThreadId getCurrentThread();
    int getThreadPriority(ThreadId id);
    void setThreadPriority(ThreadId id, int priority);
    void threadJoin(ThreadId id);
}

#endif /* PORTABLETHREAD_H */
