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

const int MAX_PATHS = 100;
const int MAX_CARS  = 100;
const int MAX_EDGES = 100;

high_resolution_clock::time_point tStart;
sem_t locks[MAX_EDGES];

duration<double, std::milli> getTime()
{
    high_resolution_clock::time_point tNow = high_resolution_clock::now();
    duration<double, std::milli> time_span = tNow - tStart;
    return time_span;
}
class Monitor{
public:
    void intialize_sem(int cnt , int initH)
    {
        h = initH;
        string address = "/s" + to_string(cnt);
        semaphore = sem_open(address.c_str(), O_CREAT, 0644, 1);
        //sem_init(&locks[cnt] , 0,1);
        //semaphore = &locks[cnt];
    }
    pair<long double, pair<duration<double, std::milli> ,duration<double, std::milli> > > acquireAndCalculate( int p )
    {
        long double res = 0 ;
        /*
        int SEMRES = 0;
        cout << " here " << sem_getvalue(semaphore, &SEMRES) << " :: " << SEMRES << endl ;
        cout << "locking on semaphore" << endl ;*/
        sem_wait(semaphore);
        auto tEntrance = getTime();
        for (int i = 0 ; i < 1e7 ; i++)
        {
            res += i / (1e6 * h * p) ;
        }
        auto tExit = getTime();
        sem_post(semaphore);
        return {res, {tEntrance, tExit}} ;
    }
private:
    sem_t *semaphore;
    int h ;
};

vector<string> paths[MAX_PATHS];
Monitor pMonitor[MAX_EDGES];
map< pair<string,string> , Monitor* > edges;
int pCars[MAX_PATHS][MAX_CARS];
long double total_emission;
vector<pair<int,int> > thread_joins;
sem_t *total_emission_lock ;

void run_car(int pathNumber, int carNumber)
{
    ofstream outFile;
    outFile.open(to_string(pathNumber) + "-" + to_string(carNumber) + ".txt");

    for ( int i = 0 ; i < paths[pathNumber].size() - 1 ; i++)
    {
        pair<string,string> inout ; inout.first = paths[pathNumber][i] ; inout.second = paths[pathNumber][i + 1];
        Monitor* thisEdge = edges[inout];
        auto result = thisEdge -> acquireAndCalculate(pCars[pathNumber][carNumber]);

        sem_wait(total_emission_lock);
        total_emission++;
        //total_emission = total_emission + result.first ;
        sem_post(total_emission_lock);

        outFile << paths[pathNumber][i] << " " << result.second.first.count() << " " << paths[pathNumber][i + 1] << " " << result.second.second.count() << " " << result.first << " " << total_emission << endl;
        cout << pathNumber << " : " << carNumber << endl << paths[pathNumber][i] << " " << result.second.first.count() << " " << paths[pathNumber][i + 1] << " " << result.second.second.count() << " " << result.first << " " << total_emission << endl;

    }
    outFile.close();
}

void pathDecompose(int num)
{
    string tmp;
    string initial = paths[num][0];
    initial.push_back('-');
    paths[num].clear();
    for (int i = 0 ; i < initial.size() ; i++)
    {
        if ( initial[i] == '-' ){
            paths[num].push_back(tmp);
            tmp.clear();
        }else
            tmp.push_back(initial[i]);
    }
}

int main(int argc, char *argv[]) {
    tStart = high_resolution_clock::now();
    total_emission_lock = sem_open("/total_emission_lock", O_CREAT, 0644, 1);

    ifstream inputFile;
    inputFile.open("input.txt");
    int edgeCnt = 0;
    while (1)
    {
        string inp, s[3];
        int cnt = 0;
        inputFile >> inp;
        if ( inp[0] == '#' ) break ;
        for ( int i = 0 ; i < inp.size() ; i++ )
        {
            if (inp[i] == '-')
                cnt++;
            else
                s[cnt].push_back(inp[i]);
        }
        edges[{s[0], s[1]}] = &pMonitor[edgeCnt];
        pMonitor -> intialize_sem(edgeCnt++, stoi(s[2]));
    }

    thread cars[MAX_PATHS][MAX_CARS];
    int carNumber = 0;
    int i = 0;
    string pathNodes;
    while ( inputFile >> pathNodes )
    {
        int car_cnt = 0;
        if ( pathNodes[0] == '#' ) break;

        paths[i].push_back(pathNodes);
        pathDecompose(i);
        inputFile >> car_cnt ;
        for (int j = 0 ; j < car_cnt ; j++ )
        {
            pCars[i][j] = rand() % 10 + 1;
            cars[i][j] = thread(run_car,i,j);
            thread_joins.push_back({i,j});
        }
        i++;
    }

    for ( int i = 0 ; i < thread_joins.size() ; i++ )
        cars[thread_joins[i].first][thread_joins[i].second].join();

    

	return 0;
}