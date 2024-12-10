#pragma once

#include "types.h"

class Transform {
private:
    union {
        mat4 _mat = mat4(1.0);
        struct {
            vec3 _left; mat4::value_type _left_w;
            vec3 _up; mat4::value_type _up_w;
            vec3 _fwd; mat4::value_type _fwd_w;
            vec3 _pos; mat4::value_type _pos_w;
        };
    };

    // Variables para los ángulos de rotación
    float yaw = 0.0;
    float pitch = 0.0;
    float roll = 0.0;

    // Puntero al padre
    Transform* parent = nullptr;

public:
    const auto& mat() const { return _mat; }
    const auto& left() const { return _left; }
    const auto& up() const { return _up; }
    const auto& fwd() const { return _fwd; }
    const auto& pos() const { return _pos; }
    auto& pos() { return _pos; }

    const auto* data() const { return &_mat[0][0]; }

    Transform() = default;
    Transform(const mat4& mat) : _mat(mat) {}

    void translate(const vec3& v);
    void rotateYaw(double radians);
    void rotatePitch(double radians);
    void rotateRoll(double radians);
    void rotate(double rads, const glm::vec3& v);

    void updateRotationMatrix();

    void setRotation(float newYaw, float newPitch, float newRoll);

    void setScale(const glm::vec3& scale);

    void lookAt(const vec3& target);
    void alignCamera(const vec3& worldUp = vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 extractEulerAngles(const glm::mat4& mat) const;
    glm::vec3 extractScale(const glm::mat4& mat) const;

    Transform operator*(const mat4& other) { return Transform(_mat * other); }
    Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }

    // Métodos para manejar el padre
    void setParent(Transform* newParent) { parent = newParent; }
    Transform* getParent() const { return parent; }
    mat4 getWorldMatrix() const;
};