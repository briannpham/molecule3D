#pragma once
#include <cmath>

const float PI = 3.1415926535f;

struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    static float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalize() const { float l = length(); return (l > 0) ? Vec3(x / l, y / l, z / l) : Vec3(0,0,0); }
};

struct Mat3 {
    float m[9]; // Column-major: m[col*3 + row]
    Mat3(float diagonal = 1.0f) {
        for(int i=0; i<9; ++i) m[i] = 0.0f;
        m[0] = m[4] = m[8] = diagonal;
    }
    // (More methods like inverse, transpose can be added if needed for mat3 specifically)
};

struct Mat4 {
    float m[16];
    Mat4(float diagonal = 1.0f) { for (int i = 0; i < 16; ++i) m[i] = 0; m[0] = m[5] = m[10] = m[15] = diagonal; }
    static Mat4 identity() { return Mat4(1.0f); }
    static Mat4 translate(const Vec3& t) { Mat4 r = identity(); r.m[12]=t.x; r.m[13]=t.y; r.m[14]=t.z; return r; }
    static Mat4 scale(const Vec3& s) { Mat4 r = identity(); r.m[0]=s.x; r.m[5]=s.y; r.m[10]=s.z; return r; }
    static Mat4 rotateZ(float angle_rad) {
        Mat4 res = identity();
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        res.m[0] = c; res.m[1] = s;
        res.m[4] = -s; res.m[5] = c;
        return res;
    }
    Mat4 operator*(const Mat4& other) const {
        Mat4 result(0.0f);
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += m[k * 4 + r] * other.m[c * 4 + k];
                }
                result.m[c * 4 + r] = sum;
            }
        }
        return result;
    }
    static Mat4 perspective(float fov_y_rad, float aspect, float z_near, float z_far) {
        Mat4 res(0.0f);
        float tan_half_fovy = std::tan(fov_y_rad / 2.0f);
        res.m[0] = 1.0f / (aspect * tan_half_fovy);
        res.m[5] = 1.0f / (tan_half_fovy);
        res.m[10] = -(z_far + z_near) / (z_far - z_near);
        res.m[11] = -1.0f;
        res.m[14] = -(2.0f * z_far * z_near) / (z_far - z_near);
        return res;
    }
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = Vec3::cross(f, up).normalize();
        Vec3 u = Vec3::cross(s, f);

        Mat4 res = identity();
        res.m[0] = s.x; res.m[4] = s.y; res.m[8] = s.z;
        res.m[1] = u.x; res.m[5] = u.y; res.m[9] = u.z;
        res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
        res.m[12] = -Vec3::dot(s, eye);
        res.m[13] = -Vec3::dot(u, eye);
        res.m[14] = Vec3::dot(f, eye);
        return res;
    }
    Mat3 toMat3() const {
        Mat3 res(0.0f);
        res.m[0] = m[0]; res.m[1] = m[1]; res.m[2] = m[2];
        res.m[3] = m[4]; res.m[4] = m[5]; res.m[5] = m[6];
        res.m[6] = m[8]; res.m[7] = m[9]; res.m[8] = m[10];
        return res;
    }
    Mat4 affineInverse() const {
        Mat4 inv = Mat4::identity();
        float det = m[0]*(m[5]*m[10] - m[6]*m[9]) - m[1]*(m[4]*m[10] - m[6]*m[8]) + m[2]*(m[4]*m[9] - m[5]*m[8]);
        if (std::abs(det) < 1e-6) return Mat4::identity();
        float invDet = 1.0f / det;

        inv.m[0] = (m[5]*m[10] - m[6]*m[9]) * invDet;
        inv.m[1] = (m[2]*m[9] - m[1]*m[10]) * invDet;
        inv.m[2] = (m[1]*m[6] - m[2]*m[5]) * invDet;
        inv.m[4] = (m[6]*m[8] - m[4]*m[10]) * invDet;
        inv.m[5] = (m[0]*m[10] - m[2]*m[8]) * invDet;
        inv.m[6] = (m[2]*m[4] - m[0]*m[6]) * invDet;
        inv.m[8] = (m[4]*m[9] - m[5]*m[8]) * invDet;
        inv.m[9] = (m[1]*m[8] - m[0]*m[9]) * invDet;
        inv.m[10] = (m[0]*m[5] - m[1]*m[4]) * invDet;

        inv.m[12] = -(m[12]*inv.m[0] + m[13]*inv.m[4] + m[14]*inv.m[8]);
        inv.m[13] = -(m[12]*inv.m[1] + m[13]*inv.m[5] + m[14]*inv.m[9]);
        inv.m[14] = -(m[12]*inv.m[2] + m[13]*inv.m[6] + m[14]*inv.m[10]);
        return inv;
    }
    Mat4 transpose() const {
        Mat4 res(0.0f);
        for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) res.m[j*4+i] = m[i*4+j];
        return res;
    }
}; 