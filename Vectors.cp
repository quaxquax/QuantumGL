#include "Vectors.h"
#include "PointList.h"
#include "ExpressionEvaluator.h"

#include GL_GL_H
#include GL_GLU_H
					
Vectors::Vectors()
	: thePointList(NULL), theData(NULL), secondaryColor(NULL),
		sphereColor(NULL), sphereSize(NULL)
{
	/*vecR3 corner = {-1,-1,-1};
	vecR3 delta = {2.0/5,2.0/5,2.0/5};
	vecN3 count = {6,6,6};*/
	SetStageName("Vectors");
}
Vectors::~Vectors()
{
	release(thePointList);
	release(theData);
	release(secondaryColor);
	release(sphereColor);
	release(sphereSize);
}

void Vectors::SetPoints(PointList *points)
{
	autorelease(thePointList);
	thePointList = retain(points);
}

void Vectors::SetData(ExpressionEvaluator *function)
{
	autorelease(theData);
	theData = retain(function);
}
void Vectors::SetSecondaryColor(ExpressionEvaluator *f)
{
	autorelease(secondaryColor);
	secondaryColor = retain(f);
}
void Vectors::SetSphereColor(ExpressionEvaluator *f)
{
	autorelease(sphereColor);
	sphereColor = retain(f);
}
void Vectors::SetSphereSize(ExpressionEvaluator *f)
{
	autorelease(sphereSize);
	sphereSize = retain(f);
}


inline bool iszero(number x)
{
	return x < 1e-3 && x > -1e-3;
}

static void anyNormal(number n[3], number a[3])
{
	if(!iszero(a[2]))
	{
		n[0] = 1;
		n[1] = 0;
		n[2] = -a[0]/a[2];
		// a[0] + n[2]*a[2] = 0
	}
	else if(!iszero(a[1]))
	{
		n[0] = 1;
		n[1] = -a[0]/a[1];
		n[2] = 0;
		// a[0] + n[1]+a[1] = 0

	}
	else if(!iszero(a[0]))
	{
		n[0] = -a[1]/a[0];
		n[1] = 1;
		n[2] = 0;
	}
	else
		n[0] = n[1] = n[2] = 0;
}


void Vectors::Draw(GLfloat camera[3], GLfloat light[3])
{
	vecR3 point;
	
	GLUquadric *quadric = gluNewQuadric();
	
	assert(thePointList);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glPointSize(5);
	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	SetupShininess();
	
	/*glColor3f(1,0,0);

	//glBegin(GL_POINTS);
		thePointList->StartIterating();
		while(thePointList->GetNextPoint(point))
		{
			//glVertex3f(point.x,point.y,point.z);
			glPushMatrix();
			glTranslatef(point.x,point.y,point.z);
			gluSphere(quadric,0.02,4,4);
			glPopMatrix();
		}
	//glEnd();*/
	
	/*glDisable(GL_BLEND);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		thePointList->StartIterating();
		while(thePointList->GetNextPoint(point))
		{
			number vec[3];
			theData->EvaluateReal(point,3,vec);
			glVertex3f(point.x,point.y,point.z);
			glVertex3f(point.x + vec[0],point.y + vec[1], point.z + vec[2]);
		}
	glEnd();*/

	thePointList->StartIterating();
	while(thePointList->GetNextPoint(point))
	{
		number vec[3];
		theData->EvaluateReal(point,3,vec);
		number len = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
		vec[0] /= len, vec[1] /= len, vec[2] /= len;

		number u[3],v[3];
		anyNormal(u,vec);
		number ulen = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
		u[0] /= ulen, u[1] /= ulen, u[2] /= ulen;
		v[0] = vec[1]*u[2] - vec[2]*u[1];
		v[1] = vec[2]*u[0] - vec[0]*u[2];
		v[2] = vec[0]*u[1] - vec[1]*u[0];

		/*cout << "u:   " << u[0] << "\t" << u[1] << "\t" << u[2] << endl;
		cout << "v:   " << v[0] << "\t" << v[1] << "\t" << v[2] << endl;
		cout << "vec: " << vec[0] << "\t" << vec[1] << "\t" << vec[2] << endl;*/

		/*glVertex3f(point.x,point.y,point.z);
		glVertex3f(point.x + vec[0],point.y + vec[1], point.z + vec[2]);*/

		number radius = len*0.1;

		number color[3] = {1,1,0};
		if(colorFunction)
			colorFunction->EvaluateReal(point,3,color);

		glColor3fv(color);

		glPushMatrix();
			glTranslatef(point.x,point.y,point.z);

			
			glPushMatrix();
				GLfloat mat[16] = {u[0],u[1],u[2],0,
									v[0],v[1],v[2],0,
									vec[0],vec[1],vec[2],0,
									0,0,0,1};
				glMultMatrixf(mat);
				glPushMatrix();
					glTranslatef(0,0,len*0.5);
					gluCylinder(quadric,radius,0,len*0.5,8,1);
				glPopMatrix();
				if(secondaryColor)
				{
					secondaryColor->EvaluateReal(point,3,color);
					glColor3fv(color);
				}
				gluCylinder(quadric,0,radius,len*0.5,8,1);
				glPopMatrix();
			if(sphereColor)
			{
				sphereColor->EvaluateReal(point,3,color);
				glColor3fv(color);
			}
			number r = 0.02;
			if(sphereSize)
				sphereSize->EvaluateReal(point,1,&r);
			if(r > 0)
				gluSphere(quadric,r,6,6);
		glPopMatrix();
	}
	/*glColor3f(1,0,0);
	glPushMatrix();
		glTranslatef(1,0,0);
		gluSphere(quadric,0.03,5,5);
	glPopMatrix();
	glColor3f(0,1,0);
	glPushMatrix();
		glTranslatef(0,1,0);
		gluSphere(quadric,0.03,5,5);
	glPopMatrix();
	glColor3f(0,0,1);
	glPushMatrix();
		glTranslatef(0,0,1);
		gluSphere(quadric,0.03,5,5);
	glPopMatrix();*/

	glPopAttrib();

	gluDeleteQuadric(quadric);
}

void Vectors::Build()
{
}
