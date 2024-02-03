#include <TotoGL/TotoGL.hpp>

#include <glm/glm.hpp>

int main(int argc, const char* argv[]) {
    using TotoGL::ShaderType::FRAGMENT;
    using TotoGL::ShaderType::VERTEX;
    using TotoGL::VectorEventName::FRAMEBUFFER_SIZE;

    constexpr int WIDTH = 640;
    constexpr int HEIGHT = 480;

    auto window = TotoGL::Window(WIDTH, HEIGHT, "a title");
    auto renderer = TotoGL::Renderer();

    const auto tex_id = TotoGL::TextureFactory::create(TotoGL::Texture(std::ifstream("assets/textures/XYZ.png")));
    const auto mat_id = TotoGL::MaterialFactory::create(TotoGL::Material(std::ifstream("assets/shader/shader.vert"), std::ifstream("assets/shader/shader.frag")));
    const auto mesh_id = TotoGL::MeshFactory::create(TotoGL::Mesh::cube());
    const auto obj_id = TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(mesh_id, mat_id));

    auto& texture = TotoGL::TextureFactory::get(tex_id);
    auto& mesh = TotoGL::MeshFactory::get(mesh_id);
    auto& material = TotoGL::MaterialFactory::get(mat_id);
    auto& object = TotoGL::RenderObjectFactory::get(obj_id);

    auto camera = TotoGL::Camera::Perspective(glm::radians(70.f), (float)WIDTH / HEIGHT, 1.f, 100.f);
    auto clock = TotoGL::Clock();
    object.translate({ 0, 0, -2 });

    auto projection = camera.projection();
    material.uniform("u_projection", projection);

    window.on(FRAMEBUFFER_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, int(event.x), int(event.y));
        camera.setPersective(glm::radians(70.f), (float)event.x / event.y, 1.f, 100.f);

        auto projection = camera.projection();
        material.uniform("u_projection", projection);
    });

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!window.shouldClose()) {
        float time = clock.getTime();
        float delta = clock.getDeltaTime();

        object.rotate(delta, { 1, 1, 0 });

        auto [width, height] = window.size();

        auto modelview = camera.view() * object.transformMatrix();
        auto normal = glm::mat3(glm::transpose(glm::inverse(modelview)));

        material.uniform("u_time", time);
        material.uniform("u_texture", texture);

        material.uniform("u_modelview", modelview);
        material.uniform("u_normal", normal);

        window.draw([&]() {
            glClearColor(.25, 0., .25, 1.);
            glClear(GL_COLOR_BUFFER_BIT);

            object.draw();
        });
    }

    return 0;
}
