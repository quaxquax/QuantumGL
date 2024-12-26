#include "FieldFunctionImpl.h"

#include "StdFieldFunctions.h"
#include "Hydrogen553.h"
#include "CoulombFunctions.h"

void RegisterFieldFunctions()
{
	// StdFieldFunctions
	RegisterFieldFunction("arg",&field::arg);
	RegisterFieldFunction("argcolor",&field::argcolor);
	RegisterFieldFunction("stdcolor",&field::stdcolor);
	RegisterFieldFunction("hue",&field::arg);

	RegisterFieldFunction("rgbcolor",&field::rgbcolor);
	RegisterFieldFunction("complex_to_RGB",&field::complex_to_RGB);
	RegisterFieldFunction("complex_to_RGBp",&field::complex_to_RGBp);
	RegisterFieldFunction("direction_to_RGB",&field::direction_to_RGB);
	RegisterFieldFunction("vector_to_RGB",&field::vector_to_RGB);
	RegisterFieldFunction("spinor_to_RGB",&field::spinor_to_RGB);

	RegisterFieldFunction("real",&field::realpartC1);
	RegisterFieldFunction("imag",&field::imagpartC1);

	
	RegisterFieldFunction("spinor_to_vector",&field::spinor_to_vector);

//	RegisterFieldFunction("test2part",&field::test2part_1,&field::test2part);

	// Hydrogen553
	RegisterFieldFunction("hydrogen553",&field::hydrogen553);

	// CoulombFunctions
	RegisterFieldFunction("CoulombFunction",&field::CoulombFunction);
	RegisterFieldFunction("CoulombParabolicEF",&field::CoulombParabolicEF);
	RegisterFieldFunction("CoulombFunctionT",&field::CoulombFunctionT);
	RegisterFieldFunction("CoulombSpinorUp",&field::CoulombSpinorUp);
	RegisterFieldFunction("CoulombSpinorDown",&field::CoulombSpinorDown);
	RegisterFieldFunction("CoulombSpinor",&field::CoulombSpinor);


	RegisterFieldFunction("multiplyR1_R",&field::multiplyR1_R);
	RegisterFieldFunction("multiplyC1_C",&field::multiplyC1_C);
	RegisterFieldFunction("multiplyC1_C1",&field::multiplyC1_C1);
	RegisterFieldFunction("addC1_C",&field::addC1_C);
	RegisterFieldFunction("addC1_C1",&field::addC1_C1);
	RegisterFieldFunction("TestFunction1",&field::TestFunction1);
	
	RegisterFieldFunction("cylinder_field",&field::cylinder_field);
}
