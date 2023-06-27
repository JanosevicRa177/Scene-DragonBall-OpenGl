/**
 * @file camera.hpp
 * @author Jovan Ivosevic
 * @brief FPS Camera class
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    void Move(float dx, float dy, float dt);
    void Rotate(float dx, float dy, float dt);
    glm::vec3 GetPosition();
    glm::vec3 GetTarget();
    glm::vec3 GetUp();
    void SetYaw(float horizontalAngle);
    void SetPitch(float verticalAngle);
    void updateVectors();
    void Grow();
    void Shrink();

private:
    glm::vec3 mWorldUp;
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mVelocity;

    float mMoveSpeed;
    float mLookSpeed;
    float mPitch;
    float mYaw;
    float mPlayerHeight; // Should be moved out
};