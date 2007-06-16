#include "job.h"
#include "jobqueue.h"
#include <iostream>
using namespace std;

class SleepJob : public Job {
private:
    int i, n;
public:
    SleepJob(int nn) :Job(nn), n(nn) {i=100000000;}
    ~SleepJob() {}
    void run() {}
    void stop() {i=0;}
};

int
main(int argc, char** argv) {
    JobQueue queue(10);
    for (int i=0; i<200000; ++i) {
        queue.addJob(new SleepJob(i));
    }
    return 0;
}
