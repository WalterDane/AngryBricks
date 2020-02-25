#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// Global constants
#define MIN_X_VIEW -50
#define MAX_X_VIEW 50
#define MIN_Y_VIEW -50
#define MAX_Y_VIEW 50
#define MIN_Z_VIEW -50
#define MAX_Z_VIEW 50
#define MIN_X_SCREEN 0
#define MAX_X_SCREEN 500
#define MIN_Y_SCREEN 0
#define MAX_Y_SCREEN 500
#define SLEEP_TIME 5
#define CUBE_SIZE 6

// Global variables
GLenum mode = GL_POLYGON;

enum direction { Up, Down }dir;

//Vertex globals for cube
float x = 0.0;
float y = 0.0;
const float z = 0.0;
const int CUBE_RADIUS = CUBE_SIZE / 2;

//Globals to start the start and end points of a click and drag
float start_point[2] = {0, 0};
float end_point[2] = {0, 0};

//Movement Globals
float horizontal_velocity = 0.0;
float vertical_velocity = 0.0;
float gravity = 0.0019;
float rotate_cube = 0.0;
float highest_height = 0.0; //maximum height the cube reaches on an upwards trajection. used for bounce physics
float cor = 0.0; //coefficient of restitution
float bounce_height = 0.0; //used to store the height after each bounce

//Booleans: keep track of start of click, end of click, and if the cube hit a border
bool click_start = false;
bool click_finish = false;
bool up = false;
bool bounced = false;

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(MIN_X_VIEW, MAX_X_VIEW,
        MIN_Y_VIEW, MAX_Y_VIEW,
        MIN_Z_VIEW, MAX_Z_VIEW);

    glEnable(GL_DEPTH_TEST);
}

//---------------------------------------
// Function to draw 3D cube
//---------------------------------------
void cube(float midx, float midy, float midz, float size)
{
    // Define 8 vertices
    float ax = midx - size / 2;
    float ay = midy - size / 2;
    float az = midz + size / 2;
    float bx = midx + size / 2;
    float by = midy - size / 2;
    float bz = midz + size / 2;
    float cx = midx + size / 2;
    float cy = midy + size / 2;
    float cz = midz + size / 2;
    float dx = midx - size / 2;
    float dy = midy + size / 2;
    float dz = midz + size / 2;
    float ex = midx - size / 2;
    float ey = midy - size / 2;
    float ez = midz - size / 2;
    float fx = midx + size / 2;
    float fy = midy - size / 2;
    float fz = midz - size / 2;
    float gx = midx + size / 2;
    float gy = midy + size / 2;
    float gz = midz - size / 2;
    float hx = midx - size / 2;
    float hy = midy + size / 2;
    float hz = midz - size / 2;

    // Draw 6 faces
    //Front
    glBegin(mode);
    glColor3f(1.0, 0.0, 0.0);  glVertex3f(ax, ay, az);
    glColor3f(0.0, 1.0, 0.0); glVertex3f(bx, by, bz);
    glColor3f(0.0, 0.0, 1.0); glVertex3f(cx, cy, cz);
    glColor3f(1.0, 0.0, 1.0); glVertex3f(dx, dy, dz);
    glEnd();

    //Back
    glBegin(mode);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(ax, ay, az);
    glVertex3f(dx, dy, dz);
    glVertex3f(hx, hy, hz);
    glVertex3f(ex, ey, ez);
    glEnd();

    //Right
    glBegin(mode);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(ax, ay, az);
    glVertex3f(ex, ey, ez);
    glVertex3f(fx, fy, fz);
    glVertex3f(bx, by, bz);
    glEnd();

    //Left
    glBegin(mode);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(gx, gy, gz);
    glVertex3f(fx, fy, fz);
    glVertex3f(ex, ey, ez);
    glVertex3f(hx, hy, hz);
    glEnd();

    //Top
    glBegin(mode);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(gx, gy, gz);
    glVertex3f(cx, cy, cz);
    glVertex3f(bx, by, bz);
    glVertex3f(fx, fy, fz);
    glEnd();

    //Bottom
    glBegin(mode);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(gx, gy, gz);
    glVertex3f(hx, hy, hz);
    glVertex3f(dx, dy, dz);
    glVertex3f(cx, cy, cz);
    glEnd();

}

