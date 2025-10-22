var exampleText=    "fovy := 25;\n" +
   "latitude := 20;\n" +
   "longitude := 17;\n" +
   "ambient := 1;\n" +
   "colorbox;\n" +
   "\n" +
   "variable nradial := 1;\n" +
   "variable lorbital := 3;\n" +
   "variable mag := 2;\n" +
   "\n" +
   "field theData resolution 64 := CoulombFunction(nradial+lorbital+1, lorbital, mag, 1) cut_to [-50,50] scaled_to [-1,1];\n" +
   "field absData := abs(theData);\n" +
   "\n" +
   "variable iso1 [0.0001,0.01] := 0.0013;\n" +
   "variable iso2 := 0.0026;\n" +
   "variable shine := 30;\n" +
   "variable isotransp [0,1] := 0.4;\n" +
   "\n" +
   "isosurface absData at iso1 color argcolor(theData) cutout <[0,1],[0,1],[-1,1]> transparency isotransp shininess shine;\n" +
   "isosurface absData at iso2 color argcolor(theData)  shininess shine;\n" +
   "\n" +
   "variable ypos [-1,1] := 0.0;\n" +
   "variable zpos [-1,1] := -0.25;\n" +
   "variable colormapR [0, 0.1] := 0.0026;\n" +
   "variable slicetransp [0,1] := 0.2;\n" +
   "\n" +
   "slice y = ypos color complex_to_RGB(theData, colormapR) framed transparency slicetransp;\n" +
   "slice z = zpos color complex_to_RGB(theData, colormapR) framed transparency slicetransp;\n";
