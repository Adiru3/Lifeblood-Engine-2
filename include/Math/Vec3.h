#pragma once
#include <cmath>
#include <iostream>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector arithmetic
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

    // Dot Product (Скалярное произведение) - Critical for Wall Run & Lighting
    static float Dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Cross Product (Векторное произведение) - For computing normals
    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    float Length() const { return std::sqrt(x*x + y*y + z*z); }
    float LengthSquared() const { return x*x + y*y + z*z; } // Optimization for checks

    // Normalize
    Vec3 Normalized() const {
        float len = Length();
        if (len > 0) return *this / len;
        return *this;
    }

    void Normalize() {
        float len = Length();
        if (len > 0) *this = *this / len;
    }
};
