#include <iostream>
#include <queue>
#include <cstring>
#include <exception>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <fstream>
#include <algorithm>
#include "getminstep.h"
using namespace std;
enum { WALL, EMPTY, MAN, BOX, EMPTY_TARGET, BOX_AT_TARGET, MAN_AT_TARGET };

struct Status
{
    vector<Point> boxs;
    int manx;
    int many;
    int step;
    int id;
    bool operator<(const Status& obj) const
    {
        if(id!=obj.id)
            return id<obj.id;
        if (step != obj.step)
            return step < obj.step;
        if (manx != obj.manx)
            return manx < obj.manx;
        if (many != obj.many)
            return many < obj.many;
        int s1 = boxs.size();
        int s2 = obj.boxs.size();
        if (s1 != s2)
            return s1 < s2;
        for (int i = 0;i < s1;i++)
        {
            if (boxs[i] != obj.boxs[i])
                return boxs[i] < obj.boxs[i];
        }
        return false;
    }
};
int directions[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };
bool isConnect(const int data[9][9], const Status& s, int x, int y)
{
    int f[9][9];
    memset(f, 0, sizeof(f));
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (data[i][j] == WALL)
                f[i][j] = 1;
        }
    }
    for (size_t i = 0;i < s.boxs.size();i++)
    {
        f[s.boxs[i].x][s.boxs[i].y] = 1;
    }
    Point start(s.manx, s.many);
    queue<Point> q;
    q.push(start);
    f[start.x][start.y] = -1;
    while (!q.empty())
    {
        Point t = q.front();
        q.pop();
        if (t.x == x&&t.y == y)
            return true;
        for (int i = 0;i < 4;i++)
        {
            int nx, ny;
            nx = t.x + directions[i][0];
            ny = t.y + directions[i][1];
            if (nx >= 0 && nx < 9 && ny >= 0 && ny < 9 && f[nx][ny] == 0)
            {
                f[nx][ny] = -1;
                q.push(Point(nx, ny));
            }
        }
    }
    return false;
}
void normally(const int data[9][9], Status& s)
{
    int f[9][9];
    memset(f, 0, sizeof(f));
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (data[i][j] == WALL)
                f[i][j] = 1;
        }
    }
    for (size_t i = 0;i < s.boxs.size();i++)
    {
        f[s.boxs[i].x][s.boxs[i].y] = 1;
    }
    Point start(s.manx, s.many);
    queue<Point> q;
    q.push(start);
    f[start.x][start.y] = -1;
    while (!q.empty())
    {
        Point t = q.front();
        q.pop();
        for (int i = 0;i < 4;i++)
        {
            int nx, ny;
            nx = t.x + directions[i][0];
            ny = t.y + directions[i][1];
            if (nx >= 0 && nx < 9 && ny >= 0 && ny < 9 && f[nx][ny] == 0)
            {
                f[nx][ny] = -1;
                q.push(Point(nx, ny));
            }
        }
    }
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (f[i][j] == -1)
            {
                s.manx = i;
                s.many = j;
                return;
            }
        }
    }
    return;
}
bool isFinish(const int data[9][9], const Status& s)
{
    for (size_t i = 0;i < s.boxs.size();i++)
    {
        if (data[s.boxs[i].x][s.boxs[i].y] != EMPTY_TARGET)
            return false;
    }
    return true;
}
void show(Status& s)
{
    for (size_t i = 0;i < s.boxs.size();i++)
    {
        cout << "(" << s.boxs[i].x << "," << s.boxs[i].y << ")" << endl;
    }
    cout << "man: (" << s.manx << "," << s.many << ")" << endl;
    cout << "step = " << s.step << endl;
    cout << endl;
}
int getMinStep(const string& s)
{

    Status initState;
    initState.step = 0;
    int map[9][9];
    //int buf[9][9];
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            char t = s[i * 9 + j];
            int a;
            switch (t)
            {
            case '#':
                a = WALL;
                break;
            case '.':
                a = EMPTY;
                break;
            case 'H':
                a = EMPTY_TARGET;
                initState.boxs.push_back(Point(i, j));
                break;
            case 'M':
                a = MAN;
                initState.manx = i;
                initState.many = j;
                break;
            case 'B':
                a = BOX;
                initState.boxs.push_back(Point(i, j));
                break;
            case 'T':
                a = EMPTY_TARGET;
                break;
            default:
                return -1;
            }
            map[i][j] = a;
        }
    }
    normally(map,initState);
    set<Status> visited;
    queue<Status> q;
    q.push(initState);
    visited.insert(initState);
    while (!q.empty())
    {
        Status t = q.front();
        //show(t);
        Status newt;
        q.pop();
        if (isFinish(map, t))
        {
            return t.step;
        }
        for (size_t i = 0;i < t.boxs.size();i++)//choose a box to ready to move
        {
            Point pos = t.boxs[i];
            for (int j = 0;j < 4;j++)//choose a direction
            {
                int nx, ny, bx, by, px, py;
                nx = pos.x + directions[j][0];
                ny = pos.y + directions[j][1];
                px = pos.x - directions[j][0];
                py = pos.y - directions[j][1];
                bx = nx + directions[j][0];
                by = ny + directions[j][1];
                if (px >= 0 && px < 9 && py >= 0 && py < 9 && map[px][py] != WALL&&isConnect(map, t, px, py) && nx >= 0 && nx < 9 && ny >= 0 && ny < 9 && map[nx][ny] != WALL)//if the (nx,ny) is moveable and (px,py) can be man
                {
                    if (find(t.boxs.begin(), t.boxs.end(), Point(nx, ny)) == t.boxs.end())//if the (nx,ny) is not box
                    {
                        newt = t;
                        //newt.boxs.erase(remove(newt.boxs.begin(), newt.boxs.end(), pos), newt.boxs.end());
                        newt.manx = pos.x;
                        newt.many = pos.y;
                        newt.boxs[i].x = nx;
                        newt.boxs[i].y = ny;
                        newt.step++;
                        normally(map, newt);
                        if (!visited.count(newt))
                        {
                            visited.insert(newt);
                            q.push(newt);
                        }
                    }
                }
            }
        }
    }
    return -1;
}
vector<Point> getSecondTrace(map<int, int>& trace, int i, map<int, Status>& status)
{
    if (trace[i] != 0)
        return getSecondTrace(trace, trace[i], status);
    else
    {
        return status[i].boxs;
    }
}
vector<Point> getNextMove(const int flags[9][9])
{
    int map[9][9];
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            map[i][j] = flags[i][j];
        }
    }
    int idcnt = 0;
    std::map<int, Status> status;
    std::map<int, int> trace;
    trace[idcnt] = -1;
    Status initState;
    initState.id = idcnt;
    idcnt++;
    initState.step = 0;
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (map[i][j] == BOX || map[i][j] == BOX_AT_TARGET)
            {
                if(map[i][j] == BOX_AT_TARGET)
                    map[i][j] = EMPTY_TARGET;
                initState.boxs.push_back(Point(i, j));
            }
            if (map[i][j] == MAN || map[i][j] == MAN_AT_TARGET)
            {
                if(map[i][j] == MAN_AT_TARGET)
                    map[i][j] = EMPTY_TARGET;
                initState.manx = i;
                initState.many = j;
            }
        }
    }
    normally(map, initState);
    set<Status> visited;
    queue<Status> q;
    q.push(initState);
    status.insert(pair<int, Status>(initState.id, initState));
    visited.insert(initState);
    while (!q.empty())
    {
        Status t = q.front();
        Status newt;
        q.pop();
        show(t);
        if (isFinish(map, t))
        {
            return getSecondTrace(trace, t.id, status);
        }
        for (size_t i = 0;i < t.boxs.size();i++)//choose a box to ready to move
        {
            Point pos = t.boxs[i];
            for (int j = 0;j < 4;j++)//choose a direction
            {
                int nx, ny, bx, by, px, py;
                nx = pos.x + directions[j][0];
                ny = pos.y + directions[j][1];
                px = pos.x - directions[j][0];
                py = pos.y - directions[j][1];
                bx = nx + directions[j][0];
                by = ny + directions[j][1];
                if (px >= 0 && px < 9 && py >= 0 && py < 9 && map[px][py] != WALL&&isConnect(map, t, px, py) && nx >= 0 && nx < 9 && ny >= 0 && ny < 9 && map[nx][ny] != WALL)//if the (nx,ny) is moveable and (px,py) can be man
                {
                    if (find(t.boxs.begin(), t.boxs.end(), Point(nx, ny)) == t.boxs.end())//if the (nx,ny) is not box
                    {
                        newt = t;
                        newt.manx = pos.x;
                        newt.many = pos.y;
                        newt.boxs[i].x = nx;
                        newt.boxs[i].y = ny;
                        newt.step++;
                        normally(map, newt);
                        if (!visited.count(newt))
                        {
                            newt.id = idcnt;
                            trace[idcnt] = t.id;
                            idcnt++;
                            visited.insert(newt);
                            q.push(newt);
                            status.insert(pair<int, Status>(newt.id, newt));
                        }
                    }
                }
            }
        }
    }
    return vector<Point>();
}
