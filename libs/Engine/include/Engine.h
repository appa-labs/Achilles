#pragma once
#include <Engine/include/tools.h>
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Engine {
   public:
    static inline int height;
    static inline int width;
    static inline double aspect;

    Engine() = delete;

    static void Initialization();
    static void LoadObjTypes(std::string path);
    static void LoadObjects(std::string path);
    static void Stop();
    static void RenderFrame();
    static void RenderObject(std::unique_ptr<Object> object);

   private:
    static inline std::vector<std::unique_ptr<Object>> objects_;
    static inline std::map<std::string, std::vector<line>> objtypes_;
    // static sf::ContextSettings settings_(0, 0, 8, 1, 1, 0, false);
    // static sf::RenderWindow win_(sf::VideoMode(width, height), "game", sf::Style::Fullscreen,
    // settings);
};