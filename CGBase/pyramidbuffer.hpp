#pragma once

#include <vector>

class PyramidBuffer {
public:
    float* GetVertices();
    unsigned* GetIndices();
    unsigned GetVertexCount();
    unsigned GetIndicesCount();
    PyramidBuffer();
private:
    static float mVertices[];
    static unsigned mIndices[];
};