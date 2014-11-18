/*
 *  main.cpp
 *  Battle_HW2
 *
 *  Created by Jessica Battle on 10/17/11.
 *  Copyright 2011 Education Advancement Alliance . All rights reserved.
 *
 */

#include "main.h"
#include <iostream>
#include <algorithm>
#include <time.h>			//for random number generator
#include <math.h>			//for displacement vector calculations
#include <vector>			//for collection of vertices
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h>
#include <GLUT/glut.h>


using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Define Classes

class Vertex3d
{
public:
	//Constructors
	Vertex3d (float xCoord, float yCoord, float zCoord)
	{
		x = xCoord;
		y = yCoord;
		z = zCoord;
	}
	
	Vertex3d()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	
	//Data members
	float x;
	float y;
	float z;
	
};//end Vertex3d	



class Polygon3d
{
public:
	//Constructors
	Polygon3d (Vertex3d newPoint)
	{
		polyPoint = newPoint;
		addVertex(polyPoint);
		newPolyColor(0.0, 0.0, 0.0, 1.0);
	}//end constructor
	
	Polygon3d ()
	{
		polyPoint.x = 0.0;
		polyPoint.y = 0.0;
		polyPoint.z = 0.0;
		addVertex(polyPoint);
		newPolyColor(0.0, 0.0, 0.0, 1.0);
	}//end default constructor
	
	
	
	//Member functions
	int getNumVertices()
	{
		return vList.size();
	}//end function getNumVertices
	
	void addVertex(Vertex3d nextPoint)
	{
		vList.push_back(nextPoint);
	}//end function addVertex
	
	void newPolyColor(float red, float green, float blue, float alpha)
	{
		polyColor[0] = red;
		polyColor[1] = green;
		polyColor[2] = blue;
		polyColor[3] = alpha;
	}//end function newPolyColor
	
	
	//Data members
	vector <Vertex3d> vList;
	float polyColor[4];
	Vertex3d polyPoint;
	
};//end class Polygon3d	


class Mesh3d
{
public:
	//Constructors
	Mesh3d (Polygon3d newPoly)
	{
		meshPoly = newPoly;
		addPoly(meshPoly);
	}//end constructor
	
	Mesh3d ()
	{
		meshPoly; 
	}//end default constructor
	
	//Member functions
	int getNumPoly()
	{
		return polyList.size();
	}//end function getNumVertices
	
	void addPoly(Polygon3d nextPoly)
	{
		polyList.push_back(nextPoly);
	}//end function addVertex
	
	//Data members
	vector <Polygon3d> polyList;
	Polygon3d meshPoly;
	
};//end class Mesh3d	

//==================================================================================
//Classes for Collision Detection

//First, a couple of useful functions with definitions for the classes
Vertex3d crossProduct(Vertex3d a, Vertex3d b)
{
	return Vertex3d (a.y * b.z - a.z * b.y,
					 a.z * b.x - a.x * b.z,
					 a.x * b.y - a.y * b.x);
}

float dotProduct(Vertex3d a, Vertex3d b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}


Vertex3d normalizeVec(Vertex3d a)
{
	float sqrMag = (a.x * a.x + a.y * a.y + a.z * a.z);
	Vertex3d normVec;
	if (sqrMag > 0.0f) //check for division by 0.0f
	{
		normVec.x = a.x * (1.0f/sqrMag);
		normVec.y = a.y * (1.0f/sqrMag);
		normVec.z = a.z * (1.0f/sqrMag);
	}
	return (normVec);
} 



class polyCollision
{
public:
	//Constructors
	polyCollision (Vertex3d newPoint)
	{
		polyPoint = newPoint;
		addVertex(polyPoint);
		collision = false;
	}//end constructor
	
	polyCollision ()
	{
		polyPoint.x = 0.0;
		polyPoint.y = 0.0;
		polyPoint.z = 0.0;
		addVertex(polyPoint);
		collision = false;
	}//end default constructor
	
	
	
	//Member functions
	void addVertex(Vertex3d nextPoint)
	{
		vList.push_back(nextPoint);
	}//end function addVertex
	
	
	void setVecPtsU()	//Set up vector u
	{
	
		vectorPtsU[0] = vList[1];
		vectorPtsU[1] = vList[0];
	}
	
	void setVecPtsV()	//Set up vector v
	{
		
		vectorPtsV[0] = vList[1];
		vectorPtsV[1] = vList[2];
	}
	
