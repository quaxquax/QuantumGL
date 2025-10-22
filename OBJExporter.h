#ifndef OBJ_EXPORTER_H
#define OBJ_EXPORTER_H

#include <fstream>
#include <string>
#include "IsoSurface.h"
#include "Slice.h"
#include "VisualObject.h"

class OBJExporter {
public:
    static bool exportToOBJ(const IsoSurface* surface, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        // Write header
        file << "# OBJ file exported from QuantumGL\n";
        
        // Write vertices
        const DynArray<vecR3>& vertices = surface->GetVertices();
        for (int i = 0; i < vertices.n; i++) {
            file << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
        }

        // Write normals
        const DynArray<vecR3>& normals = surface->GetNormals();
        for (int i = 0; i < normals.n; i++) {
            file << "vn " << normals[i].x << " " << normals[i].y << " " << normals[i].z << "\n";
        }

        // Write faces (assuming triangles)
        const DynArray<GLint>& indices = surface->GetIndices();
        for (int i = 0; i < indices.n; i += 3) {
            // OBJ indices are 1-based
            file << "f " << (indices[i]+1) << "//" << (indices[i]+1) 
                 << " " << (indices[i+1]+1) << "//" << (indices[i+1]+1)
                 << " " << (indices[i+2]+1) << "//" << (indices[i+2]+1) << "\n";
        }

        file.close();
        return true;
    }
};

#endif // OBJ_EXPORTER_H
