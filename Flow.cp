#include "Flow.h"
#include "PointList.h"
#include "ExpressionEvaluator.h"
#include "LockGL.h"
#include "PredefinedVariables.h"

#include GL_GL_H
#include GL_GLU_H

#include <algorithm>
#include <functional>
					
Flow::Flow()
	: thePointList(NULL), theData(NULL),  
	theLength(NULL), theStepsize(NULL),
	lightingTextureObject(0),
	ambientWatcher(this)
{
	SetStageName("Flow");
	SetGLObjectsDirty();
	ambientVar->watchers.push_back(&ambientWatcher);
}
Flow::~Flow()
{
	release(thePointList);
	release(theData);
	release(theLength);
	release(theStepsize);
	if(lightingTextureObject)
	{
		LockGL();
		glDeleteTextures(1,&lightingTextureObject);
		UnlockGL();
	}
}

void Flow::SetPoints(PointList *points)
{
	autorelease(thePointList);
	thePointList = retain(points);
}

void Flow::SetData(ExpressionEvaluator *function)
{
	autorelease(theData);
	theData = retain(function);
}

void Flow::SetLength(ValueEvaluator *length)
{
	autorelease(theLength);
	theLength = retain(length);
}

void Flow::SetStepsize(CommonEvaluator *stepsize)
{
	autorelease(theStepsize);
	theStepsize = retain(stepsize);
}


void Flow::Draw(GLfloat camera[3], GLfloat light[3])
{
	assert(lightingTextureObject);
#if 0
	glBindTexture(GL_TEXTURE_1D,lightingTextureObject);
#else
	glBindTexture(GL_TEXTURE_2D,lightingTextureObject);
#endif
	
	GLfloat matrix[16];
	GLfloat nCamera[3];
	
	printf("camera: %10f %10f %10f\n",camera[0],camera[1],camera[2]);
	printf("light:  %10f %10f %10f\n",light[0],light[1],light[2]);

/*	matrix[0] = light[0]*0.49;	matrix[4] = light[1]*0.49; matrix[8] = light[2]*0.49; matrix[12] = 0.5;
	matrix[1] = 0;			matrix[5] = matrix[9]	= matrix[13] = 0;
	matrix[2] = 0;			matrix[6] = matrix[10]	= matrix[14] = 0;
	matrix[3] = 0;			matrix[7] = matrix[11]	= 0; matrix[15] = 1;*/

/*	matrix[0] = light[0]/2;	matrix[4] = matrix[8]	= matrix[12] = 0;
	matrix[1] = light[1]/2;	matrix[5] = matrix[9]	= matrix[13] = 0;
	matrix[2] = light[2]/2;	matrix[6] = matrix[10]	= matrix[14] = 0;
	matrix[3] = 0.5;		matrix[7] = matrix[11]	= 0; matrix[15] = 1;*/
	
	{
		GLfloat ilen = 1/sqrt(camera[0]*camera[0] + camera[1]*camera[1] + camera[2]*camera[2]);
		nCamera[0] = camera[0] * ilen;
		nCamera[1] = camera[1] * ilen;
		nCamera[2] = camera[2] * ilen;
	}
	
	matrix[0] = light[0] * 0.49;	matrix[4] = light[1]*0.49;		matrix[8] = light[2]*0.49; matrix[12] = 0.5;
	matrix[1] = nCamera[0] * 0.49;	matrix[5] = nCamera[1]*0.49;	matrix[9] = nCamera[2]*0.49; matrix[13] = 0.5;
	matrix[2] = 0;	matrix[6] = matrix[10]	= matrix[14] = 0;
	matrix[3] = 0;		matrix[7] = matrix[11]	= 0; matrix[15] = 1;
	
#define MATRIX_LIGHT 0
	
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
#if MATRIX_LIGHT
	glLoadMatrixf(matrix);
#endif
#if 0
	glEnable(GL_TEXTURE_1D);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP);
#else
	// ### HACK ### glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
	glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
#endif
	//glLineWidth(2);
	vector<Line>::iterator p = lines.begin(), e = lines.end();
	while(p != e)
#if MATRIX_LIGHT
		(p++)->Draw();
#else
		(p++)->Draw(nCamera,light);
#endif	
		
	glPopMatrix();

	if(0){
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,-1);
			glTexCoord2f(1,0);
			glVertex3f(1,-1,-1);
			glTexCoord2f(1,1);
			glVertex3f(1,1,-1);
			glTexCoord2f(0,1);
			glVertex3f(-1,1,-1);
		glEnd();
		glPopAttrib();
	}
	
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
}

void Flow::Build()
{
	assert(thePointList);
	lines.clear();
	
	vecR3 startingPoint;
	thePointList->StartIterating();
	while(thePointList->GetNextPoint(startingPoint))
		lines.insert(lines.end(),Line())->Build(this, startingPoint);
}

