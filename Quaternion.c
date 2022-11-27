#include "Quaternion.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifndef M_PI
    #define M_PI (3.14159265358979323846)
#endif
#ifndef M_PIF
    #define M_PIF (3.14159265f)
#endif

void Quaternion_set(float w, float v1, float v2, float v3, Quaternion* output)
{
    assert(output != NULL);
    output->w = w;
    output->v[0] = v1;
    output->v[1] = v2;
    output->v[2] = v3;
}

void Quaternion_setIdentity(Quaternion* q)
{
    assert(q != NULL);
    Quaternion_set(1, 0, 0, 0, q);
}

void Quaternion_copy(Quaternion* q, Quaternion* output)
{
    Quaternion_set(q->w, q->v[0], q->v[1], q->v[2], output);
}

bool Quaternion_equal(Quaternion* q1, Quaternion* q2)
{
    bool equalW  = fabs(q1->w - q2->w) <= QUATERNION_EPS;
    bool equalV0 = fabs(q1->v[0] - q2->v[0]) <= QUATERNION_EPS;
    bool equalV1 = fabs(q1->v[1] - q2->v[1]) <= QUATERNION_EPS;
    bool equalV2 = fabs(q1->v[2] - q2->v[2]) <= QUATERNION_EPS;
    return equalW && equalV0 && equalV1 && equalV2;
}

void Quaternion_fprint(FILE* file, Quaternion* q)
{
    fprintf(file, "(%.3f, %.3f, %.3f, %.3f)",
        q->w, q->v[0], q->v[1], q->v[2]);
}


void Quaternion_fromAxisAngle(float axis[3], float angle, Quaternion* output)
{
    assert(output != NULL);
    output->w = cos(angle / 2.0);
    float c = sin(angle / 2.0);
    output->v[0] = c * axis[0];
    output->v[1] = c * axis[1];
    output->v[2] = c * axis[2];
}

float Quaternion_toAxisAngle(Quaternion* q, float output[3])
{
    assert(output != NULL);
    float angle = 2.0 * acos(q->w);
    float divider = sqrtf(1.0 - q->w * q->w);

    if(divider != 0.0) {
        // Calculate the axis
        output[0] = q->v[0] / divider;
        output[1] = q->v[1] / divider;
        output[2] = q->v[2] / divider;
    } else {
        // Arbitrary normalized axis
        output[0] = 1;
        output[1] = 0;
        output[2] = 0;
    }
    return angle;
}

void Quaternion_fromXRotation(float angle, Quaternion* output)
{
    assert(output != NULL);
    float axis[3] = {1.0, 0, 0};
    Quaternion_fromAxisAngle(axis, angle, output);
}

void Quaternion_fromYRotation(float angle, Quaternion* output)
{
    assert(output != NULL);
    float axis[3] = {0, 1.0, 0};
    Quaternion_fromAxisAngle(axis, angle, output);
}

void Quaternion_fromZRotation(float angle, Quaternion* output)
{
    assert(output != NULL);
    float axis[3] = {0, 0, 1.0};
    Quaternion_fromAxisAngle(axis, angle, output);
}

void Quaternion_fromEulerZYX(float eulerZYX[3], Quaternion* output)
{
    assert(output != NULL);

    float cy = cos(eulerZYX[2] * 0.5);
    float sy = sin(eulerZYX[2] * 0.5);
    float cr = cos(eulerZYX[0] * 0.5);
    float sr = sin(eulerZYX[0] * 0.5);
    float cp = cos(eulerZYX[1] * 0.5);
    float sp = sin(eulerZYX[1] * 0.5);

    output->w = cy * cr * cp + sy * sr * sp;
    output->v[0] = cy * sr * cp - sy * cr * sp;
    output->v[1] = cy * cr * sp + sy * sr * cp;
    output->v[2] = sy * cr * cp - cy * sr * sp;
}

void Quaternion_toEulerZYX(Quaternion* q, float output[3])
{
    assert(output != NULL);

    // Roll (x-axis rotation)
    float sinr_cosp = +2.0 * (q->w * q->v[0] + q->v[1] * q->v[2]);
    float cosr_cosp = +1.0 - 2.0 * (q->v[0] * q->v[0] + q->v[1] * q->v[1]);
    output[0] = atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    float sinp = +2.0 * (q->w * q->v[1] - q->v[2] * q->v[0]);
    if (fabs(sinp) >= 1)
        output[1] = copysign(M_PIF / 2, sinp); // use 90 degrees if out of range
    else
        output[1] = asin(sinp);

    // Yaw (z-axis rotation)
    float siny_cosp = +2.0 * (q->w * q->v[2] + q->v[0] * q->v[1]);
    float cosy_cosp = +1.0 - 2.0 * (q->v[1] * q->v[1] + q->v[2] * q->v[2]);
    output[2] = atan2(siny_cosp, cosy_cosp);
}

void Quaternion_conjugate(Quaternion* q, Quaternion* output)
{
    assert(output != NULL);
    output->w = q->w;
    output->v[0] = -q->v[0];
    output->v[1] = -q->v[1];
    output->v[2] = -q->v[2];
}

