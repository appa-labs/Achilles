#include <Engine/include/Engine.h>
#include <Engine/include/tools.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

void Engine::Initialization() {
    width = sf::VideoMode::getDesktopMode().width;
    height = sf::VideoMode::getDesktopMode().height;
    aspect = static_cast<double>(width) / height;

    sf::ContextSettings settings(0, 0, 8, 1, 1, 0, false);
    window.create(sf::VideoMode(width, height), "Achilles", sf::Style::Fullscreen, settings);
    window.setMouseCursorVisible(false);

    LoadObjTypes("resources/objectTypes.txt");
    LoadObjects("resources/objectMap.txt");
}

void Engine::LoadObjTypes(const std::string& path) {
    std::ifstream fin;
    fin.open(path);
    if (!(fin.is_open())) {
        std::cerr << "�� ������� ������� ���� � ���������� �������� �� ���� " +
                         path;  // ����� ��������� ������ ����� ������, ��� ����
        Stop();
    }
    line buf({0, 0}, {0, 0});
    std::string inputbuffer;
    std::string strbuf;
    std::vector<line> vectorbuf;
    double x1, y1, x2, y2;
    while (std::getline(fin, inputbuffer)) {
        std::stringstream input(inputbuffer);
        input >> strbuf;
        while (input >> x1 >> y1 >> x2 >> y2) {
            buf = line({x1, y1}, {x2, y2});
            vectorbuf.push_back(buf);
        }
        objtypes_.insert({strbuf, vectorbuf});
        vectorbuf.clear();
    }
}

void Engine::LoadObjects(
    const std::string& path) {  // ���� ���, ���������� �����������: ������ � �����
    // ������� � ����������� ���������� ������
    std::ifstream fin;
    fin.open(path);
    if (!(fin.is_open())) {
        std::cerr << "�� ������� ������� ���� � ������ �������� �� ���� " +
                         path;  // ����� ��������� ������ ����� ������, ��� ����
        Stop();
    }
    vec buf(0, 0);
    std::string inputbuffer;
    std::string name;
    std::vector<line> vectorcache;
    std::string cachename;
    double x1, y1;
    while (std::getline(fin, inputbuffer)) {
        std::stringstream input(inputbuffer);
        input >> name >> x1 >> y1;
        buf = vec(x1, y1);
        if (cachename != name) {
            cachename = name;
            vectorcache = objtypes_[name];
        }
        input >> inputbuffer;
        if (inputbuffer == "m") {
            double mass;
            input >> mass;
            moveableObjects_.push_back(std::make_unique<MoveableObject>(buf, vectorcache, mass));
        }
        if (inputbuffer == "s") {
            objects_.push_back(std::make_unique<Object>(buf, vectorcache));
        }
    }
    objtypes_.clear();  // ����������� ������, ���� ����� ��
}

void Engine::Stop() {
    exit(-1);
}

void Engine::RenderFrame() {
    window.clear(sf::Color(0, 0, 0, 0));
    for (const auto& obj : objects_) {
        RenderObject(obj);
    }
    for (const auto& obj : moveableObjects_) {
        RenderObject(obj);
        // RenderCollider(obj);
        PrintCollider(obj);
    }
    window.display();
}

void Engine::RenderObject(const std::unique_ptr<Object>& object) {
    for (const auto& line : object->polygons_) {
        sf::Vertex vline[] = {
          sf::Vertex(sf::Vector2f(
              (object->basepoint_ + line.d1).cord(width, height).x,
              (object->basepoint_ + line.d1).cord(width, height).y)),
          sf::Vertex(sf::Vector2f(
              (object->basepoint_ + line.d2).cord(width, height).x,
              (object->basepoint_ + line.d2).cord(width, height).y))};
        window.draw(vline, 2, sf::Lines);
    }
}

void Engine::PhysicsPerFrame() {
    for (auto& _obj : moveableObjects_) {
        auto obj = static_cast<MoveableObject*>(_obj.get());
        vec& F = obj->resultantForce;
        double m = obj->mass;
        vec& vel = obj->velocity;

        F = vec(0, -1 * m * PH_CONST_G) + obj->magicForces;
        for (const auto& coln : objects_) {
            obj->SumNormalForces(coln.get());
        }
        for (const auto& coln : moveableObjects_) {
            obj->SumNormalForces(coln.get());
        }

        obj->magicForces = vec(0, 0);

        vec a = F / m;
        vel = vel + a * frametime;
        obj->move(vel * frametime);
    }
}

void Engine::CharacterJump() {
    auto player = static_cast<MoveableObject*>(moveableObjects_[0].get());
    player->magicForces = vec(0, 3000);
}

void Engine::CharacterLeft() {
    auto player = static_cast<MoveableObject*>(moveableObjects_[0].get());
    player->magicForces = vec(-300, 0);
}

void Engine::CharacterRight() {
    auto player = static_cast<MoveableObject*>(moveableObjects_[0].get());
    player->magicForces = vec(300, 0);
}

void Engine::Restart() {
    auto player = static_cast<MoveableObject*>(moveableObjects_[0].get());
    player->basepoint_ = vec(0, 0);
    player->magicForces = vec(0, 0);
    player->velocity = vec(0, 0);
    player->resultantForce = vec(0, 0);
}

// [debug]
void Engine::RenderCollider(const std::unique_ptr<Object>& object) {
    sf::Color white = {255, 255, 255};
    for (const auto& line : object->polygons_) {
        ColliderShape collider;
        collider.setFillColor(white);
        collider.setRadius(10);
        collider.setPoints(
            {sf::Vector2f(
                 (object->basepoint_ + line.d1).cord(width, height).x,
                 (object->basepoint_ + line.d1).cord(width, height).y),
             sf::Vector2f(
                 (object->basepoint_ + line.d2).cord(width, height).x,
                 (object->basepoint_ + line.d2).cord(width, height).y)});

        window.draw(collider);
    }
}

void Engine::PrintCollider(const std::unique_ptr<Object>& obj) {
    line l = obj->polygons_[0];
    l = l.move(obj->basepoint_ - vec(PH_CONST_COLLISION_PRES, -PH_CONST_COLLISION_PRES));
    auto white = sf::Color(255, 255, 255);

    sf::CircleShape circle1(PH_CONST_COLLISION_PRES / 2 * height);
    circle1.setFillColor(white);
    circle1.move(l.d1.cord(width, height).x, l.d1.cord(width, height).y);

    sf::CircleShape circle2(PH_CONST_COLLISION_PRES / 2 * height);
    circle2.setFillColor(white);
    circle2.move(l.d2.cord(width, height).x, l.d2.cord(width, height).y);

    sf::RectangleShape rect(sf::Vector2f(
        PH_CONST_COLLISION_PRES * height,
        0.3 / 2 * height));  // the rectangle is specified by the size
    rect.setFillColor(white);
    rect.move(
        l.d1.cord(width, height).x, (l.d1 - vec(0, PH_CONST_COLLISION_PRES))
                                        .cord(width, height)
                                        .y);  // shifted relative to the top left point

    window.draw(circle1);
    window.draw(circle2);
    window.draw(rect);
}