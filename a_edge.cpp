//
// Created by Wesley Moncrief on 2/13/16.
//

#include "a_edge.h"

int a_edge::find_max_y(coordinate curr, coordinate next) {
    if (curr.second < next.second) return next.second;
    else return curr.second;
}

int a_edge::find_curr_x(coordinate curr, coordinate next) {
    if (curr.second > next.second) return next.first;
    else return curr.first;
}

float a_edge::find_x_incr(coordinate curr, coordinate next) {
    //m = (y2-y1)/(x2-x1)
    double y2 = (double) next.second;
    double y1 = (double) curr.second;
    double x2 = (double) next.first;
    double x1 = (double) curr.first;
    double m = (y2 - y1) / (x2 - x1);
    return (float) (1 / m);
}