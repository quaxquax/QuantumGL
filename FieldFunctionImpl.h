#ifndef _H_FIELD_FUNCTION_H
#define _H_FIELD_FUNCTION_H

#include "FieldFunction.h"
#include "FieldFunctionUser.h"

#include "RealVariable.h"

#include <vector>
using std::vector;

#include <assert.h>

typedef CommonEvaluator*	FieldArgList[10];
typedef RealVariable*			RVarArgList[5];

template <class Arg> class ArgConstructor
{
public:
	static Arg construct(CommonEvaluator* eval)
	{
		return Arg(eval);
	}
};

template <> class ArgConstructor<Real>
{
public:
	static Real construct(CommonEvaluator* eval)
	{
		ValueEvaluator* e = dynamic_cast<ValueEvaluator*>(eval);
		assert(e);
		assert(e->Dimensions() == 1);
		assert(!e->IsComplex());
		Real tmp;
		e->EvaluateReal(1,&tmp);
		return tmp;
	}
};

template <> class ArgConstructor<Complex>
{
public:
	static Complex construct(CommonEvaluator* eval)
	{
		ValueEvaluator* e = dynamic_cast<ValueEvaluator*>(eval);
		assert(e);
		assert(e->Dimensions() == 1);
		Complex tmp;
		e->EvaluateComplex(1,&tmp);
		return tmp;
	}
};

template <int n> class ArgConstructor< vec<Real,n> >
{
public:
	static vec<Real,n> construct(CommonEvaluator* eval)
	{
		ValueEvaluator* e = dynamic_cast<ValueEvaluator*>(eval);
		assert(e);
		assert(e->Dimensions() == n);
		assert(!e->IsComplex());
		vec<Real,n> tmp;
		e->EvaluateReal(n,tmp.directAccess);
		return tmp;
	}
};

template <int n> class ArgConstructor< vec<Complex,n> >
{
public:
	static vec<Complex,n> construct(CommonEvaluator* eval)
	{
		ValueEvaluator* e = dynamic_cast<ValueEvaluator*>(eval);
		assert(e);
		assert(e->Dimensions() == n);
		vec<Complex,n> tmp;
		e->EvaluateComplex(n,tmp.directAccess);
		return tmp;
	}
};



#define FF_CLASS_FIELD_0
#define FF_CLASS_FIELD_1	, class Field1
#define FF_CLASS_FIELD_2	, class Field1, class Field2
#define FF_CLASS_FIELD_3	, class Field1, class Field2, class Field3 
#define FF_CLASS_FIELD_4	, class Field1, class Field2, class Field3, class Field4
#define FF_CLASS_FIELD_5	, class Field1, class Field2, class Field3, class Field4, class Field5
#define FF_CLASS_FIELD_6	, class Field1, class Field2, class Field3, class Field4, class Field5 \
							, class Field6
#define FF_CLASS_FIELD_7	, class Field1, class Field2, class Field3, class Field4, class Field5 \
							, class Field6, class Field7

#define FF_CLASS_FIELD(n)	FF_CLASS_FIELD_ ## n

#define FF_FIELD_0
#define FF_FIELD_1			, Field1
#define FF_FIELD_2			, Field1, Field2
#define FF_FIELD_3			, Field1, Field2, Field3
#define FF_FIELD_4			, Field1, Field2, Field3, Field4
#define FF_FIELD_5			, Field1, Field2, Field3, Field4, Field5
#define FF_FIELD_6			, Field1, Field2, Field3, Field4, Field5, Field6
#define FF_FIELD_7			, Field1, Field2, Field3, Field4, Field5, Field6, Field7

#define FF_FIELD(n)			FF_FIELD_ ## n

#define FF_FC(field,arg)	ArgConstructor<field>::construct(arg)

#define FF_FIELD_CALL_0
#define FF_FIELD_CALL_1		, FF_FC(Field1,fa[0])
#define FF_FIELD_CALL_2		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1])
#define FF_FIELD_CALL_3		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1]), FF_FC(Field3,fa[2])
#define FF_FIELD_CALL_4		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1]), FF_FC(Field3,fa[2]), FF_FC(Field4,fa[3])
#define FF_FIELD_CALL_5		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1]), FF_FC(Field3,fa[2]), FF_FC(Field4,fa[3]) \
							, FF_FC(Field5,fa[4])
#define FF_FIELD_CALL_6		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1]), FF_FC(Field3,fa[2]), FF_FC(Field4,fa[3]) \
							, FF_FC(Field5,fa[4]), FF_FC(Field6,fa[5])
