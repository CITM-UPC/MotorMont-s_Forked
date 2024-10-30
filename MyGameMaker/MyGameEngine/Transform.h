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
    double yaw = 0.0;
    double pitch = 0.0;
    double roll = 0.0;

    // Nuevas propiedades de transformación
    vec3 _position = vec3(0.0f);  // Posición inicial en (0, 0, 0)
    vec3 _rotation = vec3(0.0f);  // Rotación inicial en (0, 0, 0)
    vec3 _scale = vec3(1.0f);     // Escala inicial en (1, 1, 1)
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

    // Métodos para obtener la posición, rotación y escala
    vec3 getPosition() const { return _position; }
    vec3 getRotation() const { return _rotation; }
    vec3 getScale() const { return _scale; }

    // Métodos para establecer la posición, rotación y escala
    void setPosition(const vec3& position) { _position = position; }
    void setRotation(const vec3& rotation) { _rotation = rotation; }
    void setScale(const vec3& scale) { _scale = scale; }

    void translate(const vec3& v);
    void rotateYaw(double radians);
    void rotatePitch(double radians);
    void rotateRoll(double radians);
    void rotate(double rads, const vec3& v);
    void updateRotationMatrix();
    void lookAt(const vec3& target);
    void alignCamera(const vec3& worldUp = vec3(0.0f, 1.0f, 0.0f));
    //void updateMatrix();

    Transform operator*(const mat4& other) { return Transform(_mat * other); }
    Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

