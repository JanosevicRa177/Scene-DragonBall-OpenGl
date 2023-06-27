#include "cubebuffer.hpp"

float CubeBuffer::mVertices[] = {
        -0.2,-0.2,-0.2,    0, 0, -1,    0, 0,   //0
         0.2,-0.2,-0.2,    0, 0, -1,    0, 1,   //1
        -0.2, 0.2,-0.2,    0, 0, -1,    1, 0,   //2
         0.2, 0.2,-0.2,    0, 0, -1,    1, 1,   //3
                                             
        -0.2,-0.2,-0.2,    0, -1, 0,    0, 0,   //4
         0.2,-0.2, 0.2,    0, -1, 0,    1, 1,   //5
        -0.2,-0.2, 0.2,    0, -1, 0,    0, 1,   //6
         0.2,-0.2,-0.2,    0, -1, 0,    1, 0,   //7
                                             
        0.2,-0.2, 0.2,     1, 0, 0,     0, 1,   //8
        0.2,-0.2,-0.2,     1, 0, 0,     0, 0,   //9
        0.2, 0.2,-0.2,     1, 0, 0,     1, 0,   //10
        0.2, 0.2, 0.2,     1, 0, 0,     1, 1,   //11
                                             
        -0.2,-0.2, 0.2,    -1, 0, 0,    0, 1,   //12
        -0.2,-0.2,-0.2,    -1, 0, 0,    0, 0,   //13
        -0.2, 0.2,-0.2,    -1, 0, 0,    1, 0,   //14
        -0.2, 0.2, 0.2,    -1, 0, 0,    1, 1,   //15
                                             
        -0.2,-0.2, 0.2,    0, 0, 1,     0, 0,   //16
         0.2,-0.2, 0.2,    0, 0, 1,     1, 0,   //17
        -0.2, 0.2, 0.2,    0, 0, 1,     0, 1,   //18
         0.2, 0.2, 0.2,    0, 0, 1,     1, 1,   //19
                                             
        -0.2, 0.2,-0.2,   0, 1, 0,      0, 0,   //20
         0.2, 0.2,-0.2,   0, 1, 0,      1, 0,   //21
         0.2, 0.2, 0.2,   0, 1, 0,      1, 1,   //22
        -0.2, 0.2, 0.2,   0, 1, 0,      0, 1,   //23

    };
unsigned CubeBuffer::mIndices[] = {
        0, 3, 1,
        0, 2, 3,

        7, 5, 6,
        4, 7, 6,

        11, 8, 9,
        10, 11, 9,

        15, 13, 12,
        15, 14, 13,

        18, 16, 17,
        19, 18, 17,

        22, 21, 23,
        21, 20, 23,
    };

CubeBuffer::CubeBuffer() {}

float* CubeBuffer::GetVertices() {
	return mVertices;
}
unsigned* CubeBuffer::GetIndices() {
    return mIndices;
}

unsigned CubeBuffer::GetVertexCount() {
	return sizeof(mVertices);
}
unsigned CubeBuffer::GetIndicesCount() {
    return sizeof(mIndices);
}