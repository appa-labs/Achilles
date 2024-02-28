#pragma once

#include <cmath>

/// ������� �������� � ���. ������������

class vec {
   public:
    double x = 0, y = 0;
    // ��� � ������, � �����, �� ����. ������ ���������� ������ ���������� ���� ��� ��������

    vec(double _x, double _y) : x(_x), y(_y){};

    vec operator+(vec vect) {
        return vec(x + vect.x, y + vect.y);
    }

    vec operator-(vec vect) {
        return vec(x - vect.x, y - vect.y);
    }

    double operator*(vec vect) {
        return x * vect.x + y * vect.y;
    }

    vec operator*(double k) {
        return vec(x * k, y * k);
    }

    vec operator/(double k) {
        return vec(x / k, y / k);
    }

    double length() {
        return std::sqrt(x * x + y * y);
    }

    // vec cord() {  // �������������� �� ���� ��������� � ����
    //     return {
    //       (x + 1. * Engine::aspect) / Engine::aspect / 2. * Engine::width,
    //       (-1 * y + 1.) / 2. * Engine::height};
    // }

    // vec anticord() {  // ��������
    //     return {
    //       x * Engine::aspect * 2. / Engine::width - 1. * Engine::aspect,
    //       (-1 * y) * 2. / Engine::height + 1.};
    // }
};

class line {
   public:
    vec d1, d2;  // ��� ��������� ���� �������� (�� � ���, � ��� � � ��� �������� - ����� ���������,
                 // ������ ��� ����� ����� ������ - �������.

    line(vec p1, vec p2) : d1(p1), d2(p2){};

    vec norm() {  // ��������� ������� � �����������
        double a = d1.y - d2.y, b = d2.x - d1.x;
        vec no(a, b);
        return no / no.length();
    }
};

// ����������� ���� ������� ��� ������������ ����������

double Distance(vec p1, vec p2);

double Distance(vec dot, line line);  // ���������� �� ����� �� �������

double SignedDistance(vec dot, line line);  // �������� ���������� �� ����� �� �������(������)

inline double Distance(line line, vec dot) {
    return Distance(dot, line);
}

double Distance(
    line l1, line l2);  // ���������� �� ������� �� ������� (�.� ����������� ����� �������, ������
                        // �������� ����������� ������� �������, � ����� - �������

bool IsIntersect(line l1, line l2);  // ������������ �� �������