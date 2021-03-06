// VoronoiAniTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


// (5) -- division by 'w' deleted
// (6) -- curr_line_y coordinate changed from [-1;1] to [0; double]

/// ------------------------- THE FOLLOWING IS VORONOI DIAGRAM PROGRAM --------------------------------------------------------------------
//**
//#include <GL/glew.h> // Include the GLEW header file
#include "freeglut/include/GL/freeglut.h" // Include the GLUT header file
#include <iostream>
#include <math.h>
#include <algorithm>
#include <time.h>


#include "Voronoi.h"
#include "VPoint.h"

using namespace vor;
using namespace std;

void display(void);
void onEF();
void reshape(int width, int height);

vor::Voronoi * v;
vor::Vertices * ver; // vertices
vor::Edges * edg;	 // diagram edges

double w = 10000, x_left, x_right, curr_line_y, min_y;
const int  Sites = 10;
int line_dots = 1;
const double x_step = 0.01, y_line_step = 0.6; ///x_num = number of x
vector<double>* xs;
vector<double>* ys;
vector<double> lys;
vector<set<VPoint*>> line_sites;
int curr_beach_line_pos = -1;


double get_parabola_value_for_x(VPoint* focus, const double& line_y, const double& x)
{
	double fx = focus->x, fy = focus->y;
	double f = (fy - line_y) / 2, v2 = line_y + f;
	double y = (x * x / (4 * f)) - (x * fx / (2 * f)) + (fx * fx / (4 * f)) + v2;
	return y;
}

/*
void Voronoi::draw_Beach_Line(const double& y_curr, const double& y_next)
{
double curr_point_y = 0;
curr_line_y = y_curr;
VParabola* p;
VPoint* focii;
while (curr_line_y > y_next) {
for (int i = 0; i < line_dots; ++i) {
p = GetParabolaByX(xs[i]);
focii = p->site;
curr_point_y = get_parabola_value_for_x(focii, curr_point_y, xs[i]);
ys[i] = curr_point_y;
}
glBegin(GL_LINES);
glVertex2f( 0,  curr_line_y/w);
glVertex2f( xs[line_dots - 1] / w, curr_line_y/w);
glEnd();
for(int i = 0; i < line_dots - 1; ++i) {
glBegin(GL_LINES);
glVertex2f( xs[i] / w,  ys[i] / w);
glVertex2f( xs[i + 1] / w, ys[i + 1] / w);
glEnd();
}
curr_line_y -= y_line_step;
}
}
*/

Edges * Voronoi::GetEdges(Vertices * v, int w, int h)
{
	places = v;
	width = w;
	height = h;
	root = 0;

	if (!edges) edges = new Edges();
	else
	{
		for (Vertices::iterator i = points.begin(); i != points.end(); ++i) delete (*i);
		for (Edges::iterator i = edges->begin(); i != edges->end(); ++i) delete (*i);
		points.clear();
		edges->clear();
	}

	for (Vertices::iterator i = places->begin(); i != places->end(); ++i)
	{
		queue.push(new VEvent(*i, true));
	}

	VEvent * e;
	while (!queue.empty())
	{
		e = queue.top();
		queue.pop();
		ly = e->point->y;
		//cout<<ly<<endl;
		lys.emplace_back(ly);  // (6) <<<------
		if (deleted.find(e) != deleted.end()) { delete(e); deleted.erase(e); continue; }
		if (e->pe) InsertParabola(e->point);
		else RemoveParabola(e);
		delete(e);
		//draw_Beach_Line(ly, e->point->y);

		VParabola * p = root;///
		set<VPoint *> curr_sites;
		build_line(p, curr_sites);///            <<< -------------
		line_sites.emplace_back(curr_sites);///
											/// saving all sites that are focii of some parabola from current beachline;
	}

	FinishEdge(root);

	for (Edges::iterator i = edges->begin(); i != edges->end(); ++i)
	{
		if ((*i)->neighbour)
		{
			(*i)->start = (*i)->neighbour->end;
			delete (*i)->neighbour;
		}
	}

	return edges;
}

