#ifndef GETMINSTEP_H
#define GETMINSTEP_H
#include <string>
using namespace std;
struct Point
{
    int x, y;
    Point(){};
    Point(int a, int b) :x(a), y(b) {}
    bool operator==(const Point& obj) const
    {
        return obj.x == x&&obj.y == y;
    }
    bool operator<(const Point& obj) const
    {
        if (obj.x == x)
            return y < obj.y;
        return x < obj.x;
    }
    bool operator!=(const Point& obj) const
    {
        return !(*this == obj);
    }
};
int getMinStep(const string& s);
vector<Point> getNextMove(const int flags[9][9]);
#endif // GETMINSTEP_H
