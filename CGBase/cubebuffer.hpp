#pragma once

#include <vector>

class CubeBuffer {
public:
    float* GetVertices();
    unsigned* GetIndices();
    unsigned GetVertexCount();
    unsigned GetIndicesCount();
    CubeBuffer();
private:
    static float mVertices[];
    static unsigned mIndices[];
};