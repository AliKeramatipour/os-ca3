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

time_t getTime()
{
    time_t result = std::time(nullptr);
    return result;
}

class Monitor{
public:
    void intialize_sem(int cnt , int initH, string a, string b)
    {
        f = a ; s = b ;
        h = initH;
        string address = to_string(cnt);
        semaphore = sem_open(address.c_str(), O_CREAT, 0644, 1);
    }
    pair<long double, pair<time_t ,time_t > > acquireAndCalculate( int p )
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