void Flow::BuildGLObjects1()
{
	if(!lightingTextureObject)
		glGenTextures(1,&lightingTextureObject);
#if 0
	glBindTexture(GL_TEXTURE_1D,lightingTextureObject);
	
	unsigned char tex[256];
	const float diffuse = 0.9, ambient = 0.1;
	
	for(int i=0;i<256;i++)
	{
		float t1 = i/255.0;
		float intensity = ambient+diffuse*sqrt(1-(2*t1-1)*(2*t1-1));
		tex[i] = (unsigned char) (intensity * 255);
		//tex[i] = i;
	}
	
	glTexImage1D(GL_TEXTURE_1D,0,GL_LUMINANCE,256,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,tex);
#else
	glBindTexture(GL_TEXTURE_2D,lightingTextureObject);
	
	unsigned char tex[256][256];
	
	float k_a = ambientVar->value;
	const float k_d = 1-k_a, k_s = 1;
	number shininess = -1;
	
	if(shininessExpr)
		shininessExpr -> EvaluateReal(1,&shininess);
	printf("build\n");
	for(int j=0;j<256;j++)
	{
		float t2 = j/255.0;		// t2 == 1/2 (V.T + 1)
		for(int i=0;i<256;i++)
		{
			float t1 = i/255.0;	// t1 == 1/2 (L.T + 1)
			
			// I = k_a + k_d * L.N + k_s * (V.R)^n
			// L ... vertex to light, unit
			// N ... normal vector
			// R ... reflected light vector
			// n ... shininess

			// I = k_a + k_d * sqrt(1-(L.T)^2) + k_s * ((L.T)(V.T) - sqrt(1-(L.T)^2)*sqrt(1-(V.T)^2))^n
			
			// T ... tangent
			
			float LT = 2*t1 - 1;
			float VT = 2*t2 - 1;
			
			float diffuse = sqrt(1-LT*LT);
			float specular = shininess >= 0 ? pow(LT*VT-sqrt((1-LT*LT)*(1-VT*VT)),shininess) : 0;
			
			float intensity = k_a + k_d*diffuse + k_s * specular;
			
			if(intensity > 1)
				intensity = 1;
			
			//tex[j][i] = (unsigned char) (intensity * 255);
			//tex[j][i] = (i%2) == (j%2) ? 255 : 0;//###
			tex[j][i] = 0;
		}
	}
	glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,256,256,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,tex);
#endif
}

static void normvec(number vec[3])
{
	number len = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
	if(len == 0)
		return;
	vec[0] /= len, vec[1] /= len, vec[2] /= len;
}

