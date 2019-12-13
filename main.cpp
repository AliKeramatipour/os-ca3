#include <iostream>
#include <ctime>
#include <map>
#include <string>
#include <thread>

using namespace std;

const int MAX_PATHS = 100;
const int MAX_CARS  = 100;

map< pair<string,string> , int > edges;
string paths[MAX_PATHS];

class Mointor{
public:
    void acquire()
    {
        latch.lock();
        
        latch.unlock();
    }
private:
    mutex latch;
};

void run_car(string my_path)
{

}

int main(int argc, char *argv[]) {

    while (1)
    {

        string inp, s[3];
        int cnt = 0;
        cin >> inp;
        if ( inp[0] == '#' ) break ;
        string tmp;
        for ( int i = 0 ; i < inp.size() ; i++ )
        {
            if ( inp[i] == '-' )
            {
                cnt++;
            } else {
                s[cnt].push_back(inp[i]);
            }
        }
        edges[{s[0], s[1]}] = stoi(s[2]);
    }
    thread cars[MAX_PATHS][MAX_CARS];
    for (int i = 0; true ;i++)
    {
        int car_cnt = 0;
        cin >> paths[i];
        cin >> car_cnt ;
        for (int j = 0 ; j < car_cnt ; j++ )
        {
            cars[i][j] = thread(run_car,paths[i]);
        }
    }

	return 0;
}