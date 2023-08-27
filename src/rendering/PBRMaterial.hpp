#pragma once

#include "Material.hpp"

namespace Villain {

    // NOTE: Not sure yet if we want to inherit from simple material or use something else
    // But for now at least we can reuse normal maps and parallax displacement maps if needed from Material
    class PBRMaterial : public Material {
        public:
            PBRMaterial(const std::string& name = std::string()) { this->name = name; }

            virtual void updateUniforms(Shader& shader) override;

            PBRMaterial(
                    const std::string& name,
                    Texture* albedoMp,
                    Texture* normalMp = nullptr,
                    Texture* metallicMp = nullptr,
                    Texture* roughnessMp = nullptr,
                    Texture* aoMp = nullptr
                    ) :
                albedoMap(albedoMp), metallicMap(metallicMp), roughnessMap(roughnessMp), aoMap(aoMp)
            {
                this->name = name;
                this->normalMap = normalMp;
            }

            PBRMaterial(
                    const std::string& name,
                    const glm::vec3& albedoValue = glm::vec3(1.0f),
                    float metallicValue = 0.0f,
                    float roughnessValue = 0.5f,
                    float aoValue = 0.0f
                    ) :
                albedo(albedoValue), metallic(metallicValue), roughness(roughnessValue), ao(aoValue)
            {
                this->name = name;
            }

            float* getAlbedoColorPtr() { return glm::value_ptr(albedo); }
            float* getMetallicPtr() { return &metallic; }
            float* getRoughnessPtr() { return &roughness; }
            float* getAOPtr() { return &ao; }
            Texture* getAlbedoMap() { return albedoMap; }
            Texture* getMetallicMap() { return metallicMap; }
            Texture* getRoughnessMap() { return roughnessMap; }
            Texture* getAOMap() { return aoMap; }

            void setAlbedoMap(Texture* alb) { albedoMap = alb; }
            void setMetallicMap(Texture* metal) { metallicMap = metal; }
            void setRoughnessMap(Texture* rough) { roughnessMap = rough; }
            void setAOMap(Texture* ambientOcl) { aoMap = ambientOcl; }

        private:
            glm::vec3 albedo{1.0f};
            float metallic = 0.0f;
            float roughness = 0.5f;
            float ao = 0.0f;

            /// Normal map is inherited from Material
            Texture* albedoMap = nullptr; ///< Surface colour map
            Texture* metallicMap = nullptr; ///< Specifies whether material is metallic or not
            Texture* roughnessMap = nullptr; ///< Rougher surface gets wider, blurrier reflections, opposite to smoothness map
            Texture* aoMap = nullptr; ///<  Ambient oclussion map
    };
}
