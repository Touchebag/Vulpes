#include "shader_handle.h"

#include "uniforms/constant_int.h"
#include "uniforms/constant_float.h"
#include "uniforms/constant_vec2.h"
#include "uniforms/constant_vec4.h"
#include "uniforms/window_size.h"
#include "uniforms/relative_timer.h"

#include "utils/file.h"

#include "utils/log.h"

ShaderHandle::ShaderHandle(std::shared_ptr<sf::Shader> shader) :
    shader_(shader) {
}

ShaderHandle ShaderHandle::createFromJson(nlohmann::json j) {
    ShaderHandle handle{File().loadShader(j["shader"].get<std::string>())};

    if (j.contains("uniforms")) {
        for (auto it : j["uniforms"]) {
            if (!it.contains("type")) {
                LOGW("Shader uniform: missing type");
                continue;
            }

            std::shared_ptr<IShaderUniform> uniform;
            std::string type = it["type"].get<std::string>();

            if (type == "constant_int") {
                uniform = ConstantInt::createFromJson(it);
            } else if (type == "constant_float") {
                uniform = ConstantFloat::createFromJson(it);
            } else if (type == "constant_vec2") {
                uniform = ConstantVec2::createFromJson(it);
            } else if (type == "constant_vec4") {
                uniform = ConstantVec4::createFromJson(it);
            } else if (type == "window_size") {
                uniform = WindowSize::createFromJson(it);
            } else if (type == "relative_timer") {
                uniform = RelativeTimer::createFromJson(it);
            } else {
                LOGW("Shader uniform: unknown type %s", it["type"].get<std::string>().c_str());
                continue;
            }

            if (uniform) {
                handle.uniforms_.push_back(uniform);
            } else {
                LOGW("Failed to load shader %s", it["type"].get<std::string>().c_str());
            }
        }
    }

    return handle;
}

ShaderHandle& ShaderHandle::update() {
    for (auto it : uniforms_) {
        it->applyUniform(shader_);
    }

    return *this;
}

sf::Shader* ShaderHandle::get() {
    return shader_.get();
}
