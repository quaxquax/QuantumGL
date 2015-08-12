#include "Slice.h"

#include "ExpressionEvaluator.h"
#include "BSPTree.h"
#include "LockGL.h"

#include GL_GL_H
#include GL_GLU_H

extern Range worldXRange, worldYRange, worldZRange;

Slice::Slice()
	: plane(0), coordValueExpr(NULL)
{
	displayListIndex = 0;
	displayListDirty = true;
	textureObject = 0;
	
	useTextures = true;
	frameSlice = false;
	
	SetStageName("Slice");
}

Slice::~Slice()
{
	if(coordValueExpr)
		coordValueExpr->DecRefCount();
	if(displayListIndex)
		glDeleteLists(displayListIndex,1);
	if(textureObject)
	{
		LockGL();
		glDeleteTextures(1,&textureObject);
		UnlockGL();
	}
}

void Slice::Draw(GLfloat camera[3], GLfloat light[3])
{
#if 1
	if(transparencyExpr && !frameSlice)
		return;
		
	if(!displayListIndex)
	{
		displayListIndex = glGenLists(1);	
		displayListDirty = true;
	}
	if(displayListDirty)
	{
		displayListDirty = false;

		
		glNewList(displayListIndex,GL_COMPILE_AND_EXECUTE);
		
			number coordVal = 0;
			if(coordValueExpr)
				coordValueExpr->EvaluateReal(1,&coordVal);
			Range coordRanges[] = { xRange, yRange, zRange };

			if(coordVal < coordRanges[plane].first)
				coordVal = coordRanges[plane].first;
			else if(coordVal > coordRanges[plane].second)
				coordVal = coordRanges[plane].second;
			
			int iRes = 2;
			int jRes = 2;
			
			GLfloat normal[3] = {0,0,0};
			
			normal[plane] = 1;
			
			GLfloat zero_zero[3] = {0,0,0};
			
			zero_zero[0] += coordVal * normal[0];
			zero_zero[1] += coordVal * normal[1];
			zero_zero[2] += coordVal * normal[2];
			
			GLfloat iDir[3] = {0,0,0};
			GLfloat jDir[3] = {0,0,0};
		
			if(plane == 0)
				iDir[1] = jDir[2] = 1;
			else if(plane == 1)
				iDir[2] = jDir[0] = 1;
			else
				iDir[0] = jDir[1] = 1;
			
			if(colorFunction && !useTextures)
			{
				vecN3 res;
				if(colorFunction->GetDefaultResolution(res))
				{
					if(plane == 0)
						iRes = res.y, jRes = res.z;
					else if(plane == 1)
						iRes = res.z, jRes = res.x;
					else
						iRes = res.x, jRes = res.y;
				}
			}
			
			zero_zero[0] += (iDir[0] + jDir[0]) * xRange.first;
			zero_zero[1] += (iDir[1] + jDir[1]) * yRange.first;
			zero_zero[2] += (iDir[2] + jDir[2]) * zRange.first;
			
			iDir[0] *= xRange.size() / (iRes-1);
			iDir[1] *= yRange.size() / (iRes-1);
			iDir[2] *= zRange.size() / (iRes-1);
			
			jDir[0] *= xRange.size()  / (jRes-1);
			jDir[1] *= yRange.size()  / (jRes-1);
			jDir[2] *= zRange.size()  / (jRes-1);
		
			if(frameSlice)
			{
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				//glPolygonOffset(1.0,1.0);
				//glEnable(GL_POLYGON_OFFSET_FILL);
				
				glBegin(GL_QUADS);
					glColor3f(1,1,1);
					glVertex3fv(zero_zero);
					glVertex3f(zero_zero[0] + (iRes-1)*iDir[0],
								zero_zero[1] + (iRes-1)*iDir[1],
								zero_zero[2] + (iRes-1)*iDir[2]);
					glVertex3f(zero_zero[0] + (iRes-1)*iDir[0] + (jRes-1)*jDir[0],
								zero_zero[1] + (iRes-1)*iDir[1] + (jRes-1)*jDir[1],
								zero_zero[2] + (iRes-1)*iDir[2] + (jRes-1)*jDir[2]);
					glVertex3f(zero_zero[0] + (jRes-1)*jDir[0],
								zero_zero[1] + (jRes-1)*jDir[1],
								zero_zero[2] + (jRes-1)*jDir[2]);
				glEnd();
				//glDisable(GL_POLYGON_OFFSET_LINE);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			}
		
			if(!transparencyExpr)
			{
				GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, /*0.5*/1 };
				glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
				glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
				SetupShininess();				
				// glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
				//            (the default value)
				
				glEnable(GL_COLOR_MATERIAL);
				glColor3f(1,1,1);
				glEnable(GL_LIGHTING);
						
				if(useTextures)
				{
					glBindTexture(GL_TEXTURE_2D,textureObject);
					glEnable(GL_TEXTURE_2D);
				}
				
				for(int i=0;i<iRes-1;i++)
				{
					glBegin(GL_QUAD_STRIP);
					glNormal3fv(normal);
					for(int j=0;j<jRes;j++)
					{
						vecR3 v;
						v.x = zero_zero[0] + iDir[0] * i + jDir[0] * j;
						v.y = zero_zero[1] + iDir[1] * i + jDir[1] * j;
						v.z = zero_zero[2] + iDir[2] * i + jDir[2] * j;
			
						vecR3 color;

						if(useTextures)
							glTexCoord2f(GLfloat(i)/(iRes-1), GLfloat(j)/(jRes-1));
						else
						{
							if(colorFunction)
								colorFunction->EvaluateReal(v,3,(number*)(&color));
							else
								color.x = 1, color.y = color.z = 0;
							glColor3f(color.x,color.y,color.z);
						}
						glVertex3f(v.x,v.y,v.z);
						
						v.x += iDir[0];
						v.y += iDir[1];
						v.z += iDir[2];
			
						if(useTextures)
							glTexCoord2f(GLfloat(i+1)/(iRes-1), GLfloat(j)/(jRes-1));
						else
						{
							if(colorFunction)
								colorFunction->EvaluateReal(v,3,(number*)(&color));
							else
								color.x = 1, color.y = color.z = 0;
							glColor3f(color.x,color.y,color.z);
						}
						glVertex3f(v.x,v.y,v.z);
					}
					glEnd();
				}
			
				glDisable(GL_LIGHTING);
				if(useTextures)
				{
					glDisable(GL_TEXTURE_2D);
				}
			}
		glEndList();
	}
	else
	{
		glCallList(displayListIndex);
	}
#endif
}