	void printVec(Vertex3d* A) //Helper function for testing
	{
		//Print start vertex
		cout << endl;
		cout << A[0].x << " ";
		cout << A[0].y << " ";
		cout << A[0].z << " ";
		
		//Print end vertex
		cout << endl;
		cout << A[1].x << " ";
		cout << A[1].y << " ";
		cout << A[1].z << " ";
	}
	
	void computeVecs() //compute vector
	{
		//Compute vector U
		vectorU.x = vectorPtsU[1].x - vectorPtsU[0].x;
		vectorU.y = vectorPtsU[1].y - vectorPtsU[0].y;
		vectorU.z = vectorPtsU[1].z - vectorPtsU[0].z;
		
		//Compute vector V
		vectorV.x = vectorPtsV[1].x - vectorPtsV[0].x;
		vectorV.y = vectorPtsV[1].y - vectorPtsV[0].y;
		vectorV.z = vectorPtsV[1].z - vectorPtsV[0].z;
	}
	
	void computeNorm()//computes the normalized vector
	{
		normal = normalizeVec(crossProduct(vectorU, vectorV));
	}
	
	bool isInside(Vertex3d d)
	{
		
		float distance = 0.0;
		
		distance = ((normal.x * (d.x - vectorPtsU[0].x)) 
					+ (normal.y * (d.y - vectorPtsU[0].y)) 
					+ (normal.z * (d.z - vectorPtsU[0].z)));
		
		if (distance > 0)
		{
			collision = true;
			return 0;
		}
		else
		{
			return 1;
		}
	}
	
	
	
	//Data members
	vector <Vertex3d> vList;
	Vertex3d polyPoint;
	Vertex3d vectorPtsU[2];
	Vertex3d vectorPtsV[2];
	Vertex3d vectorU;
	Vertex3d vectorV;
	Vertex3d normal;	//normalized normal
	bool collision;
	
};//end class polyCollision	


class meshCollision
{
public:
	//Constructors
	meshCollision (polyCollision newPolyCol)
	{
		meshPolyCol = newPolyCol;
		addPolyCol(meshPolyCol);
		for (int i = 0; i < 11; i++) 
		{
			collision[i] = 0;
		}
		
	}//end constructor
	
	meshCollision ()
	{
		meshPolyCol; 
		for (int i = 0; i < 11; i++) 
		{
			collision[i] = 0;
		}
	}//end default constructor
	
	
	void addPolyCol(polyCollision nextPolyCol)
	{
		polyListCol.push_back(nextPolyCol);
	}//end function addVertex
	
	int isOutsideMesh()
	{
		
		for(int i = 1; i < 11; i++)
		{
			if(polyListCol[i].collision)
			{
				//collision[i] = i;
				return i;
			}
		}//end for
		return 0;
		
	}//end function isOutsideMesh

	//Data members
	vector <polyCollision> polyListCol;
	int collision[11];
	polyCollision meshPolyCol;

	
};//end class meshCollision	




////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////



//Set up Variables
GLsizei width, height;
const float BOX_WIDTH = 2.0;
const float BOX_HEIGHT = 2.0;



//Stuff for the displacement vector
const float PI = 3.14;					//For the displacement vector's angle
struct displacement						//Record for the displacement vector
{
	float x;
	float y;
	float z;
};
displacement vectorA, vectorB;			//declare struct variables for displacement vector


//Variables to manage points/vertices
struct point							//Record the next point
{
	float x;
	float y;
	float z;
};
point oldPoint, newPoint;
vector<point> vertexCollection;			//Set up data structure to contain all vertices to be drawn

//Variable to track mouse left-click
bool lbuttonDown = false;


//**************Set up 3D mesh points**************************

float length = 4.0;

Vertex3d p1(-0.5,-1.5, length/2.0);
Vertex3d p2(0.5, -1.5, length/2.0);
Vertex3d p3(1.5, -0.5, length/2.0);
Vertex3d p4(1.5, 0.5, length/2.0);
Vertex3d p5(0.5, 1.5, length/2.0);
Vertex3d p6(-0.5, 1.5, length/2.0);
Vertex3d p7(-1.5, 0.5, length/2.0);
Vertex3d p8(-1.5, -0.5, length/2.0);

Vertex3d p9(-0.5,-1.5, -length/2.0);
Vertex3d p10(0.5, -1.5, -length/2.0);
Vertex3d p11(1.5, -0.5, -length/2.0);
Vertex3d p12(1.5, 0.5, -length/2.0);
Vertex3d p13(0.5, 1.5, -length/2.0);
Vertex3d p14(-0.5, 1.5, -length/2.0);
Vertex3d p15(-1.5, 0.5, -length/2.0);
Vertex3d p16(-1.5, -0.5, -length/2.0);

