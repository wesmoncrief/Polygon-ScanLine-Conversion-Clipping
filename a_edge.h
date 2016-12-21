//
// Created by Wesley Moncrief on 2/13/16.
//

#ifndef POLYGON_CLIPPING_A_EDGE_H
#define POLYGON_CLIPPING_A_EDGE_H

#include <utility>

using namespace std;
typedef pair<int, int> coordinate;

class a_edge {
public:
    int max_y; // highest y-value
    //todo: should max_y be coordinates relative the shape or relative to the origin?
    float current_x; // x-value of endpoint with lowest y-value
    float x_incr; // 1/slope
    a_edge(pair<coordinate, coordinate> c_pair) {
        max_y = find_max_y(c_pair.first, c_pair.second);
        current_x = find_curr_x(c_pair.first, c_pair.second);
        x_incr = find_x_incr(c_pair.first, c_pair.second);
    };

    int find_max_y(coordinate curr, coordinate next);

    int find_curr_x(coordinate curr, coordinate next);

    float find_x_incr(coordinate curr, coordinate next);

};

#endif //POLYGON_CLIPPING_A_EDGE_H
