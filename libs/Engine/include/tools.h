#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
extern inline const double PH_CONST_G = 9.81;
extern inline const double PH_CONST_COLLISION_PRES = 0.05;

/// ������� �������� � ���. ������������

class vec {
   public:
    double x = 0, y = 0;

    // ��� � ������, � �����, �� ����. ������ ���������� ������ ���������� ���� ��� ��������

    vec() : x(0), y(0) {
    }

    vec(double _x, double _y) : x(_x), y(_y) {
    }

    vec operator+(const vec& vect) const {
        return vec(x + vect.x, y + vect.y);
    }

    vec operator-(const vec& vect) const {
        return vec(x - vect.x, y - vect.y);
    }

    double operator*(const vec& vect) const {
        return x * vect.x + y * vect.y;
    }

    vec operator*(double k) const {
        return vec(x * k, y * k);
    }

    vec operator/(double k) const {
        return vec(x / k, y / k);
    }

    bool operator==(const vec& other) const {
        return abs(x - other.x) <= 0.0001 && abs(y - other.y) <= 0.0001;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }

    vec cord(int width, int height) const;  // �������������� �� ���� ��������� � ����

    vec anticord(int width, int height) const;  // ��������
};

class line {
   public:
    vec d1, d2;  // ��� ��������� ���� �������� (�� � ���, � ��� � � ��� �������� - ����� ���������,
                 // ������ ��� ����� ����� ������ - �������.

    line(const vec& p1, const vec& p2) : d1(p1), d2(p2){};

    line move(const vec& vector) const;
    vec norm() const;  // ��������� ������� � �����������

    bool operator==(const line& other) const {
        return d1 == other.d1 && d2 == other.d2;
    }
};

// ����������� ���� ������� ��� ������������ ����������

double Distance(const vec& p1, const vec& p2);

double Distance(const vec& dot, const line& line);  // ���������� �� ����� �� �������

double SignedDistance(
    const vec& dot, const line& line);  // �������� ���������� �� ����� �� �������(������)

inline double Distance(const line& line, const vec& dot) {
    return Distance(dot, line);
}

double Distance(const line& l1, const line& l2);  // ���������� �� ������� �� ������� (�.�
                                                  // ����������� ����� �������,
// ������
// �������� ����������� ������� �������, � ����� - �������

bool IsIntersect(const line& l1, const line& l2);  // ������������ �� �������

vec Proection(const vec& what, const line& axis);

vec Proection(const vec& what, vec vaxis);

/// ����� ��������� - ������. ����� ������� ��� ��� �������� ����� (�� ��� ����� ����� �����������
/// ��� ��������������� ���������); �������� � ���� ����� ��������� (�����), ����� ������� ��������

class Object {
   public:  // ���� ���, �������, ����� ��������
    friend class Engine;
    vec basepoint_;
    std::vector<line> polygons_;

   public:
    explicit Object(const vec& basepoint, const std::vector<line>& polygons)
        : basepoint_(basepoint), polygons_(polygons) {
    }

    virtual ~Object() = default;
};

// ����: ������� �������� ��������, ���� �������� ������� � ��� �������� (������������ basepoint). �
// � ������ ������� ������ ��������� �������� � basepoint. Load ���� ����� ��� ��������...

class MoveableObject : public Object {
   public:
    vec resultantForce;
    vec velocity;
    vec magicForces;
    double mass = 0;
    double frictionCoef = 1;

   public:
    void move(const vec& vector);

    void SumNormalForces(Object* obj);

    explicit MoveableObject(const vec& basepoint, const std::vector<line>& polygons)
        : Object(basepoint, polygons) {
    }

    explicit MoveableObject(const vec& basepoint, const std::vector<line>& polygons, double m)
        : Object(basepoint, polygons), mass(m) {
    }
};

class ColliderShape : public sf::Shape {
   public:
    ColliderShape() : points_{{0, 0}, {0, 0}}, point_count_(50), radius_(PH_CONST_COLLISION_PRES) {
        update();
    }

    void setPoints(const std::pair<sf::Vector2f, sf::Vector2f>&);

    void setRadius(const int&);

    void setPointCount(const size_t&);

    [[nodiscard]] virtual size_t getPointCount() const override {
        return point_count_;
    }

    [[nodiscard]] virtual sf::Vector2f getPoint(size_t index) const override {
        static const float pi = 3.141592654f;
        static const float k_eps = 1e-9;

        const float segment_normal_angle =
            pi / 2 -
            std::atan2(points_.first.y - points_.first.y, points_.first.x - points_.first.x);
        const float angle = index * 2 * pi / getPointCount();

        const float x = std::cos(angle + segment_normal_angle) * radius_;
        const float y = std::sin(angle + segment_normal_angle) * radius_;

        if (angle - pi < k_eps) {
            return {points_.first.x + x, points_.first.y + y};
        }
        return {points_.second.y + x, points_.second.y + y};
    }

   private:
    std::pair<sf::Vector2f, sf::Vector2f> points_;
    size_t point_count_;
    int radius_;
};