#define FF_FIELD_CALL_7		, FF_FC(Field1,fa[0]), FF_FC(Field2,fa[1]), FF_FC(Field3,fa[2]), FF_FC(Field4,fa[3]) \
							, FF_FC(Field5,fa[4]), FF_FC(Field6,fa[5]), FF_FC(Field7,fa[6])

#define FF_FIELD_CALL(n)	FF_FIELD_CALL_ ## n

/*
#define FF_REAL_0
#define FF_REAL_1			, Real
#define FF_REAL_2			, Real, Real
#define FF_REAL_3			, Real, Real, Real
#define FF_REAL_4			, Real, Real, Real, Real
#define FF_REAL_5			, Real, Real, Real, Real, Real

#define FF_REAL(n)			FF_REAL_ ## n

#define FF_REAL_CALL_0		
#define FF_REAL_CALL_1		, rva[0]->value
#define FF_REAL_CALL_2		, rva[0]->value, rva[1]->value
#define FF_REAL_CALL_3		, rva[0]->value, rva[1]->value, rva[2]->value
#define FF_REAL_CALL_4		, rva[0]->value, rva[1]->value, rva[2]->value, rva[3]->value
#define FF_REAL_CALL_5		, rva[0]->value, rva[1]->value, rva[2]->value, rva[3]->value, rva[4]->value

#define FF_REAL_CALL(n)		FF_REAL_CALL_ ## n
*/

#define FF_COMMENT_IF_ZERO_0(x)
#define FF_COMMENT_IF_ZERO_1(x)		x
#define FF_COMMENT_IF_ZERO_2(x)		x
#define FF_COMMENT_IF_ZERO_3(x)		x
#define FF_COMMENT_IF_ZERO_4(x)		x
#define FF_COMMENT_IF_ZERO_5(x)		x
#define FF_COMMENT_IF_ZERO_6(x)		x
#define FF_COMMENT_IF_ZERO_7(x)		x

#define FF_COMMENT_IF_ZERO(n,x)		FF_COMMENT_IF_ZERO_ ## n(x)


#define DECLARE_ARGUMENT_ADAPTOR(n)														\
	template <class Ret FF_CLASS_FIELD(n)>												\
	class FFArgumentAdaptor_ ## n														\
	{																					\
	public:																				\
		typedef Ret	return_type;														\
		typedef Ret (*funType)(vec<Real,3> FF_FIELD(n));								\
		funType f;																		\
																						\
		FFArgumentAdaptor_ ## n(funType fun = NULL) : f(fun) {}							\
																						\
		inline int getN() { return n; }													\
																						\
		inline void precalculate(	FieldArgList FF_COMMENT_IF_ZERO(n,fa)) { }			\
		inline void invalidate()	{ }													\
																						\
		inline Ret operator() (vecR3 pos, 												\
								FieldArgList FF_COMMENT_IF_ZERO(n,fa))					\
		{																				\
			return (*f)(makeVecR(pos.x,pos.y,pos.z) FF_FIELD_CALL(n));					\
		}																				\
	};																					\
	template <class Ret FF_CLASS_FIELD(n)>												\
	inline FFArgumentAdaptor_ ## n <Ret FF_FIELD(n)>									\
	AdaptArg(Ret (*f)(vec<Real,3> FF_FIELD(n)))											\
	{																					\
		return FFArgumentAdaptor_ ## n <Ret FF_FIELD(n)> (f);							\
	}

DECLARE_ARGUMENT_ADAPTOR(0)
DECLARE_ARGUMENT_ADAPTOR(1)
DECLARE_ARGUMENT_ADAPTOR(2)
DECLARE_ARGUMENT_ADAPTOR(3)
DECLARE_ARGUMENT_ADAPTOR(4)
DECLARE_ARGUMENT_ADAPTOR(5)
DECLARE_ARGUMENT_ADAPTOR(6)
DECLARE_ARGUMENT_ADAPTOR(7)

