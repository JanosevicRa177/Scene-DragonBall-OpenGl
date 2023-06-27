#include "pyramidbuffer.hpp"

float PyramidBuffer::mVertices[] = {
        -0.2,-0.2,-0.2,    0, -1, 0,            0, 0,      //0
         0.2,-0.2, 0.2,    0, -1, 0,            1, 1,      //1
        -0.2,-0.2, 0.2,    0, -1, 0,            1, 0,      //2
         0.2,-0.2,-0.2,    0, -1, 0,            0, 1,      //3
                                                       
         0.2,-0.2, 0.2,    0, 0.4227, 0.5773,   1, 0,      //4
        -0.2,-0.2, 0.2,    0, 0.4227, 0.5773,   0, 0,      //5
         0.0, 0.2, 0.0,    0, 0.4227, 0.5773,   0.5, 1,    //6
                                                       
        -0.2,-0.2,-0.2,    0, 0.4227, -0.5773,  0, 0,      //7
         0.2,-0.2,-0.2,    0, 0.4227, -0.5773,  1, 0,      //8
         0.0, 0.2, 0.0,    0, 0.4227, -0.5773,  0.5, 1,    //9
                                                       
         0.2,-0.2, 0.2,    0.5773, 0.4227, 0,   1, 0,      //10
         0.2,-0.2,-0.2,    0.5773, 0.4227, 0,   0, 0,      //11
         0.0, 0.2, 0.0,    0.5773, 0.4227, 0,   0.5, 1,    //12
                                                       
        -0.2,-0.2,-0.2,    -0.5773, 0.4227, 0,  0, 0,      //13
        -0.2,-0.2, 0.2,    -0.5773, 0.4227, 0,  1, 0,      //14
         0.0, 0.2, 0.0,    -0.5773, 0.4227, 0,  0.5, 1,    //15


};
unsigned PyramidBuffer::mIndices[] = {
        3, 1, 0,
        0, 1, 2,
        
        5, 4, 6,

        8, 7, 9,

        10, 11, 12,

        13, 14, 15,
};

PyramidBuffer::PyramidBuffer() {}

float* PyramidBuffer::GetVertices() {
    return mVertices;
}
unsigned* PyramidBuffer::GetIndices() {
    return mIndices;
}

unsigned PyramidBuffer::GetVertexCount() {
    return sizeof(mVertices);
}
unsigned PyramidBuffer::GetIndicesCount() {
    return sizeof(mIndices);
}