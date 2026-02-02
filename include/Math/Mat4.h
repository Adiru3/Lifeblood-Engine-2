#pragma once
#include "Vec3.h"
#include <cstring>
#include <cmath>

// Column-major 4x4 Matrix (OpenGL compatible)
struct Mat4 {
    float elements[4][4];

    Mat4() {
        memset(elements, 0, 4 * 4 * sizeof(float));
    }

    static Mat4 Identity() {
        Mat4 res;
        res.elements[0][0] = 1.0f;
        res.elements[1][1] = 1.0f;
        res.elements[2][2] = 1.0f;
        res.elements[3][3] = 1.0f;
        return res;
    }

    static Mat4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        Mat4 res; 
        float tanHalfFov = tan(fov / 2.0f);
        
        res.elements[0][0] = 1.0f / (aspectRatio * tanHalfFov);
        res.elements[1][1] = 1.0f / tanHalfFov;
        res.elements[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        res.elements[2][3] = -1.0f;
        res.elements[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        return res;
    }

    static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Mat4 res;
        Vec3 f = (center - eye).Normalized();
        Vec3 s = Vec3::Cross(f, up).Normalized();
        Vec3 u = Vec3::Cross(s, f);

        res.elements[0][0] = s.x;
        res.elements[1][0] = s.y;
        res.elements[2][0] = s.z;
        res.elements[0][1] = u.x;
        res.elements[1][1] = u.y;
        res.elements[2][1] = u.z;
        res.elements[0][2] = -f.x;
        res.elements[1][2] = -f.y;
        res.elements[2][2] = -f.z;
        res.elements[3][0] = -Vec3::Dot(s, eye);
        res.elements[3][1] = -Vec3::Dot(u, eye);
        res.elements[3][2] = Vec3::Dot(f, eye);
        res.elements[3][3] = 1.0f;

        return res;
    }
};