Vertex3d specialPtsTop1(-1.5, -1.5, length/2.0);
Vertex3d specialPtsTop2(1.5, -1.5, length/2.0);
Vertex3d specialPtsTop3(1.5, 1.5, length/2.0);
Vertex3d specialPtsTop4(-1.5, 1.5, length/2.0);

Vertex3d specialPtsBottom1(-0.5, -1.5, -length/2.0);
Vertex3d specialPtsBottom2(0.5, -1.5, -length/2.0);
Vertex3d specialPtsBottom3(0.5, 1.5, -length/2.0);
Vertex3d specialPtsBottom4(-0.5, 1.5, -length/2.0);


//************************************************************************

//Flag to stop the particle motion when a collision happens
bool keepGoing = true;

//Flag to stop the particle motion when pause is pressed
bool isPaused = false;

//Flags to change the color of the polygon that's hit by particle
int changeColor[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};		//I included a dummy value for the 0th slot so numbers line up to sides




//Function Prototypes
void ChangeSize(GLsizei width, GLsizei height);
void DrawScene(void);
void init(void);
void particleMotion(void);
void pauseMotion(unsigned char, int, int); //Press 'I' or 'i' to pause/unpause; 'S' or 's' to start
void m_glewInitAndVersion(void);
void mouse(int, int, int, int);
void motion(int, int);

//Variables for moving shape
GLfloat     rotateAngleX;     // Angle rotation
GLfloat     rotateAngleY;     // Angle rotation
GLfloat oldPos[2] = {0, 0};
GLfloat currentPos[2] = {0, 0};




//Enter the main program
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);		//Pass command-line arguments & initialize GLUT library
	
	//Tell GLUT library what type of display mode we want when we create the window
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  //Use a double buffer for animation & RGBA = color mode
	
	
	//Tell GLUT how large & where the window should be & create the window
	glutInitWindowSize(600,600);
	glutInitWindowPosition(300,300);
	glutCreateWindow("Project 2");
	
	
	//Register callback functions for specific events --> when the window changes size
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(DrawScene);
	glutIdleFunc(particleMotion);
	glutKeyboardFunc(pauseMotion);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	
	//Initialize
	init();
	m_glewInitAndVersion(); 
	
	//Enter the event loop
	glutMainLoop();
	
	return 0;
}//end main



//Function Definitions
void ChangeSize(GLsizei width, GLsizei height)
/*Set up the viewport, projection matrix, & camera.  We need to do 
 this when the window is first created or if it changes size after it is created*/