void Slice::Build()
{
	if(useTextures)
		SetGLObjectsDirty();
	displayListDirty = true;
}

void Slice::SetGLObjectsDirty()
{
	VisualObject::SetGLObjectsDirty();
	displayListDirty = true;
}

void Slice::BuildGLObjects1()
{
	if(colorFunction)
		colorFunction->GetRanges(xRange,yRange,zRange);
	else
		xRange = yRange = zRange = fullRange;
	if(xRange == fullRange)
	{
		xRange = worldXRange;
		yRange = worldYRange;
		zRange = worldZRange;
	}
	if(useTextures)
	{
		number coordVal = 0;
		if(coordValueExpr)
			coordValueExpr->EvaluateReal(1,&coordVal);
		Range coordRanges[] = { xRange, yRange, zRange };

		if(coordVal < coordRanges[plane].first)
			coordVal = coordRanges[plane].first;
		else if(coordVal > coordRanges[plane].second)
			coordVal = coordRanges[plane].second;
		
		int iRes = 32;
		int jRes = 32;
		
		GLfloat normal[3] = {0,0,0};
		
		normal[plane] = 1;
		
		GLfloat zero_zero[3] = {0,0,0};
		
		zero_zero[0] += coordVal * normal[0];
		zero_zero[1] += coordVal * normal[1];
		zero_zero[2] += coordVal * normal[2];
		
		GLfloat iDir[3] = {0,0,0};
		GLfloat jDir[3] = {0,0,0};
	
		if(plane == 0)
			iDir[1] = jDir[2] = 1;
		else if(plane == 1)
			iDir[2] = jDir[0] = 1;
		else
			iDir[0] = jDir[1] = 1;
		
		if(colorFunction)
		{
			vecN3 res;
			if(colorFunction->GetDefaultResolution(res))
			{
				if(plane == 0)
					iRes = res.y, jRes = res.z;
				else if(plane == 1)
					iRes = res.z, jRes = res.x;
				else
					iRes = res.x, jRes = res.y;
			}
		}
		{
			int tmpRes;
			tmpRes = 1;
			while(tmpRes < iRes)
				tmpRes *= 2;
			iRes = tmpRes;
			tmpRes = 1;
			while(tmpRes < jRes)
				tmpRes *= 2;
			jRes = tmpRes;
		}
		
		zero_zero[0] += (iDir[0] + jDir[0]) * xRange.first;
		zero_zero[1] += (iDir[1] + jDir[1]) * yRange.first;
		zero_zero[2] += (iDir[2] + jDir[2]) * zRange.first;
		
		iDir[0] *= xRange.size() / (iRes-1);
		iDir[1] *= yRange.size() / (iRes-1);
		iDir[2] *= zRange.size() / (iRes-1);
		
		jDir[0] *= xRange.size()  / (jRes-1);
		jDir[1] *= yRange.size()  / (jRes-1);
		jDir[2] *= zRange.size()  / (jRes-1);

		GLubyte *texture = new GLubyte [iRes*jRes*3];
		
		for(int j=0;j<jRes;j++)
		{
			for(int i=0;i<iRes;i++)
			{
				vecR3 v;
				v.x = zero_zero[0] + iDir[0] * i + jDir[0] * j;
				v.y = zero_zero[1] + iDir[1] * i + jDir[1] * j;
				v.z = zero_zero[2] + iDir[2] * i + jDir[2] * j;
	
				vecR3 color;

				if(colorFunction)
					colorFunction->EvaluateReal(v,3,(number*)(&color));
				else
					color.x = 1, color.y = color.z = 0;

				if(!((color.x >= 0)
					&&	(color.y >= 0)
					&&	(color.z >= 0)
					&&	(color.x <= 1)
					&&	(color.y <= 1)
					&&	(color.z <= 1)))
				{
					printf("barf!\n");
				}

				texture[(i + j*iRes)*3 + 0] = (int) (255*color.x);
				texture[(i + j*iRes)*3 + 1] = (int) (255*color.y);
				texture[(i + j*iRes)*3 + 2] = (int) (255*color.z);
			}
		}
		
		if(!textureObject)
			glGenTextures(1,&textureObject);
		
		glBindTexture(GL_TEXTURE_2D,textureObject);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iRes, jRes, 0,
		//	GL_RGB, GL_UNSIGNED_BYTE, texture);
		gluBuild2DMipmaps(GL_TEXTURE_2D,
						GL_RGB,
						iRes, jRes,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						texture);
			
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		
		delete [] texture;
	}
}

