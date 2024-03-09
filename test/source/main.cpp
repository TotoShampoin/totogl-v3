#include "TotoGL/RenderObject/RenderObject.hpp"
#include <TotoGL/TotoGL.hpp>

#include <TotoGL/RenderObject/Light.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

void event(
    TotoGL::Window& window,
    TotoGL::Camera& camera,
    bool& holding,
    TotoGL::OrbitControl& orbit,
    glm::vec3& velocity);

TotoGL::ShaderMaterialInstanceId getMaterial();
TotoGL::RenderObjectInstanceId makeObject();
TotoGL::RenderObjectInstanceId makeHelper();

int main(int /* argc */, const char** /* argv */) {
    using TotoGL::ShaderType::FRAGMENT;
    using TotoGL::ShaderType::VERTEX;

    using TotoGL::InputEventName::KEY;
    using TotoGL::InputEventName::MOUSE_BUTTON;
    using TotoGL::VectorEventName::CURSOR_POSITION;
    using TotoGL::VectorEventName::FRAMEBUFFER_SIZE;
    using TotoGL::VectorEventName::SCROLL;

    constexpr int WIDTH = 640;
    constexpr int HEIGHT = 480;
    constexpr float FOV = glm::radians(70.f);

    auto window = TotoGL::Window(WIDTH, HEIGHT, "a title");
    auto renderer = TotoGL::Renderer();

    auto scene = TotoGL::Scene();

    auto kirbies = std::vector<std::reference_wrapper<TotoGL::RenderObject>>();
    for (int i = 0; i < 6; i++) {
        auto kirby = makeObject();
        scene.add(kirby);
        kirbies.push_back(TotoGL::RenderObjectFactory::get(kirby));
    }

    kirbies[0].get().translate({ 0, 0, 2 });
    kirbies[1].get().translate({ 0, 0, -2 });
    kirbies[2].get().translate({ 0, 2, 0 });
    kirbies[3].get().translate({ 0, -2, 0 });
    kirbies[4].get().translate({ 2, 0, 0 });
    kirbies[5].get().translate({ -2, 0, 0 });

    // auto lights = std::vector<TotoGL::Light>();
    {
        auto ambient_id = TotoGL::LightFactory::create(
            TotoGL::Light(glm::vec3(1, 1, 1), .25, TotoGL::LightType::AMBIENT));
        auto dirlight_id = TotoGL::LightFactory::create(
            TotoGL::Light(glm::vec3(1, 1, 1), 1, TotoGL::LightType::DIRECTIONAL));

        auto& dirlight = TotoGL::LightFactory::get(dirlight_id);
        dirlight.position() = { 3, 3, 3 };
        dirlight.transformation().lookAt({ 0, 0, 0 });

        scene.add(ambient_id);
        scene.add(dirlight_id);
    }

    auto& material = kirbies[0].get().material();

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, .1f, 100.f);
    auto clock = TotoGL::Clock();

    auto orbit = TotoGL::OrbitControl(0, 0, 8);

    bool holding = false;
    glm::vec3 velocity = { 0, 0, 0 };

    event(window, camera, holding, orbit, velocity);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.clearColor({ 0, 0, 0, 1 });
    while (!window.shouldClose()) {
        float time = clock.getTime();
        float delta = clock.getDeltaTime();

        for (auto& kirby : kirbies) {
            kirby.get().rotation() += glm::vec3(2, 3, 5) * delta;
        }

        orbit.apply(camera);

        material.uniform("u_time", time);

        window.draw([&]() {
            renderer.clear();
            renderer.render(scene, camera);
        });
    }

    return 0;
}