{
	//cout << "ChangeSize" << endl;
	glViewport(0,0,(GLsizei) width, (GLsizei) height);
	
	//Set up camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-4.0, 4.0, -4.0, 4.0, -4.0, 4.0);
	
	//Position the camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}//end ChangeSize


void init(void)
//Set the canvas color up for the program
{
	glClearColor(0.79, 0.88, 0.95, 1.0);	//Clear bg 
	
	//Prepare for random generation based on time
	srand (time(NULL));
	float randomx_coord, randomy_coord, randomz_coord;		//Coordinates to be randomly generated
	int a = rand();
	int b = rand();
	int c = rand();
	
	//cout << "a = " << a << " " << (float)a/RAND_MAX << endl;
	//cout << "b = " << b << " " << (float)b/RAND_MAX << endl;
	randomx_coord = (float)rand()/RAND_MAX;//Get random coordinates for the start point
	randomy_coord = (float)rand()/RAND_MAX;//Get random coordinates for the start point
	randomz_coord = (float)rand()/RAND_MAX;//Get random coordinates for the start point

	
	//cout << "x = " << randomx_coord << " y = " << randomy_coord << endl;
	//Set the first member of the vertex collection to the starting point
	point startPoint; 
	//startPoint.x = 0.0;
	//startPoint.y = 0.0;
	//startPoint.z = 0.0;
	
	startPoint.x = randomx_coord;
	startPoint.y = randomy_coord;
	startPoint.z = randomz_coord;
	
	vertexCollection.push_back(startPoint);
	//cout << vertexCollection[0].x << " " << vertexCollection[0].y << endl;
	
	//glEnable(GL_BLEND);	//enable blending
	glBlendFunc(GL_ONE, GL_ONE);
	//glShadeModel(GL_FLAT);
	
	glutIdleFunc(NULL);
	

	
}//end init


void DrawScene(void)
{
	//cout << "DrawScene" << endl;
	//clear window with clearing color set in init function
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	//Set up 3D mesh points
	
	//---Top Face
	Polygon3d octagonTop(p1);
	octagonTop.addVertex(p2);
	octagonTop.addVertex(p3);
	octagonTop.addVertex(p4);
	octagonTop.addVertex(p5);
	octagonTop.addVertex(p6);
	octagonTop.addVertex(p7);
	octagonTop.addVertex(p8);
	//cout << changeColor[9];
	if (!changeColor[9]) 
	{
		octagonTop.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		octagonTop.newPolyColor(1.0, 1.0, 0.0, 0.75);
	}
	Mesh3d octMesh(octagonTop);
	
	//---Bottom Face
	Polygon3d octagonBottom(p9);
	octagonBottom.addVertex(p10);
	octagonBottom.addVertex(p11);
	octagonBottom.addVertex(p12);
	octagonBottom.addVertex(p13);
	octagonBottom.addVertex(p14);
	octagonBottom.addVertex(p15);
	octagonBottom.addVertex(p16);
	if (!changeColor[10]) 
	{
		octagonBottom.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		octagonBottom.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(octagonBottom);
	
	
	//---Side Face1
	Polygon3d sideFace1(p1);
	sideFace1.addVertex(p2);
	sideFace1.addVertex(p10);
	sideFace1.addVertex(p9);
	if (!changeColor[1]) 
	{
		sideFace1.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace1.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace1);
	
	//---Side Face2
	Polygon3d sideFace2(p1);
	sideFace2.addVertex(p9);
	sideFace2.addVertex(p16);
	sideFace2.addVertex(p8);
	if (!changeColor[2]) 
	{
		sideFace2.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace2.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace2);
	
	//---Side Face3
	Polygon3d sideFace3(p8);
	sideFace3.addVertex(p7);
	sideFace3.addVertex(p15);
	sideFace3.addVertex(p16);
	if (!changeColor[3]) 
	{
		sideFace3.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace3.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace3);
	
	//---Side Face4
	Polygon3d sideFace4(p7);
	sideFace4.addVertex(p6);
	sideFace4.addVertex(p14);
	sideFace4.addVertex(p15);
	if (!changeColor[4]) 
	{
		sideFace4.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace4.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace4);
	
	//---Side Face5
	Polygon3d sideFace5(p6);
	sideFace5.addVertex(p5);
	sideFace5.addVertex(p13);
	sideFace5.addVertex(p14);
	if (!changeColor[5]) 
	{
		sideFace5.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace5.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace5);
	
	//---Side Face6
	Polygon3d sideFace6(p5);
	sideFace6.addVertex(p4);
	sideFace6.addVertex(p12);
	sideFace6.addVertex(p13);
	if (!changeColor[6]) 
	{
		sideFace6.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace6.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace6);
	
	//---Side Face7
	Polygon3d sideFace7(p4);
	sideFace7.addVertex(p3);
	sideFace7.addVertex(p11);
	sideFace7.addVertex(p12);
	if (!changeColor[7]) 
	{
		sideFace7.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace7.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace7);
	
	//---Side Face8
	Polygon3d sideFace8(p3);
	sideFace8.addVertex(p2);
	sideFace8.addVertex(p10);
	sideFace8.addVertex(p11);
	if (!changeColor[8]) 
	{
		sideFace8.newPolyColor(0.0, 1.0, 0.0, 0.75);
	}
	else 
	{
		sideFace8.newPolyColor(1.0, 0.0, 0.0, 0.75);
	}
	octMesh.addPoly(sideFace8);
	


//-------Draw particle onto screen--------
	//Draw start point on screen
	//cout << "x = " << vertexCollection[0].x << " " << vertexCollection[0].y << endl;
	glColor3f(1.0, 1.0, 1.0);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glVertex3f(vertexCollection[0].x, vertexCollection[0].y, vertexCollection[0].z);
	glEnd();
	//glutIdleFunc(NULL);
	
	//std::vector<point>::iterator it;
	
//Draw all of the lines to connect the dots of the vertexCollection
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < (unsigned)vertexCollection.size(); i++)
	{
		//cout << vertexCollection[i].x << " " << vertexCollection[i].y << " " << vertexCollection[i].z << endl;
		glVertex3f(vertexCollection[i].x, vertexCollection[i].y, vertexCollection[i].z);		//vertex
	}//end for		
	glEnd();
	

		

//-------Draw the shape---------------	
	
	//glRotatef(rotateAngleX,0.0f,1.0f,0.0f);            // Rotate The Mesh
	//glRotatef(rotateAngleY,1.0f,0.0f,0.0f);            // Rotate The Mesh
	
	
	//Draw top face
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(octagonTop.polyColor[0], octagonTop.polyColor[1], octagonTop.polyColor[2]);
	
	for (int i = 0; i < octagonTop.getNumVertices(); i++) 
	{
		glVertex3f(octagonTop.vList[i].x, octagonTop.vList[i].y, octagonTop.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < octagonTop.getNumVertices(); i++) 
	{
		glVertex3f(octagonTop.vList[i].x, octagonTop.vList[i].y, octagonTop.vList[i].z);
	}//end for loop
	//octagonTop.vList[0];
	glEnd();
	
	
	//Draw bottom face
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(octagonBottom.polyColor[0], octagonBottom.polyColor[1], octagonBottom.polyColor[2]);
	
	for (int i = 0; i < octagonBottom.getNumVertices(); i++) 
	{
		glVertex3f(octagonBottom.vList[i].x, octagonBottom.vList[i].y, octagonBottom.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < octagonBottom.getNumVertices(); i++) 
	{
		glVertex3f(octagonBottom.vList[i].x, octagonBottom.vList[i].y, octagonBottom.vList[i].z);
	}//end for loop
	//octagonBottom.vList[0];
	glEnd();
	
	//-------------------------------------	
	

	//Draw sideFace1
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace1.polyColor[0], sideFace1.polyColor[1], sideFace1.polyColor[2]);
	
	for (int i = 0; i < sideFace1.getNumVertices(); i++) 
	{
		glVertex3f(sideFace1.vList[i].x, sideFace1.vList[i].y, sideFace1.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace1.getNumVertices(); i++) 
	{
		glVertex3f(sideFace1.vList[i].x, sideFace1.vList[i].y, sideFace1.vList[i].z);
	}//end for loop
	//sideFace1.vList[0];
	glEnd();
	
	
	//-------------------------------------
	
	//Draw sideFace2
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace2.polyColor[0], sideFace2.polyColor[1], sideFace2.polyColor[2]);
	
	for (int i = 0; i < sideFace2.getNumVertices(); i++) 
	{
		glVertex3f(sideFace2.vList[i].x, sideFace2.vList[i].y, sideFace2.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//Disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace2.getNumVertices(); i++) 
	{
		glVertex3f(sideFace2.vList[i].x, sideFace2.vList[i].y, sideFace2.vList[i].z);
	}//end for loop
	//sideFace2.vList[0];
	glEnd();
	
	//-------------------------------------

	//Draw sideFace3
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace3.polyColor[0], sideFace3.polyColor[1], sideFace3.polyColor[2]);
	
	for (int i = 0; i < sideFace3.getNumVertices(); i++) 
	{
		glVertex3f(sideFace3.vList[i].x, sideFace3.vList[i].y, sideFace3.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//Disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace3.getNumVertices(); i++) 
	{
		glVertex3f(sideFace3.vList[i].x, sideFace3.vList[i].y, sideFace3.vList[i].z);
	}//end for loop
	//sideFace3.vList[0];
	glEnd();
	
	//-------------------------------------
	
	//Draw sideFace4
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace4.polyColor[0], sideFace4.polyColor[1], sideFace4.polyColor[2]);
	
	for (int i = 0; i < sideFace2.getNumVertices(); i++) 
	{
		glVertex3f(sideFace4.vList[i].x, sideFace4.vList[i].y, sideFace4.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace4.getNumVertices(); i++) 
	{
		glVertex3f(sideFace4.vList[i].x, sideFace4.vList[i].y, sideFace4.vList[i].z);
	}//end for loop
	//sideFace4.vList[0];
	glEnd();
	
	//-------------------------------------	
	
	//Draw sideFace5
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace5.polyColor[0], sideFace5.polyColor[1], sideFace5.polyColor[2]);
	
	for (int i = 0; i < sideFace5.getNumVertices(); i++) 
	{
		glVertex3f(sideFace5.vList[i].x, sideFace5.vList[i].y, sideFace5.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//Disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace5.getNumVertices(); i++) 
	{
		glVertex3f(sideFace5.vList[i].x, sideFace5.vList[i].y, sideFace5.vList[i].z);
	}//end for loop
	//sideFace5.vList[0];
	glEnd();
	
	//-------------------------------------	
	
	//Draw sideFace6
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace6.polyColor[0], sideFace6.polyColor[1], sideFace6.polyColor[2]);
	
	for (int i = 0; i < sideFace6.getNumVertices(); i++) 
	{
		glVertex3f(sideFace6.vList[i].x, sideFace6.vList[i].y, sideFace6.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace6.getNumVertices(); i++) 
	{
		glVertex3f(sideFace6.vList[i].x, sideFace6.vList[i].y, sideFace6.vList[i].z);
	}//end for loop
	//sideFace6.vList[0];
	glEnd();
	
	//-------------------------------------	
	
	//Draw sideFace7
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace7.polyColor[0], sideFace7.polyColor[1], sideFace7.polyColor[2]);
	
	for (int i = 0; i < sideFace2.getNumVertices(); i++) 
	{
		glVertex3f(sideFace7.vList[i].x, sideFace7.vList[i].y, sideFace7.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace7.getNumVertices(); i++) 
	{
		glVertex3f(sideFace7.vList[i].x, sideFace7.vList[i].y, sideFace7.vList[i].z);
	}//end for loop
	//sideFace7.vList[0];
	glEnd();
	
	//-------------------------------------	
	
	//Draw sideFace8
	glEnable(GL_BLEND);	//enable blending
	glBegin(GL_POLYGON);
	glColor3f(sideFace8.polyColor[0], sideFace8.polyColor[1], sideFace8.polyColor[2]);
	
	for (int i = 0; i < sideFace8.getNumVertices(); i++) 
	{
		glVertex3f(sideFace8.vList[i].x, sideFace8.vList[i].y, sideFace8.vList[i].z);
	}//end for loop
	glEnd();
	glDisable(GL_BLEND);	//disable blending
	
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0);
	
	for (int i = 0; i < sideFace8.getNumVertices(); i++) 
	{
		glVertex3f(sideFace8.vList[i].x, sideFace8.vList[i].y, sideFace8.vList[i].z);
	}//end for loop
	//sideFace8.vList[0];
	glEnd();
	
	//-------------------------------------	
	
	
	//Draw point at exit point
	if(!keepGoing)
	{
		
		glColor3f(0.0, 0.0, 1.0);
		glPointSize(4.0f);
		glBegin(GL_POINTS);
		glVertex3f(vertexCollection.back().x, vertexCollection.back().y, vertexCollection.back().z);
		glEnd();
	/*	
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(vertexCollection.back().x - .05, vertexCollection.back().y, 0.0f);		//vertex	
		glVertex3f(vertexCollection.back().x + .05, vertexCollection.back().y, 0.0f);	
		glVertex3f(vertexCollection.back().x, vertexCollection.back().y - .05, 0.0f);
		glVertex3f(vertexCollection.back().x, vertexCollection.back().y + .05, 0.0f);
	*/	
		glEnd();
	}//end if
	
	
	
	//Do the buffer swap to display the back buffer
	glutSwapBuffers();

}//end DrawScene


void particleMotion(void)
{
	
	//cout << "particleMotion" << endl;
	
	//Calculate displacement vectorA to use when there's no collision
	float x_val = 2*((float)rand()/RAND_MAX)-1;		//Find random x, y, z values
	float y_val = 2*((float)rand()/RAND_MAX)-1;
	float z_val = 2*((float)rand()/RAND_MAX)-1;
	//cout << "\nHere's z_val" << z_val << endl;
	
	float magnitude = sqrt((x_val*x_val) + (y_val*y_val) + (z_val*z_val));
	
	float unit_x = x_val/magnitude;  //Calculate the unit vector for both x & y
	float unit_y = y_val/magnitude;
	float unit_z = z_val/magnitude;
	
	vectorA.x = unit_x * ((0.02) * min(BOX_HEIGHT, BOX_WIDTH));
	vectorA.y = unit_y * ((0.02) * min(BOX_HEIGHT, BOX_WIDTH));
	vectorA.z = unit_z * ((0.02) * min(BOX_HEIGHT, BOX_WIDTH));
	//cout << endl << vectorA.z << endl;
	
	//point addPoint = vertexCollection.end();
	
	newPoint.x = (vertexCollection.back().x + vectorA.x);	//Calculate the new point position
	newPoint.y = (vertexCollection.back().y + vectorA.y);
	newPoint.z = (vertexCollection.back().z + vectorA.z);
	//cout << endl << newPoint.z << endl;
	
	//Add new point to point bag
	vertexCollection.push_back(newPoint);
	
	glutPostRedisplay();
	
	
	//check for collision
	/*
	//Test polyCollision
	
	//---Side Face3
	polyCollision sideFace3Col(p7);
	sideFace3Col.addVertex(p8);
	sideFace3Col.addVertex(p16);
	sideFace3Col.setVecPtsU();
	sideFace3Col.setVecPtsV();
	sideFace3Col.computeVecs();
	sideFace3Col.computeNorm();
	//sideFace3Col.printVec(sideFace1Col.vectorPtsU);
	//sideFace3Col.printVec(sideFace1Col.vectorPtsV);
	*/
	
//*************************Set up collision mesh***********************************
	meshCollision octagonMeshCol;
	
	//---Top Face
	polyCollision octTopCol(specialPtsTop4);
	octTopCol.addVertex(specialPtsTop3);
	octTopCol.addVertex(specialPtsTop2);
	octTopCol.setVecPtsU();
	octTopCol.setVecPtsV();
	octTopCol.computeVecs();
	octTopCol.computeNorm();
	octagonMeshCol.addPolyCol(octTopCol); 
	 
	
	//---Bottom Face
	 polyCollision octBotCol(specialPtsBottom2);
	 octBotCol.addVertex(specialPtsBottom3);
	 octBotCol.addVertex(specialPtsBottom4);
	 octBotCol.setVecPtsU();
	 octBotCol.setVecPtsV();
	 octBotCol.computeVecs();
	 octBotCol.computeNorm();
	 octagonMeshCol.addPolyCol(octBotCol);
	 
	 
	//---Side Face1
	polyCollision sideFace1Col(p1);
	sideFace1Col.addVertex(p2);
	sideFace1Col.addVertex(p10);
	sideFace1Col.setVecPtsU();
	sideFace1Col.setVecPtsV();
	sideFace1Col.computeVecs();
	sideFace1Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace1Col);
	
	//---Side Face2
	polyCollision sideFace2Col(p8);
	sideFace2Col.addVertex(p1);
	sideFace2Col.addVertex(p9);
	sideFace2Col.setVecPtsU();
	sideFace2Col.setVecPtsV();
	sideFace2Col.computeVecs();
	sideFace2Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace2Col);
	
	//---Side Face3
	polyCollision sideFace3Col(p7);
	sideFace3Col.addVertex(p8);
	sideFace3Col.addVertex(p16);
	sideFace3Col.setVecPtsU();
	sideFace3Col.setVecPtsV();
	sideFace3Col.computeVecs();
	sideFace3Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace3Col);
	
	//---Side Face4
	polyCollision sideFace4Col(p6);
	sideFace4Col.addVertex(p7);
	sideFace4Col.addVertex(p15);
	sideFace4Col.setVecPtsU();
	sideFace4Col.setVecPtsV();
	sideFace4Col.computeVecs();
	sideFace4Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace4Col);
	
	//---Side Face5
	polyCollision sideFace5Col(p5);
	sideFace5Col.addVertex(p6);
	sideFace5Col.addVertex(p14);
	sideFace5Col.setVecPtsU();
	sideFace5Col.setVecPtsV();
	sideFace5Col.computeVecs();
	sideFace5Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace5Col);
	
	//---Side Face6
	polyCollision sideFace6Col(p4);
	sideFace6Col.addVertex(p5);
	sideFace6Col.addVertex(p13);
	sideFace6Col.setVecPtsU();
	sideFace6Col.setVecPtsV();
	sideFace6Col.computeVecs();
	sideFace6Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace6Col);
	
	//---Side Face7
	polyCollision sideFace7Col(p3);
	sideFace7Col.addVertex(p4);
	sideFace7Col.addVertex(p12);
	sideFace7Col.setVecPtsU();
	sideFace7Col.setVecPtsV();
	sideFace7Col.computeVecs();
	sideFace7Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace7Col);
	
	//---Side Face8
	polyCollision sideFace8Col(p2);
	sideFace8Col.addVertex(p3);
	sideFace8Col.addVertex(p11);
	sideFace8Col.setVecPtsU();
	sideFace8Col.setVecPtsV();
	sideFace8Col.computeVecs();
	sideFace8Col.computeNorm();
	octagonMeshCol.addPolyCol(sideFace8Col);
	
	
	