void Slice::SetCoordValue(int p,ValueEvaluator* coord)
{
	if(coordValueExpr)
		coordValueExpr->DecRefCount();
	plane = p;
	coordValueExpr = retain(coord);
}

void Slice::SubmitToBSP()
{
	VisualObject::SubmitToBSP();
#if 1
	if(!transparencyExpr)
		return;
		
	number coordVal = 0;
	if(coordValueExpr)
		coordValueExpr->EvaluateReal(1,&coordVal);
	Range coordRanges[] = { xRange, yRange, zRange };

	if(coordVal < coordRanges[plane].first)
		coordVal = coordRanges[plane].first;
	else if(coordVal > coordRanges[plane].second)
		coordVal = coordRanges[plane].second;

	GLfloat normal[3] = {0,0,0};
	
	normal[plane] = 1;
	
	GLfloat zero_zero[3] = {0,0,0};
	
	zero_zero[0] += coordVal * normal[0];
	zero_zero[1] += coordVal * normal[1];
	zero_zero[2] += coordVal * normal[2];
	
	GLfloat iDir[3] = {0,0,0};
	GLfloat jDir[3] = {0,0,0};

	if(plane == 0)
		iDir[1] = jDir[2] = 1;
	else if(plane == 1)
		iDir[2] = jDir[0] = 1;
	else
		iDir[0] = jDir[1] = 1;
	
	zero_zero[0] += (iDir[0] + jDir[0]) * xRange.first;
	zero_zero[1] += (iDir[1] + jDir[1]) * yRange.first;
	zero_zero[2] += (iDir[2] + jDir[2]) * zRange.first;
	
	iDir[0] *= xRange.size();
	iDir[1] *= yRange.size();
	iDir[2] *= zRange.size();
	
	jDir[0] *= xRange.size();
	jDir[1] *= yRange.size();
	jDir[2] *= zRange.size();

	
	/*GLfloat trans;
	{
		number tmp;
		transparencyExpr->EvaluateReal(1,&tmp);
		trans = Range(0,1).clamp((GLfloat)tmp);
	}*/
	
	BSPTree::Vertex vertices[4] = 
		{
			{
					{0, 0},
					{1, 1, 1, 0},
					{normal[0], normal[1], normal[2]},
					{zero_zero[0], zero_zero[1], zero_zero[2]},
			},
			{
					{1, 0},
					{1, 1, 1, 0},
					{normal[0], normal[1], normal[2]},
					{zero_zero[0]+iDir[0], zero_zero[1]+iDir[1], zero_zero[2]+iDir[2]},
			},
			{
					{1, 1},
					{1, 1, 1, 0},
					{normal[0], normal[1], normal[2]},
					{zero_zero[0]+iDir[0]+jDir[0], zero_zero[1]+iDir[1]+jDir[1], zero_zero[2]+iDir[2]+jDir[2]}
			},
			{
					{0, 1},
					{1, 1, 1, 0},
					{normal[0], normal[1], normal[2]},
					{zero_zero[0]+jDir[0], zero_zero[1]+jDir[1], zero_zero[2]+jDir[2]},
			}
		};
	BSP->SubmitPolygon(4,vertices,mainMaterial);
#endif
}

void Slice::UpdateMaterials()
{
	VisualObject::UpdateMaterials();
	BSPTree::Material& m = BSP->GetMaterial(mainMaterial);
	m.texture = textureObject;
}

bool Slice::GetObjectRanges(Range& xr, Range& yr, Range& zr)
{
	if(!colorFunction)
		return false;
	colorFunction->GetRanges(xr,yr,zr);
	return true;
}
