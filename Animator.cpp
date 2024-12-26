#include "QuantumConfig.h"

#include "Animator.h"

#include "RealVariable.h"
#include "ImageExporter.h"

#include <iostream>
#include <cstdio>
using namespace std;

bool ExportImage(const char * name = NULL);

void AnimatorVariableSpec::SetVariable(int frame, int n)
{
	number completion;
	if(inclusive)
		completion = frame / number(n-1);
	else
		completion = frame / number(n);
	var->ChangeTo(range.first + completion*(range.second-range.first));
	cout << "\t\t" << var->name << " = " << var->value << endl;
}

bool AnimationSection::GenerateAnimation(int startingFrame)
{
	for(int i=0;i<frames;i++)
	{
		char s[256];
		sprintf(s,"%03d.pic",i+startingFrame);
		vector<AnimatorVariableSpec>::iterator p;
		
		cout << "************ ANIMATION RENDERING FRAME " <<
				i << " of " << frames << " ************\n";
		for(p = vars.begin();p < vars.end();++p)
		{
			p->SetVariable(i,frames);
		}
		if(!ExportImage(s))
			return false;
	}
	return true;
}

void AnimationSection::SetFrame(int frame)
{
	vector<AnimatorVariableSpec>::iterator p;
	for(p = vars.begin();p < vars.end();++p)
		p->SetVariable(frame,frames);
}

void Animator::GenerateAnimation()
{
	vector<AnimationSection>::iterator p;
	int frame = 1;
	for(p = sections.begin();p < sections.end();++p)
	{
		if(!p->GenerateAnimation(frame))
			return;
		frame += p->frames;
	}
}

int Animator::GetFrameCount()
{
	vector<AnimationSection>::iterator p;
	int frames = 0;
	for(p = sections.begin();p < sections.end();++p)
		frames += p->frames;
	return frames;
}

void Animator::SetFrame(int f)
{
	vector<AnimationSection>::iterator p;
	for(p = sections.begin();p < sections.end();++p)
	{
		if(f < p->frames)
		{
			p->SetFrame(f);
			return;
		}
		f -= p->frames;
	}
}