//***************************************************************************************
	
	
	//convert relevant displacement types Vertex3d types
	Vertex3d newPointV3d;
	newPointV3d.x = newPoint.x;
	newPointV3d.y = newPoint.y;
	newPointV3d.z = newPoint.z;
	
	//Collision Test
	//if point is not inside any of these polygons
	if(!(sideFace1Col.isInside(newPointV3d) 
		 &&
		 sideFace2Col.isInside(newPointV3d) 
		 &&
		 sideFace3Col.isInside(newPointV3d) 
		 &&
		 sideFace4Col.isInside(newPointV3d) 
		 &&
		 sideFace5Col.isInside(newPointV3d) 
		 &&
		 sideFace6Col.isInside(newPointV3d) 
		 &&
		 sideFace7Col.isInside(newPointV3d) 
		 && 
		 sideFace8Col.isInside(newPointV3d)
		 &&
		 octTopCol.isInside(newPointV3d)
		 &&
		 octBotCol.isInside(newPointV3d)))
	{
		keepGoing = false;
		cout << sideFace1Col.collision << endl;
		cout << sideFace2Col.collision << endl;
		cout << sideFace3Col.collision << endl;
		cout << sideFace4Col.collision << endl;
		cout << sideFace5Col.collision << endl;
		cout << sideFace6Col.collision << endl;
		cout << sideFace7Col.collision << endl;
		cout << sideFace8Col.collision << endl;
		cout << octTopCol.collision << endl;
		cout << octBotCol.collision << endl;
		
	}//end if
	
	
	

	
	
	//if there is no collision
	if(keepGoing)
	{
		//Ask for draw function again
		glutPostRedisplay();
	}//end if
	
	//if there is collision
	else
	{
		cout << "COLLISION!!!!" ;//<< octagonMeshCol.isOutsideMesh();
		if(sideFace1Col.collision)
			{changeColor[1] = 1;}
		else if(sideFace2Col.collision)
			{changeColor[2] = 1;}
		else if(sideFace3Col.collision)
			{changeColor[3] = 1;}
		else if(sideFace4Col.collision)
			{changeColor[4] = 1;}
		else if(sideFace5Col.collision)
			{changeColor[5] = 1;}
		else if(sideFace6Col.collision)
			{changeColor[6] = 1;}
		else if(sideFace7Col.collision)
			{changeColor[7] = 1;}
		else if(sideFace8Col.collision)
			{changeColor[8] = 1;}
		else if(octTopCol.collision)
			{changeColor[9] = 1;}
		else if(octBotCol.collision)
			{changeColor[10] = 1;}
		
		glutPostRedisplay();
		glutIdleFunc(NULL);
	}//end else
	
	
}//end particleMotion

