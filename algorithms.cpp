//
// Created by Wesley Moncrief on 2/12/16.
//
#include <utility>
#include <vector>
#include <list>
#include "a_edge.h"

using namespace std;

typedef pair<int, int> coordinate;
typedef pair<coordinate, coordinate> edge;


vector<edge> make_clip_rectangle(vector<coordinate> clip_points) {
    //the clipedges should present as follows: left, top, right, bottom

    vector<edge> edges(4);
    vector<coordinate> pts(4);
    //this is all necessary so position_code works for the is_inside function
    if (clip_points[0].first > clip_points[1].first) {//then clip_points[0] is on the right
        if (clip_points[0].second > clip_points[1].second) {//clip_points[0] is on top right
            pts[0] = clip_points[1];
            pts[1] = make_pair(clip_points[1].first, clip_points[0].second);
            pts[2] = clip_points[0];
            pts[3] = make_pair(clip_points[0].first, clip_points[1].second);
        }
        else { //clip_points[0] is on bottom right
            pts[0] = make_pair(clip_points[1].first, clip_points[0].second);
            pts[1] = clip_points[1];
            pts[2] = make_pair(clip_points[0].first, clip_points[1].second);
            pts[3] = clip_points[0];
        }
    }
    else { //clip_points[0] is on the left
        if (clip_points[0].second > clip_points[1].second) {//clipPoints[0] is on top left
            pts[0] = make_pair(clip_points[0].first, clip_points[1].second);
            pts[1] = clip_points[0];
            pts[2] = make_pair(clip_points[1].first, clip_points[0].second);
            pts[3] = clip_points[1];
        }
        else { //clip_points[0] is on bottom left
            pts[0] = clip_points[0];
            pts[1] = make_pair(clip_points[0].first, clip_points[1].second);
            pts[2] = clip_points[1];
            pts[3] = make_pair(clip_points[1].first, clip_points[0].second);
        }

    }
    for (int i = 0; i < 4; ++i) {
        edges[i] = make_pair(pts[i], pts[(i + 1) % 4]);
    }
    //the clipedges should present as follows: left, top, right, bottom
    return edges;

}

bool is_inside(coordinate c, edge clip_edge, int position_code) {
    //position code is as follows, 0 indexed: left, top, right, bottom
    if (position_code == 0) {
        return c.first > clip_edge.first.first;
    }
    if (position_code == 1) {
        return c.second < clip_edge.first.second;
    }
    if (position_code == 2) {
        return c.first < clip_edge.first.first;
    }
    if (position_code == 3) {
        return c.second > clip_edge.first.second;
    }
}


coordinate intersection(coordinate c1, coordinate c2, edge _edge) {
    coordinate c3 = _edge.first;
    coordinate c4 = _edge.second;
    int x1 = c1.first;
    int y1 = c1.second;
    int x2 = c2.first;
    int y2 = c2.second;
    int x3 = c3.first;
    int y3 = c3.second;
    int x4 = c4.first;
    int y4 = c4.second;

    float determinant = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    coordinate ans;
    ans.first = (1 / determinant) * ((x3 - x4) * (x1 * y2 - y1 * x2) - (x1 - x2) * (x3 * y4 - y3 * x4));
    ans.second = (1 / determinant) * ((y3 - y4) * (x1 * y2 - y1 * x2) - (y1 - y2) * (x3 * y4 - y3 * x4));
    return ans;
}

//sutherland-hodgman, roughly follows sourcecode from wikipedia
vector<coordinate> s_hodgman(vector<coordinate> polygon_vertices, vector<coordinate> highlight_corners) {

    vector<coordinate> empty_vec; //for when the shape is completely outside box
    //create vector with all 4 rectangle coordinates
    vector<edge> clip_edges = make_clip_rectangle(highlight_corners);

    //for each edge
    for (int edge_index = 0; edge_index < clip_edges.size(); ++edge_index) {
        //the clipedges should present as follows: left, top, right, bottom
        //edge clip_edge = clip_edges[i];
        vector<coordinate> input_vertices(polygon_vertices);
        polygon_vertices.clear();
        if (input_vertices.size() == 0)
            return empty_vec; //if shape is totally outside box
        coordinate s = input_vertices.back();

        //for each point e, e in input_vertices
        for (int e = 0; e < input_vertices.size(); ++e) {
            //coordinate e = input_vertices[e];
            if (is_inside(input_vertices[e], clip_edges[edge_index], edge_index)) { //e is inside of edge edge_index
                if (!is_inside(s, clip_edges[edge_index], edge_index)) {//s outside of edge edge_index
                    polygon_vertices.push_back(intersection(s, input_vertices[e], clip_edges[edge_index]));
                }
                polygon_vertices.push_back(input_vertices[e]);
            }
            else if (is_inside(s, clip_edges[edge_index], edge_index)) { //s inside of edge edge_index
                polygon_vertices.push_back(intersection(s, input_vertices[e], clip_edges[edge_index]));
            }
            s = input_vertices[e];
        }
    }
    return polygon_vertices;

}


//the following are used as helpers in the drawings
int find_min_y(coordinate curr, coordinate next) {
    if (curr.second > next.second) return next.second;
    else return curr.second;
}

vector<list<pair<coordinate, coordinate> > > fill_sorted_edge_table(vector<coordinate> vertices, int image_h) {
    //need to "initialize" the sorted_edge_table
    vector<list<pair<coordinate, coordinate> > > s_edge_tab(image_h);

    //for each point pair of points, aka 'edge'
    for (int points_index = 0; points_index < vertices.size(); ++points_index) {

        //for each "edge" find the edge's lowest y-value's height above the polygon's lowest y-value
        coordinate curr_coord = vertices[points_index];
        coordinate next_coord = vertices[(points_index + 1) % vertices.size()];
        if (curr_coord.second != next_coord.second) { //don't add it if there is a horizontal line
            int min_y = find_min_y(curr_coord, next_coord);
            s_edge_tab[min_y].push_back(make_pair(curr_coord, next_coord));
        }

    }
    return s_edge_tab;
}

bool compare_curr_x(a_edge edge_one, a_edge edge_two) {
    return (edge_one.current_x < edge_two.current_x);
}

list<a_edge> sort_a_edge_list_by_curr_x(list<a_edge> l) {
    l.sort(compare_curr_x);
    return l;
}

int x_min(vector<coordinate> h) {
    if (h[0].first < h[1].first) return h[0].first;
    else return h[1].first;
}

int x_max(vector<coordinate> h) {
    if (h[0].first > h[1].first) return h[0].first;
    else return h[1].first;
}

int y_min(vector<coordinate> h) {
    if (h[0].second < h[1].second) return h[0].second;
    else return h[1].second;
}

int y_max(vector<coordinate> h) {
    if (h[0].second > h[1].second) return h[0].second;
    else return h[1].second;
}