float Quaternion_norm(Quaternion* q)
{
    assert(q != NULL);
    return sqrt(q->w*q->w + q->v[0]*q->v[0] + q->v[1]*q->v[1] + q->v[2]*q->v[2]);
}

void Quaternion_normalize(Quaternion* q, Quaternion* output)
{
    assert(output != NULL);
    float len = Quaternion_norm(q);
    Quaternion_set(
        q->w / len,
        q->v[0] / len,
        q->v[1] / len,
        q->v[2] / len,
        output);
}

void Quaternion_multiply(Quaternion* q1, Quaternion* q2, Quaternion* output)
{
    assert(output != NULL);
    Quaternion result;

    /*
             a*e - b*f - c*g - d*h
        + i (b*e + a*f + c*h- d*g)
        + j (a*g - b*h + c*e + d*f)
        + k (a*h + b*g - c*f + d*e)
    */
    result.w =    q1->w   *q2->w    - q1->v[0]*q2->v[0] - q1->v[1]*q2->v[1] - q1->v[2]*q2->v[2];
    result.v[0] = q1->v[0]*q2->w    + q1->w   *q2->v[0] + q1->v[1]*q2->v[2] - q1->v[2]*q2->v[1];
    result.v[1] = q1->w   *q2->v[1] - q1->v[0]*q2->v[2] + q1->v[1]*q2->w    + q1->v[2]*q2->v[0];
    result.v[2] = q1->w   *q2->v[2] + q1->v[0]*q2->v[1] - q1->v[1]*q2->v[0] + q1->v[2]*q2->w   ;

    *output = result;
}

void Quaternion_rotate(Quaternion* q, float v[3], float output[3])
{
    assert(output != NULL);
    float result[3];

    float ww = q->w * q->w;
    float xx = q->v[0] * q->v[0];
    float yy = q->v[1] * q->v[1];
    float zz = q->v[2] * q->v[2];
    float wx = q->w * q->v[0];
    float wy = q->w * q->v[1];
    float wz = q->w * q->v[2];
    float xy = q->v[0] * q->v[1];
    float xz = q->v[0] * q->v[2];
    float yz = q->v[1] * q->v[2];

    // p2.x = w*w*p1.x + 2*y*w*p1.z - 2*z*w*p1.y + x*x*p1.x + 2*y*x*p1.y + 2*z*x*p1.z - z*z*p1.x - y*y*p1.x;
    // p2.y = 2*x*y*p1.x + y*y*p1.y + 2*z*y*p1.z + 2*w*z*p1.x - z*z*p1.y + w*w*p1.y - 2*x*w*p1.z - x*x*p1.y;
    // p2.z = 2*x*z*p1.x + 2*y*z*p1.y + z*z*p1.z - 2*w*y*p1.x - y*y*p1.z + 2*w*x*p1.y - x*x*p1.z + w*w*p1.z;

    result[0] = ww*v[0] + 2*wy*v[2] - 2*wz*v[1] +
                xx*v[0] + 2*xy*v[1] + 2*xz*v[2] -
                zz*v[0] - yy*v[0];
    result[1] = 2*xy*v[0] + yy*v[1] + 2*yz*v[2] +
                2*wz*v[0] - zz*v[1] + ww*v[1] -
                2*wx*v[2] - xx*v[1];
    result[2] = 2*xz*v[0] + 2*yz*v[1] + zz*v[2] -
                2*wy*v[0] - yy*v[2] + 2*wx*v[1] -
                xx*v[2] + ww*v[2];

    // Copy result to output
    output[0] = result[0];
    output[1] = result[1];
    output[2] = result[2];
}

void Quaternion_slerp(Quaternion* q1, Quaternion* q2, float t, Quaternion* output)
{
    Quaternion result;

    float cosHalfTheta = q1->w*q2->w + q1->v[0]*q2->v[0] + q1->v[1]*q2->v[1] + q1->v[2]*q2->v[2];

    // if q1=q2 or qa=-q2 then theta = 0 and we can return qa
    if (fabs(cosHalfTheta) >= 1.0) {
        Quaternion_copy(q1, output);
        return;
    }

    float halfTheta = acos(cosHalfTheta);
    float sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
    // If theta = 180 degrees then result is not fully defined
    // We could rotate around any axis normal to q1 or q2
    if (fabs(sinHalfTheta) < QUATERNION_EPS) {
        result.w = (q1->w * 0.5 + q2->w * 0.5);
        result.v[0] = (q1->v[0] * 0.5 + q2->v[0] * 0.5);
        result.v[1] = (q1->v[1] * 0.5 + q2->v[1] * 0.5);
        result.v[2] = (q1->v[2] * 0.5 + q2->v[2] * 0.5);
    } else {
        // Default quaternion calculation
        float ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
        float ratioB = sin(t * halfTheta) / sinHalfTheta;
        result.w = (q1->w * ratioA + q2->w * ratioB);
        result.v[0] = (q1->v[0] * ratioA + q2->v[0] * ratioB);
        result.v[1] = (q1->v[1] * ratioA + q2->v[1] * ratioB);
        result.v[2] = (q1->v[2] * ratioA + q2->v[2] * ratioB);
    }
    *output = result;
}