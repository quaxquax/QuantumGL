#include "ExpressionEvaluator.h"

class ReadEvaluator : public ExpressionEvaluator
{
	char			filename[256];
	vecN3			res;
	int				n;
	bool			c;
	bool			bin;
protected:
	virtual bool	AlwaysCreateField();
	virtual GeneralCubicData* 
					DoCreateField(vecN3 res = nullVecN3, bool forceRes = false);
public:
	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);
	
					ReadEvaluator(const char* fn);					
};