int main(int argc, char **argv)
{
	xs = new vector<double>;
	ys = new vector<double>;
	

	v = new Voronoi();
	ver = new Vertices();

	srand(time(NULL));

	//double miny, maxy;
	double px, py; //minx = w, maxx = -w;
	for (int i = 0; i<Sites; i++)
	{
		px = (double)rand() / (double)RAND_MAX;
		py = (double)rand() / (double)RAND_MAX;
		ver->push_back(new VPoint(px*w, py*w)); // (5) <- here was VPoint(px*w,py*w)
		//if (px < minx) minx = px;
		//if (px > maxx) maxx = px;
		//if (py < miny) miny = py;
		//if (py > maxy) maxy = py;
	}
	x_left = -1; // x_left = -1 <<---------
	x_right = 1; // x_right = 1 <<---------
	curr_line_y = w;  // (6) <<--
	min_y = 0;  // (6) <<--

	//double x_curr = minx - 100;
	double x_curr = x_left;
	int i = 0;
	//while (x_curr < (maxx + 100)) {
	while (x_curr < x_right) {
		xs->emplace_back(x_curr*w);
		ys->emplace_back(w);
		x_curr += x_step;
		++i;
	}
	line_dots = xs->size();


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<----------------------------------------------------------------------------------------
	edg = v->GetEdges(ver, w, w);
	std::cout << "voronois done!\n";

	for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
	{
		if ((*i)->start == 0)
		{
			std::cout << "missing the start of the edge!\n";
			continue;
		}
		if ((*i)->end == 0)
		{
			std::cout << "missing the end of the edge!\n";
			continue;
		}
	}
	/**
	for (int i = 0; i < lys.size(); ++i)
	{
		cout << " " << lys[i] << endl;
	}
	**/
	//**
	glutInit(&argc, argv); // Initialize GLUT
	glutInitDisplayMode(GLUT_SINGLE); // Set up a basic display buffer (only single buffered for now)
	glutInitWindowSize(600, 600); // Set the width and height of the window
	glutInitWindowPosition(100, 100); // Set the position of the window
	glutCreateWindow("OpenGL Window"); // Set the title for the window
	//glEnable( GL_POINT_SMOOTH );

	glutIdleFunc(onEF);
	glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering

	glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping

							  //glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
							  //glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events

	glutMainLoop(); // Enter GLUT's main loop
	//**/

	system("pause");
	return 0;
}

void recalcBeachLine()
{
	if (curr_line_y >= min_y) {
		curr_line_y -= y_line_step;
		//cout << curr_line_y << endl;
		//**
		if (curr_line_y < lys[curr_beach_line_pos + 1]) {curr_beach_line_pos++;} // (6) <<--
		if (curr_beach_line_pos >= 0) {
			for (int i = 0; i < line_dots; ++i)
			{
				double min_y = w, curr_y = 0;
				for (auto j : line_sites[curr_beach_line_pos])
				{
					VPoint * foc = j;
					curr_y = get_parabola_value_for_x(foc, curr_line_y, (*xs)[i]); //curline *w/2 + 1
					if (curr_y < min_y) { min_y = curr_y; }
				}
				//if (min_y > w) min_y = w;
				(*ys)[i] = min_y;
			}
		}
		///curr_beach_line_pos++;   ///<<<<<<<<<<<<<<<<<-----------------------------------------------------------------
		//**/
	}
}