void Flow::Line::Build(Flow *flow, vecR3 startingPoint)
{
	this->flow = flow;
	
	points.clear();
	colors.clear();
	
	number h = 0.05;	/* value mentioned in documentation! */
	ExpressionEvaluator *dynamicH = NULL;
	
	number maximumLength = 10;	/* value mentioned in documentation! */
	
	if(flow->theLength)
		flow->theLength->EvaluateReal(1,&maximumLength);

	if(flow->theStepsize)
	{
		if(flow->theStepsize->IsField())
			dynamicH = dynamic_cast<ExpressionEvaluator*>(flow->theStepsize);
		else
		{
			ValueEvaluator *hVal = dynamic_cast<ValueEvaluator*>(flow->theStepsize);
			assert(hVal);
			hVal->EvaluateReal(1,&h);
		}
	}
	
	vecR3 point = startingPoint;
	
	//glBegin(GL_LINE_STRIP);
	for(number lengthSoFar = 0; lengthSoFar < maximumLength; lengthSoFar += h)
	{
			// abort if outside
		if(point.x < -1 || point.y < -1 || point.z < -1
			|| point.x > 1 || point.y > 1 || point.z > 1)
			break;
			
		if(dynamicH)
			dynamicH->EvaluateReal(point,1,&h);
			
		/*printf("point: %f %f %f\n",point.x,point.y,point.z);
		printf("vec: %f %f %f\n",vec[0],vec[1],vec[2]);
		printf("len: %f\n",len);*/

			// set color
		number color[3] = {1,1,0};
		if(flow->colorFunction)
			flow->colorFunction->EvaluateReal(point,3,color);

		//glColor3fv(color);
		vecR3 colorR3;
		colorR3.x = color[0]; colorR3.y = color[1]; colorR3.z = color[2];
		colors.push_back(colorR3);
		
			// draw line
		points.push_back(point);
		//glVertex3f(point.x,point.y,point.z);
	
		vecR3 tangent;
#if 0	/* euler */
			// evaluate field at point
		number vec[3];
		flow->theData->EvaluateReal(point,3,vec);
		normvec(vec);
						
			// Euler
		tangent.x = h*vec[0];
		tangent.y = h*vec[1];
		tangent.z = h*vec[2];
#elif 1 /* runge kutta */
			// evaluate field at point
		number k1[3];
		flow->theData->EvaluateReal(point,3,k1);
		normvec(k1);
		
		vecR3 tmp;
		number k2[3];
		tmp.x = point.x + h/2*k1[0];
		tmp.y = point.y + h/2*k1[1];
		tmp.z = point.z + h/2*k1[2];
		flow->theData->EvaluateReal(tmp,3,k2);
		normvec(k2);

		number k3[3];
		tmp.x = point.x + h/2*k2[0];
		tmp.y = point.y + h/2*k2[1];
		tmp.z = point.z + h/2*k2[2];
		flow->theData->EvaluateReal(tmp,3,k3);
		normvec(k3);

		number k4[3];
		tmp.x = point.x + h*k3[0];
		tmp.y = point.y + h*k3[1];
		tmp.z = point.z + h*k3[2];
		flow->theData->EvaluateReal(tmp,3,k4);
		normvec(k4);
		
		tangent.x = h/6*(k1[0]+2*k2[0]+2*k3[0]+k4[0]);
		tangent.y = h/6*(k1[1]+2*k2[1]+2*k3[1]+k4[1]);
		tangent.z = h/6*(k1[2]+2*k2[2]+2*k3[2]+k4[2]);
#endif

		point.x += tangent.x;
		point.y += tangent.y;
		point.z += tangent.z;
		number len = sqrt(tangent.x*tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
		if(len == 0)
		{
			if(tangents.size())
				tangents.push_back(tangents.back());
			else
				tangents.push_back(tangent);
			break;
		}
		len = 1/len;
		tangent.x *= len; tangent.y *= len; tangent.z *= len;
		tangents.push_back(tangent);
	}
	
	//glEnd();
}

void Flow::Line::Draw(GLfloat *camera, GLfloat *light)
{
	glBegin(GL_LINE_STRIP);
	vector<vecR3>::iterator p,c,t,e;
	
	p = points.begin();
	c = colors.begin();
	t = tangents.begin();
	e = points.end();
	
	/* ### HACK ### */
	float k_a = ambientVar->value;
	const float k_d = 1-k_a, k_s = 1;
	number shininess = -1;
	
	if(flow->shininessExpr)
		flow->shininessExpr -> EvaluateReal(1,&shininess);
	/* ### HACK ### */

	while(p != e)
	{
		vecR3 point = *p++;
		vecR3 color = *c++;
		vecR3 tangent = *t++;
		glColor3f(color.x,color.y,color.z);
		if(light)
		{
			//printf("%g,%g\n",(tangent.x*light[0] + tangent.y*light[1] + tangent.z*light[2])/2+0.5,
			//			(tangent.x*camera[0] + tangent.y*camera[1] + tangent.z*camera[2])/2+0.5);
			//glTexCoord2f((tangent.x*light[0] + tangent.y*light[1] + tangent.z*light[2])/2+0.5,
			//			(tangent.x*camera[0] + tangent.y*camera[1] + tangent.z*camera[2])/2+0.5);
			
				// HACK: don't use textures
			float t1 = (tangent.x*light[0] + tangent.y*light[1] + tangent.z*light[2])/2+0.5;
			float t2 = (tangent.x*camera[0] + tangent.y*camera[1] + tangent.z*camera[2])/2+0.5;
			float LT = 2*t1 - 1;
			float VT = 2*t2 - 1;
			
			float diffuse = sqrt(1-LT*LT);
			float specular = shininess >= 0 ? pow(LT*VT-sqrt((1-LT*LT)*(1-VT*VT)),shininess) : 0;
			
#if 0			
			float intensity = k_a + k_d*diffuse + k_s * specular;
			if(intensity > 1)
				intensity = 1;
			glColor3f(color.x * intensity,color.y * intensity,color.z * intensity);
#elif 1
			float red = color.x * (k_a + k_d * diffuse + k_s * specular);
			float green = color.y * (k_a + k_d * diffuse + k_s * specular);
			float blue = color.z * (k_a + k_d * diffuse + k_s * specular);

			if(red > 1) red = 1;
			if(green > 1) green = 1;
			if(blue > 1) blue = 1;

			glColor3f(red,green,blue);
#else
			float red = color.x * (k_a + k_d * diffuse) + k_s * specular;
			float green = color.y * (k_a + k_d * diffuse) + k_s * specular;
			float blue = color.z * (k_a + k_d * diffuse) + k_s * specular;

			if(red > 1) red = 1;
			if(green > 1) green = 1;
			if(blue > 1) blue = 1;

			glColor3f(red,green,blue);
#endif
		}
		else
			glTexCoord3f(tangent.x,tangent.y,tangent.z);
		glVertex3f(point.x,point.y,point.z);
	}
	glEnd();
}

void Flow::AmbientWatcher::VariableChanged(RealVariable *var)
{
	theFlow->SetGLObjectsDirty();
}