void pauseMotion(unsigned char key, int x, int y) //Press S/s to start, i/I to pause
{	
	switch (key) {
		case 's':
			if (isPaused) 
			{
				isPaused = false;
				glutIdleFunc(particleMotion);
				break;
			}
			
			
		case 'S':
			if (isPaused) 
			{
				isPaused = false;
				glutIdleFunc(particleMotion);
				break;
			}
			
		case 'i':
			if(!isPaused)
			{
				isPaused = true;
				glutIdleFunc(NULL);
			}//end if
			else
			{
				isPaused = false;
				glutIdleFunc(particleMotion);
			}//end else
			break;
			
		case 'I':
			if(!isPaused)
			{
				isPaused = true;
				glutIdleFunc(NULL);
			}//end if
			else
			{
				isPaused = false;
				glutIdleFunc(particleMotion);
			}//end else
			break;
	}//end switch
	
	
}//end pauseMotion

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) //change state of lbuttonDown
		{
			lbuttonDown = true;
			//cout << "\nleft button clicked\n";
		}
		
		else
		{
			lbuttonDown = false;
		}

	}
}//end mouse


void motion(int x, int y)
{
	
	oldPos[0] = currentPos[0];
	oldPos[1] = currentPos[1];
	
	currentPos[0] = x;
	currentPos[1] = y;
	
	if (lbuttonDown) 
	{
		//cout << "Mouse was dragged w/ left button."
		//<< "x: " << x << "y: " << y << endl;
		GLfloat x_component = currentPos[0] - oldPos[0];
		GLfloat y_component = currentPos[1] - oldPos[1];
		if(x_component > 0.0)
		{
			rotateAngleY += (0.008f) * x_component;
			glRotatef(rotateAngleY,0.0f,1.0f,0.0f);            // Rotate The Cube On Y
		}
		else 
		{
			rotateAngleY += (0.008f) * x_component;
			glRotatef(rotateAngleY,0.0f,-1.0f,0.0f);            // Rotate The Cube On Y
		}

		if(y_component > 0.0)
		{
			rotateAngleX += (0.008f) * y_component;
			glRotatef(rotateAngleX,1.0f,0.0f,0.0f);            // Rotate The Cube in y direction
		}
		else 
		{
			rotateAngleX += (0.008f) * y_component;
			glRotatef(rotateAngleX,-1.0f,0.0f,0.0f);          
		}
		
	}
	

	glutPostRedisplay();
	
}//end motion


void m_glewInitAndVersion(void)
{
	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
/*	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
*/
 }


