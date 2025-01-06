#ifndef _H_OBJEXPORTER
#define _H_OBJEXPORTER

#include <string>
#include <fstream>
#include "IsoSurface.h"
#include "VisualObject.h"

class OBJExporter {
public:
    static bool ExportToOBJ(const IsoSurface& surface, const std::string& filename);
    static bool ExportToOBJ(const VisualObject& object, const std::string& filename);

private:
    static void WriteVertex(std::ofstream& out, const vecR3& vertex);
    static void WriteNormal(std::ofstream& out, const vecR3& normal);
    static void WriteFace(std::ofstream& out, const GLint* indices, int count);
};

#endif // _H_OBJEXPORTER