#define DECLARE_ARGUMENT_DATA_ADAPTOR(n)												\
	template <class Ret, class Data FF_CLASS_FIELD(n)>									\
	class FFArgumentDataAdaptor_ ## n													\
	{																					\
	public:																				\
		typedef Ret	return_type;														\
		typedef void (*preFunType)(Data& FF_FIELD(n));									\
		typedef Ret (*funType)(vec<Real,3>, Data& FF_FIELD(n));							\
		preFunType f1;																	\
		funType f;																		\
		Data	*data;																	\
																						\
		FFArgumentDataAdaptor_ ## n(preFunType fun1 = NULL, 							\
												funType fun = NULL)						\
											 : f1(fun1), f(fun), data(NULL) {}			\
		~FFArgumentDataAdaptor_ ## n()	{ if(data) delete data; }						\
																						\
		inline int getN() { return n; }													\
																						\
		void precalculate(		FieldArgList FF_COMMENT_IF_ZERO(n,fa))					\
		{																				\
			if(data) delete data;														\
			data = new Data();															\
			(*f1)(*data FF_FIELD_CALL(n));												\
		}																				\
		void invalidate()																\
		{																				\
			if(data) delete data;														\
			data = NULL;																\
		}																				\
																						\
		inline Ret operator() (vecR3 pos, 												\
								FieldArgList FF_COMMENT_IF_ZERO(n,fa))					\
		{																				\
			if(!data) precalculate(fa,rva);												\
			return (*f)(makeVecR(pos.x,pos.y,pos.z), *data								\
						FF_FIELD_CALL(n));												\
		}																				\
	};																					\
	template <class Ret, class Data FF_CLASS_FIELD(n)>									\
	inline FFArgumentDataAdaptor_ ## n <Ret, Data FF_FIELD(n)>							\
	AdaptArg(void (*f1)(Data& FF_FIELD(n)),												\
				Ret (*f)(vec<Real,3>, Data& FF_FIELD(n)))								\
	{																					\
		return FFArgumentDataAdaptor_ ## n <Ret, Data FF_FIELD(n)> (f1,f);				\
	}

	
DECLARE_ARGUMENT_DATA_ADAPTOR(0)
DECLARE_ARGUMENT_DATA_ADAPTOR(1)
DECLARE_ARGUMENT_DATA_ADAPTOR(2)
DECLARE_ARGUMENT_DATA_ADAPTOR(3)
DECLARE_ARGUMENT_DATA_ADAPTOR(4)
DECLARE_ARGUMENT_DATA_ADAPTOR(5)
DECLARE_ARGUMENT_DATA_ADAPTOR(6)
DECLARE_ARGUMENT_DATA_ADAPTOR(7)


template <class T> class FFTypeInfo
{
public:
};

class FFTypeInfo<Real>
{
public:
	inline static bool IsComplex()	{ return false; }
	inline static int	Dimensions() { return 1; }
	typedef Complex	complexVariant;
	typedef Real baseType;
	inline static Real*	NumberPointer(Real& r) { return &r; }
};

class FFTypeInfo<Complex>
{
public:
	inline static bool IsComplex()	{ return true; }
	inline static int	Dimensions() { return 1; }
	typedef Complex	complexVariant;
	typedef Complex baseType;
	inline static Complex*	NumberPointer(Complex& c) { return &c; }
};

template <class T, int n> class FFTypeInfo< vec<T,n> >
{
public:
	inline static bool IsComplex() { return FFTypeInfo<T>::IsComplex(); }
	inline static int	Dimensions() { return n; }
	typedef vec<typename FFTypeInfo<T>::complexVariant,n>	complexVariant;
	typedef T baseType;
	inline static T*	NumberPointer(vec<T,n>& v) { return v.directAccess; }
};

template <class ArgAdaptor>
class CommonAdaptedEvaluator : public ExpressionEvaluator
{
protected:
	bool	c;
	int		n;
	
	ArgAdaptor		fun;
	FieldArgList	fArgs;
	int				nFArgs;
	
	typedef typename ArgAdaptor::return_type retType;
	typedef FFTypeInfo<retType>	info;
	
	void			init(ArgAdaptor	adaptor,
							const vector<CommonEvaluator*>& fieldArgs)
					{
						fun = adaptor;
						c = info::IsComplex();
						n = info::Dimensions();
					
						int i;
						nFArgs = fieldArgs.size();

						xRange = yRange = zRange = fullRange;
						if(nFArgs)
						{
							
							for(i=0;i<nFArgs;i++)
							{
								fArgs[i] = retain(fieldArgs[i]);

								if(ExpressionEvaluator* field = dynamic_cast<ExpressionEvaluator*>(fieldArgs[i]))
								{
									Range xr, yr, zr;
									field->GetRanges(xr,yr,zr);
									if(xRange == fullRange)
										xRange = xr, yRange = yr, zRange = zr;
									else if(xr == fullRange)
										;
									else
										if(xr != xRange || yr != yRange || zr != zRange)
										{
											/*throw RangeMismatch(xRange,yRange,zRange,
																xr,yr,zr);*/
											assert(false);//###
										}
								}
							}
						}
					}
public:
					CommonAdaptedEvaluator()
					{
					}
	virtual			~CommonAdaptedEvaluator()
					{
						int i;
						for(i=0;i<nFArgs;i++)
							fArgs[i]->DecRefCount();
					}

