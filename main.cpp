#include <GLUT/glut.h>
#include <utility>
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <thread>
#include "a_edge.h"

using namespace std;

/******************************************************************
	Notes:
	Image size is 400 by 400 by default.  You may adjust this if
		you want to.
	You can assume the window will NOT be resized.
	Call clearFramebuffer to clear the entire framebuffer.
	Call setFramebuffer to set a pixel.  This should be the only
		routine you use to set the color (other than clearing the
		entire framebuffer).  drawit() will cause the current
		framebuffer to be displayed.
	As is, your scan conversion should probably be called from
		within the display function.  There is a very short sample
		of code there now.
	You may add code to any of the subroutines here,  You probably
		want to leave the drawit, clearFramebuffer, and
		setFramebuffer commands alone, though
  *****************************************************************/

#define ImageW 400
#define ImageH 400
typedef pair<int, int> coordinate;

//******************* START GLOBAL DATA **************************************
bool input_phase = true; // this becomes false after 'c' is entered.
float framebuffer[ImageH][ImageW][3];

vector<vector<coordinate> > points; //up to 10 points, up to 10 points each. only as many elements as shapes, points

vector<vector<bool> > colored_pixels(400, vector<bool>(400));
int pixel_color;
float r[] = {.9, .92, .1, .6, .5, .4, .3, .8, .9, .1};
float g[] = {.2, .58, .23, .8, .3, .9, .2, .1, .6, .6};
float b[] = {.3, .39, .0, .5, .9, .1, .2, .2, .2, .9};

vector<coordinate> highlight(2);
//0 is original, 1 is second
bool draw_highlight = false;

//******************* END GLOBAL DATA **************************************
//******************* START ALGORITHM DECLARATIONS **********************
int x_min(vector<coordinate> h);

int x_max(vector<coordinate> h);

int y_min(vector<coordinate> h);

int y_max(vector<coordinate> h);

vector<list<pair<coordinate, coordinate> > > fill_sorted_edge_table(vector<coordinate> vertices, int image_h);

list<a_edge> sort_a_edge_list_by_curr_x(list<a_edge> a_edge_list);

vector<coordinate> s_hodgman(vector<coordinate> polygon_vertices, vector<coordinate> highlight_corners);

//******************* END ALGORITHM DECLARATIONS **************************

// Draws the scene
void drawit() {
    glDrawPixels(ImageW, ImageH, GL_RGB, GL_FLOAT, framebuffer);
    glFlush();
}

// Clears framebuffer to black
void clearFramebuffer() {
    int i, j;

    for (i = 0; i < ImageH; i++) {
        for (j = 0; j < ImageW; j++) {
            framebuffer[i][j][0] = 0.0;
            framebuffer[i][j][1] = 0.0;
            framebuffer[i][j][2] = 0.0;
        }
    }
}

// i changed this to put the origin on the lower left corner...
void setFramebuffer(int x, int y, float R, float G, float B) {
    // changes the origin from the lower-left corner to the upper-left corner
    //y = ImageH - 1 - y;
    if (R <= 1.0) if (R >= 0.0)
        framebuffer[y][x][0] = R;
    else
        framebuffer[y][x][0] = 0.0;
    else
        framebuffer[y][x][0] = 1.0;
    if (G <= 1.0) if (G >= 0.0)
        framebuffer[y][x][1] = G;
    else
        framebuffer[y][x][1] = 0.0;
    else
        framebuffer[y][x][1] = 1.0;
    if (B <= 1.0) if (B >= 0.0)
        framebuffer[y][x][2] = B;
    else
        framebuffer[y][x][2] = 0.0;
    else
        framebuffer[y][x][2] = 1.0;
}

//for color_code, use the index of points.
void poly_draw(vector<coordinate> vertices, int color_code) {

    vector<list<pair<coordinate, coordinate> > > s_edge_tab = fill_sorted_edge_table(vertices, ImageH);
    //sorted edge table
    list<a_edge> a_edge_list; //active edge list

    int line = 0;
    while (line < ImageH) { //algorithm for shape, instead of fill it adds data to global colored_pixels

        //this loop will "add edges to active edge list from sorted edge table starting at variable line"
        for (list<pair<coordinate, coordinate> >::iterator it = s_edge_tab[line].begin();
             it != s_edge_tab[line].end(); ++it) {
            a_edge_list.push_back(a_edge(*it));
        }

        //remove edges that end at line
        for (list<a_edge>::iterator it = a_edge_list.begin(); it != a_edge_list.end(); ++it) {
            if (it->max_y == line) {
                a_edge_list.erase(it);
            }
        }
        //sorts active edge list by current_x so that drawing will work properly
        a_edge_list = sort_a_edge_list_by_curr_x(a_edge_list);

        //fill pixels on current line
        //instead of this, we change color_pixels values to true
        list<a_edge>::iterator it = a_edge_list.begin();
        while (it != a_edge_list.end()) {
            for (double i = it->current_x; i < next(it)->current_x; ++i) {
                //this needs to be sorted by currentX
                colored_pixels[line][i] = true;
            }
            ++it;
            ++it;
        }

        //increment x-values on edges in active edge list
        it = a_edge_list.begin();
        while (it != a_edge_list.end()) {
            it->current_x = it->current_x + it->x_incr;
            ++it;
        }

        ++line;
    }

    pixel_color = color_code;
    glutPostRedisplay();


}


