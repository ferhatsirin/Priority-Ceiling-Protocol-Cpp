#include <cstdlib>
#include <iostream>
#include "mutex.h"
#include "PortableThread.h"
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

using namespace std;
using namespace gtu;

using CLOCK = chrono::high_resolution_clock;
using TimePoint = CLOCK::time_point;

struct threadParam {
    gtu::mutex s1;
    gtu::mutex s2;
};

struct threadParam2 {
    gtu::mutex s1;
    gtu::mutex s2;
    gtu::mutex s3;
};

void setPriority(int priority) {
    ThreadId t1 = getCurrentThread();
    setThreadPriority(t1, priority);
}

TimePoint startTime = CLOCK::now();

string getTime() {
    long int time = chrono::duration_cast<chrono::microseconds>(CLOCK::now() - startTime).count();
    return to_string(time) + " us";
}

std::mutex m;

void safePrint(string str) {
    lock_guard<std::mutex> lk(m);
    cout << str << endl;
}

void* f1(void* arg) {

    threadParam* param = (threadParam*) arg;

    setPriority(10);
    param->s1.registerThread();
    param->s2.registerThread();

    safePrint("A try to get S1 at " + getTime());
    unique_lock<gtu::mutex> lk1(param->s1);
    safePrint("A lock S1 at " + getTime());

    this_thread::sleep_for(chrono::microseconds(200));

    safePrint("A try to get S2 at " + getTime());
    unique_lock<gtu::mutex> lk2(param->s2);
    safePrint("A lock S2 at " + getTime());


    safePrint("A unlock S2 at " + getTime());
    lk2.unlock();

    safePrint("A unlock S1 at " + getTime());
    lk1.unlock();

    return 0;
}

void* f2(void* arg) {
    threadParam* param = (threadParam*) arg;

    setPriority(9);
    param->s1.registerThread();
    param->s2.registerThread();

    safePrint("B try to get S2 at " + getTime());
    unique_lock<gtu::mutex> lk2(param->s2);
    safePrint("B lock S2 at " + getTime());

    this_thread::sleep_for(chrono::microseconds(200));

    safePrint("B try to get S1 at " + getTime());
    unique_lock<gtu::mutex> lk1(param->s1);
    safePrint("B lock S1 at " + getTime());


    safePrint("B unlock S1 at " + getTime());
    lk1.unlock();

    safePrint("B unlock S2 at " + getTime());
    lk2.unlock();

    return 0;
}

void* f3(void * args) {

    threadParam2* param = (threadParam2*) args;

    setPriority(10);
    param->s1.registerThread();

    safePrint("A try to get S1 at " + getTime());
    unique_lock<gtu::mutex> lk1(param->s1);
    safePrint("A lock S1 at " + getTime());

    safePrint("A unlock S1 at " + getTime());
    lk1.unlock();

    return 0;
}

void* f4(void * args) {
    threadParam2* param = (threadParam2*) args;

    setPriority(9);
    param->s2.registerThread();
    param->s3.registerThread();

    safePrint("B try to lock S2 at " + getTime());
    unique_lock<gtu::mutex> lk2(param->s2);
    safePrint("B lock S2 at " + getTime());

    this_thread::sleep_for(chrono::microseconds(200));

    safePrint("B try to lock S3 at " + getTime());
    unique_lock<gtu::mutex> lk3(param->s3);
    safePrint("B lock S3 at " + getTime());


    safePrint("B unlock S3 at " + getTime());
    lk3.unlock();

    safePrint("B unlock S2 at " + getTime());
    lk2.unlock();

    return 0;
}

void* f5(void * args) {

    threadParam2* param = (threadParam2*) args;

    setPriority(8);
    param->s2.registerThread();
    param->s3.registerThread();

    safePrint("C try to lock S3 at " + getTime());
    unique_lock<gtu::mutex> lk3(param->s3);
    safePrint("C lock S3 at " + getTime());

    this_thread::sleep_for(chrono::microseconds(200));

    safePrint("C try to lock S2 at " + getTime());
    unique_lock<gtu::mutex> lk2(param->s2);
    safePrint("C lock S2 at " + getTime());


    safePrint("C unlock S2 at " + getTime());
    lk2.unlock();

    safePrint("C unlock S3 at " + getTime());
    lk3.unlock();

    return 0;
}

int main(int argc, char** argv) {

    ThreadId t1, t2;
    threadParam param;

    cout << "First Test : " << endl;
    cout << "Task | Priority | Locks" << endl;
    cout << "A        10        S1  S2" << endl;
    cout << "B         9        S2  S1" << endl;
    cout << "First test is starting" << endl << endl;
    startTime = CLOCK::now();
    t1 = createThread(f1, &param);
    t2 = createThread(f2, &param);

    threadJoin(t1);
    threadJoin(t2);

    ThreadId t3, t4, t5;
    threadParam2 param2;

    cout << endl << "Second Test :" << endl;
    cout << "Task | Priority | Locks" << endl;
    cout << "A        10        S1 " << endl;
    cout << "B         9        S2  S3" << endl;
    cout << "C         8        S3  S2" << endl;
    cout << "Second test is starting" << endl << endl;
    startTime = CLOCK::now();
    t3 = createThread(f3, &param2);
    t4 = createThread(f4, &param2);
    t5 = createThread(f5, &param2);

    threadJoin(t3);
    threadJoin(t4);
    threadJoin(t5);


    return 0;
}