void reset()
{
    click_start = true;
    click_finish = false;
    highest_height = 0.0;
}
//---------------------------------------
// Mouse callback for OpenGL
//---------------------------------------
void mouse(int button, int state, int local_x, int local_y)
{
    // Calculate scale factors
    float x_scale = (MAX_X_VIEW - MIN_X_VIEW) /
        (float)(MAX_X_SCREEN - MIN_X_SCREEN);
    float y_scale = (MIN_Y_VIEW - MAX_Y_VIEW) /
        (float)(MAX_Y_SCREEN - MIN_Y_SCREEN);

    //If the mouse is held down...
    if (state == GLUT_DOWN)
    {
        //Used in the timer function
        //Reset when the user clicks again
        reset();

        //Reset velocities
        horizontal_velocity = 0.0;
        vertical_velocity = 0.0;

        x = MIN_X_VIEW + (local_x - MIN_X_SCREEN) * x_scale;
        y = MAX_Y_VIEW + (local_y - MIN_Y_SCREEN) * y_scale;

        //Get the (x1, y1) location where the mouse was clicked
        start_point[0] = x;
        start_point[1] = y;
    }
    else {

        //Used in the timer function 
        click_start = false;
        click_finish = true;

        x = MIN_X_VIEW + (local_x - MIN_X_SCREEN) * x_scale;
        y = MAX_Y_VIEW + (local_y - MIN_Y_SCREEN) * y_scale;

        //Get the (x2, y2) location where the mouse was released
        end_point[0] = x;
        end_point[1] = y;
        horizontal_velocity = (start_point[0] - end_point[0])/100; //Scaling
        vertical_velocity = (start_point[1] - end_point[1])/100; //Scaling
        glutPostRedisplay();
    }
}

//---------------------------------------
// Motion callback for OpenGL
//---------------------------------------
void motion(int local_x, int local_y)
{

    // Calculate scale factors
    float x_scale = (MAX_X_VIEW - MIN_X_VIEW) /
        (float)(MAX_X_SCREEN - MIN_X_SCREEN);
    float y_scale = (MIN_Y_VIEW - MAX_Y_VIEW) /
        (float)(MAX_Y_SCREEN - MIN_Y_SCREEN);

    x = MIN_X_VIEW + (local_x - MIN_X_SCREEN) * x_scale;
    y = MAX_Y_VIEW + (local_y - MIN_Y_SCREEN) * y_scale;

    glutPostRedisplay();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
    rotate_cube += 2.5;

    // Clear display window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, 0);//rotate the cube at the origin
    if (click_start == false && click_finish == true) {
        glRotatef(rotate_cube, 1.0, 1.0, 1.0); //spin it
    }
    else {
        glRotatef(36, 1.0, 1.0, 1.0);
    }
    glTranslatef(-x, -y, 0); //translate the cube back

    //Draw the cube at the start of the click OR when the mouse is released
    if (click_start == true || click_start == false && click_finish == true)
        cube(x, y, z, CUBE_SIZE);

    glFlush();
}

void timer(int value)
{
    if (click_start == false && click_finish == true) {

            x += horizontal_velocity;
            y += vertical_velocity;
            vertical_velocity -= gravity;
    }

    // Redraw objects
    glutPostRedisplay();
    glutTimerFunc(SLEEP_TIME, timer, 0);
}

//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(MAX_Y_SCREEN, MAX_X_SCREEN);
    glutInitWindowPosition(MAX_Y_SCREEN / 2, MAX_X_SCREEN / 2);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutCreateWindow("Rectangles");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(SLEEP_TIME, timer, 0);

    glutMainLoop();
    return 0;
}