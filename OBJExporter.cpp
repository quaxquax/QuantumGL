#include "OBJExporter.h"
#include "IsoSurface.h"
#include <iostream>
#include <iomanip>
#include <fstream>

void OBJExporter::WriteVertex(std::ofstream& out, const vecR3& vertex) {
    out << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
}

void OBJExporter::WriteNormal(std::ofstream& out, const vecR3& normal) {
    out << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
}

void OBJExporter::WriteFace(std::ofstream& out, const GLint* indices, int count) {
    out << "f ";
    for (int i = 0; i < count; i++) {
        out << indices[i] + 1 << "//" << indices[i] + 1 << " ";
    }
    out << "\n";
}

bool OBJExporter::ExportToOBJ(const VisualObject& object, const std::string& filename) {
    if (!object.HasMeshData()) {
        std::cerr << "Object does not have mesh data to export\n";
        return false;
    }

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return false;
    }

    // Write header
    out << "# OBJ file exported from QuantumGL" << std::endl;
    const DynArray<vecR3>* vertices = object.GetVertices();
    const DynArray<vecR3>* normals = object.GetNormals();
    out << "# Vertices: " << vertices->n << std::endl;
    out << "# Normals: " << normals->n << std::endl;

    // Write vertices
    for (int i = 0; i < vertices->n; i++) {
        WriteVertex(out, vertices->x[i]);
    }

    // Write normals
    for (int i = 0; i < normals->n; i++) {
        WriteNormal(out, normals->x[i]);
    }

    // Write faces (assuming triangles)
    const DynArray<GLint>* indices = object.GetIndices();
    for (int i = 0; i < indices->n; i += 3) {
        GLint faceIndices[3] = { indices->x[i], indices->x[i+1], indices->x[i+2] };
        WriteFace(out, faceIndices, 3);
    }

    out.close();
    return true;
}

bool OBJExporter::ExportToOBJ(const IsoSurface& surface, const std::string& filename) {
    return ExportToOBJ(static_cast<const VisualObject&>(surface), filename);
}
