#include <iostream>
#include <ctime>
#include <map>
#include <string>
#include <thread>
#include <semaphore.h> 
#include <unistd.h> 
#include <pthread.h>
#include <stdio.h> 
#include <fstream>
#include <vector>
#include <fcntl.h>
#include <chrono>
#include <ctime>
#include <ratio>

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point tStart;

duration<double, std::milli> getTime()
{
    high_resolution_clock::time_point tNow = high_resolution_clock::now();
    duration<double, std::milli> time_span = tNow - tStart;
    return time_span;
}

class Monitor{
public:
    void intialize_sem(int cnt , int initH, string a, string b)
    {
        f = a ; s = b ;
        h = initH;
        string address = to_string(cnt);
        semaphore = sem_open(address.c_str(), O_CREAT, 0644, 1);
        //sem_init(&locks[cnt] , 0,1);
        //semaphore = &locks[cnt];
    }
    pair<long double, pair<duration<double, std::milli> ,duration<double, std::milli> > > acquireAndCalculate( int p )
    {
        long double res = 0 ;
        sem_wait(this -> semaphore);
        auto tEntrance = getTime();
        for (int i = 0 ; i < 1e7 ; i++)
        {
            res += (long double)i / (long double)(1e6 * h * p) ;
        }
        auto tExit = getTime();
        sem_post(this -> semaphore);
        return {res, {tEntrance, tExit}} ;
    }
private:
    sem_t *semaphore;
    string f , s ;
    int h ;
};