void display() {
    //for (int i = 0; i <= 100; i++) setFramebuffer(i, i * 1 / 2, 1.0, 1.0, 1.0);

    for (int i = 0; i < ImageH; ++i) {
        for (int j = 0; j < ImageW; ++j) {
            if (colored_pixels[i][j])
                setFramebuffer(j, i, r[pixel_color], g[pixel_color], b[pixel_color]);
        }
    }
    //reset colored_pixels
    for (int i = 0; i < ImageH; ++i) {
        for (int j = 0; j < ImageW; ++j) {
            colored_pixels[j][i] = false;
        }
    }

    //drawing highlighter box
    if (draw_highlight) { //used to prevent drawing this at the start of the show
        for (int i = x_min(highlight); i < x_max(highlight); ++i) {
            setFramebuffer(i, y_min(highlight), 1, 1, 1);
            setFramebuffer(i, y_max(highlight), 1, 1, 1);
        }
        for (int i = y_min(highlight); i < y_max(highlight); ++i) {
            setFramebuffer(x_min(highlight), i, 1, 1, 1);
            setFramebuffer(x_max(highlight), i, 1, 1, 1);
        }
        draw_highlight = false;
    }


    drawit();
}

void init() {
    clearFramebuffer();
}

int i = 0, j = 0;

//indices for vector. i describes which shape, j describes which point
void mouse(int button, int state, int x, int y) {
    y = ImageH - y; //puts the origin on the lower left corner...
    if (state == GLUT_DOWN) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                if (input_phase) {
                    if (i == 10 || j == 9 || input_phase == false) {
                        cout << "invalid shape point input.\n";
                        return;
                    }
                    if (j == 0) points.push_back(vector<coordinate>());
                    points[i].push_back(make_pair(x, y));
                    j += 1;
                }
                else { // this is where we highlight and clip
                    highlight[0] = make_pair(x, y);
                }
                break;
            case GLUT_RIGHT_BUTTON:
                if (i == 10 || j == 10 || j < 2 || input_phase == false) {
                    cout << "invalid shape point input.\n";
                    return;
                }
                points[i].push_back(make_pair(x, y));
                poly_draw(points[i], i);
                i += 1;
                j = 0;
                break;
        }
    }
    if (state == GLUT_UP && button == GLUT_LEFT_BUTTON && !input_phase) {
        clearFramebuffer();
        for (int shape_index = 0; shape_index < points.size(); ++shape_index) {
            vector<coordinate> clipped_points = s_hodgman(points[shape_index], highlight);
            for (int vertex_index = 0; vertex_index < clipped_points.size(); ++vertex_index) {
//                printf("points[%d][%d] = (%d,%d)\n", shape_index, vertex_index,
//                       clipped_points[vertex_index].first,
//                       clipped_points[vertex_index].second);
                poly_draw(clipped_points, shape_index);
                display();
            }
        }
    }


    glutPostRedisplay();
}

void motion(int x, int y) {
    y = ImageH - y;//y is normal coordinates
    if (x < 0 || x > ImageW || y < 0 || y > ImageH || input_phase) return; //out of bounds safety
    highlight[1] = make_pair(x, y);
    clearFramebuffer();
    for (int k = 0; k < points.size(); ++k) {
        if (k == points.size() - 1)
            draw_highlight = true;
        poly_draw(points[k], k);
        display();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'c':
            if (input_phase == true) { //only able to execute this once
                input_phase = false;
//                for (int shape_index = 0; shape_index < points.size(); ++shape_index) {
//                    for (int vertex_index = 0; vertex_index < points[shape_index].size(); ++vertex_index) {
//                        printf("points[%d][%d] = (%d,%d)\n", shape_index, vertex_index,
//                               points[shape_index][vertex_index].first,
//                               points[shape_index][vertex_index].second);
//                    }
//                }

                break;
            }

    }

}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(ImageW, ImageH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Wesley Moncrief - Polygon Clipping");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}