	virtual bool	IsComplex()
	{
		return c;
	}
	virtual int		Dimensions()
	{
		return n;
	}
	virtual bool	GetDefaultResolution(vecN3& res)
	{
		for(int i=0;i<nFArgs;i++)
		{
			ExpressionEvaluator *fieldEval = dynamic_cast<ExpressionEvaluator*>(fArgs[i]);
			if(fieldEval && fieldEval->GetDefaultResolution(res))
				return true;
		}
		return ExpressionEvaluator::GetDefaultResolution(res);
	}
};

template <class ArgAdaptor, class baseType>
class AdaptedEvaluator : public CommonAdaptedEvaluator<ArgAdaptor>
{
};

template <class ArgAdaptor>
class AdaptedEvaluator<ArgAdaptor, Real> : public CommonAdaptedEvaluator<ArgAdaptor>
{
public:
					AdaptedEvaluator(
							ArgAdaptor	adaptor,
							const vector<CommonEvaluator*>& fieldArgs)
					{
						init(adaptor, fieldArgs);
					}

	virtual void 	EvaluateReal(vecR3 pos, int n, number values[])
	{
		if(n != this->n)
			throw TypeMismatch();
			
		retType& ret = *((retType*)values);
										// ###! relies on layout of vec<>
		if(!IsEvaluationCached())
			fun.invalidate(), SetEvaluationCached();
		ret = fun(pos,fArgs);
	}
};

template <class ArgAdaptor>
class AdaptedEvaluator<ArgAdaptor, Complex> : public CommonAdaptedEvaluator<ArgAdaptor>
{
public:
					AdaptedEvaluator(
							ArgAdaptor	adaptor,
							const vector<CommonEvaluator*>& fieldArgs)
					{
						init(adaptor, fieldArgs);
					}

	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[])
	{
		if(n != this->n)
			throw TypeMismatch();
			
		retType& ret = *((retType*)values);
										// ###! relies on layout of vec<>
		if(!IsEvaluationCached())
			fun.invalidate(), SetEvaluationCached();
		ret = fun(pos,fArgs);
	}
};


class AdaptedEvaluatorFactory
{
	string		name;
	int			n;
public:
			AdaptedEvaluatorFactory(const string& theName, 
									int theN);

	virtual ExpressionEvaluator	*MakeEvaluator(
			const vector<CommonEvaluator*>& fieldArgs) = 0;
	
	bool	CheckArgs(int theN);
	
	string	GetName() { return name; }
	
	virtual ~AdaptedEvaluatorFactory() {}
};

template <class ArgAdaptor>
class AdaptedEvaluatorFactory_Ret : public AdaptedEvaluatorFactory
{
public:
	ArgAdaptor	adaptedFun;
	
	AdaptedEvaluatorFactory_Ret(const string& theName, ArgAdaptor fun)
		: AdaptedEvaluatorFactory(theName, fun.getN()),
		adaptedFun(fun)
	{
	}
	
	ExpressionEvaluator	*MakeEvaluator(
			const vector<CommonEvaluator*>& fieldArgs)
	{
		return new AdaptedEvaluator<ArgAdaptor,typename FFTypeInfo<typename ArgAdaptor::return_type>::baseType>
									(adaptedFun, fieldArgs);
	}
};

template <class ArgAdaptor>
AdaptedEvaluatorFactory * EvaluatorFactory(const string& theName,
												ArgAdaptor fun)
{
	return new AdaptedEvaluatorFactory_Ret<ArgAdaptor> (theName,fun);
}

extern vector<AdaptedEvaluatorFactory*> registeredFieldFunctions;

void RegisterFieldFunctionID(string s);
	// declaration duplicated from QuantumDescription.h

template <class F>
void RegisterFieldFunction(const char* theName, F fun)
{
	registeredFieldFunctions.push_back(EvaluatorFactory(string(theName),AdaptArg(fun)));
	RegisterFieldFunctionID(string(theName));
}

template <class F1, class F>
void RegisterFieldFunction(const char* theName, F1 fun1, F fun)
{
	registeredFieldFunctions.push_back(EvaluatorFactory(string(theName),AdaptArg(fun1,fun)));
	RegisterFieldFunctionID(string(theName));
}

#endif