void animationVoronoi()
{
	///float w = 1;
	glBegin(GL_LINES);
	glVertex2f(x_left,  -1 + 2 * curr_line_y / w); // (6) <<--
	glVertex2f(x_right,  -1 + 2 * curr_line_y / w); // (6) <<--
	glEnd();
	/**
	glBegin(GL_LINES);
	glVertex2f(x_left, -1 + 2 * lys[curr_beach_line_pos+1] / w); // (6) <<--
	glVertex2f(x_right, -1 + 2 * lys[curr_beach_line_pos+1] / w); // (6) <<--
	glEnd();
	**/
	for (vor::Vertices::iterator i = ver->begin(); i != ver->end(); ++i)
	{
		//**
		glBegin(GL_QUADS);
		//cout << (*i)->x << " " << (*i)->y << endl;
		glVertex2f(-1 + 2 * (*i)->x / w - 0.01, -1 + 2 * (*i)->y / w - 0.01);  // (5) <-
		glVertex2f(-1 + 2 * (*i)->x / w + 0.01, -1 + 2 * (*i)->y / w - 0.01);  // (5) <-
		glVertex2f(-1 + 2 * (*i)->x / w + 0.01, -1 + 2 * (*i)->y / w + 0.01);  // (5) <-
		glVertex2f(-1 + 2 * (*i)->x / w - 0.01, -1 + 2 * (*i)->y / w + 0.01);  // (5) <-
		/**/
		/**
		glBegin(GL_POINTS);
		glVertex2f((*i)->x, (*i)->y);
		//cout << (*i)->x << " " << (*i)->y << endl;
		**/
		glEnd();
	}
	//**
	for (int i = 0; i < line_dots - 1; ++i) {
		glBegin(GL_LINES);
		glVertex2f(-1 + 2 * (*xs)[i] / w, -1 + 2 * (*ys)[i] / w);
		glVertex2f(-1 + 2 * (*xs)[i + 1] / w, -1 + 2 * (*ys)[i + 1] / w);
		glEnd();
	}
	//**/
}

void drawVoronoi()
{

	/**
	vor::Vertices::iterator j = dir->begin();
	for(vor::Vertices::iterator i = ver->begin(); i != ver->end(); ++i)
	{
	(*i)->x += (*j)->x * w/50;
	(*i)->y += (*j)->y * w/50;
	if( (*i)->x > w ) (*j)->x *= -1;
	if( (*i)->x < 0 ) (*j)->x *= -1;

	if( (*i)->y > w ) (*j)->y *= -1;
	if( (*i)->y < 0 ) (*j)->y *= -1;
	++j;
	}  **/


	edg = v->GetEdges(ver, w, w);
	//std::cout << "voronoi done";
	//**
	for(vor::Vertices::iterator i = ver->begin(); i!= ver->end(); ++i)
	{
	glBegin(GL_QUADS);
	//std::cout << (*i)->x << "\n";
	glVertex2f( -1+2*(*i)->x/w -0.01,  -1+2*(*i)->y/w - 0.01);  // (5) <-
	glVertex2f( -1+2*(*i)->x/w +0.01,  -1+2*(*i)->y/w - 0.01);  // (5) <-
	glVertex2f( -1+2*(*i)->x/w +0.01,  -1+2*(*i)->y/w + 0.01);  // (5) <-
	glVertex2f( -1+2*(*i)->x/w -0.01,  -1+2*(*i)->y/w + 0.01);  // (5) <-
	glEnd();
	}   //**/

	//**
	for(vor::Edges::iterator i = edg->begin(); i!= edg->end(); ++i)
	{
	/**   This cycle builds the Dilone triangulation
	glBegin(GL_LINES);
	glVertex2f( -1+2*(*i)->left->x/w,  -1+2*(*i)->left->y/w);
	glVertex2f( -1+2*(*i)->right->x/w, -1+2*(*i)->right->y/w);
	glEnd();
	**/ //  <<-----------------------------------------------------------------------------
	//**
	glBegin(GL_LINES);
	glVertex2f( -1+2*(*i)->start->x/w,  -1+2*(*i)->start->y/w);  // (5) <-
	glVertex2f( -1+2*(*i)->end->x/w, -1+2*(*i)->end->y/w);       // (5) <-
	glEnd();
	//**

	}
	/**/
}



void display(void)
{
	//std::cout << "display\n";
	///glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	///glTranslatef(0.0f, 0.0f, -5.0f);

	///glClear(GL_COLOR_BUFFER_BIT);//Clear the screen
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Clear the background of our window to red
	glClear(GL_COLOR_BUFFER_BIT);//Clear the screen

	//drawVoronoi();
	animationVoronoi();
	glutSwapBuffers();

	glFlush();
}


void onEF()
{
	recalcBeachLine();
	glutPostRedisplay();
}

void reshape(int width, int height)
{

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
					  ///gluPerspective(22.5, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
	glOrtho(-1, 1, -1, 1, -1, 10);
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}


//**
///

///----------------------------------------------------- END OF PROGRAM --------------------------------------------------------------------


