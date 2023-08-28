#include "SkyDome.hpp"

#include "Logger.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "camera/Camera.hpp"

namespace Villain {

    VertexP1UV mapSphericalUV(const glm::vec3& p) {
        VertexP1UV v;
        v.Position = p;

        glm::vec3 pn = glm::normalize(p);

        v.UV.x = asinf(pn.x) / (float)M_PI + 0.5f;
        v.UV.y = asinf(pn.y) / (float)M_PI + 0.5f;
        // Map equirectangular texture using latitude and longitude
        //v.UV.x = 0.5f + atan2(pn.z, pn.x) / (2.0f * (float)M_PI);
        //v.UV.y = 0.5f - asinf(pn.y) / (float)M_PI;

        return v;
    }

    SkyDome::SkyDome(int numPitchStripes, int numHeadingStripes, float radius, Texture* texture) {

        skyDomeShader = Shader::createFromResource("skydome");
        skyDomeTexture = texture;

        populateBuffers(numPitchStripes, numHeadingStripes, radius);
    }

    void SkyDome::render(Camera* camera) {
        skyDomeShader->bind();
        skydomeVao->bind();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), camera->getPosition() - glm::vec3(0.0, 0.2, 0.0));
        glm::mat4 mvp = camera->getProjMatrix() * camera->getViewMatrix() * model;
        skyDomeShader->setUniformMat4f("MVP", mvp);

        if (skyDomeTexture) {
            skyDomeTexture->bind();
            skyDomeShader->setUniform1i("skydomeTexture", 0);
            skyDomeShader->setUniform1i("useTexture", true);
        } else {
            skyDomeShader->setUniform1i("useTexture", false);
        }

        GLint oldDepthFuncMode;
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

        glDepthFunc(GL_LEQUAL);
        GLCall(glDrawArrays(GL_TRIANGLES, 0, numVertices));
        glDepthFunc(oldDepthFuncMode);
    }

    void SkyDome::populateBuffers(int numRows, int numCols, int radius) {
        // Calculate total number of vertices required to construct hemisphere for the skydome
        int numVerticesTopStrip = 3 * numCols;
        int numVerticesRegularStrip = 6 * numCols;
        numVertices = numVerticesTopStrip + (numRows - 1) * numVerticesRegularStrip;

        std::vector<VertexP1UV> vertices(numVertices);

        float pitchAngle = 90.0f / numRows;
        float headingAngle = 360.0f / numCols;

        glm::vec3 apex(0.0, radius, 0.0f);
        float pitch = -90.0f;
        int i = 0;

        // Generate the top most triangle strip for the hemisphere
        for (float heading = 0.0f; heading < 360.0f; heading += headingAngle) {
            VertexP1UV v0 = mapSphericalUV(apex);
            vertices[i++] = v0;

            glm::vec3 pos1 = sphericalToCartesian(radius, pitch + pitchAngle, heading + headingAngle);
            VertexP1UV v1 = mapSphericalUV(pos1);
            vertices[i++] = v1;

            glm::vec3 pos2 = sphericalToCartesian(radius, pitch + pitchAngle, heading);
            VertexP1UV v2 = mapSphericalUV(pos2);
            vertices[i++] = v2;
        }

        // Generate the rest of the hemisphere
        for (pitch = -90.0f + pitchAngle; pitch < 0; pitch += pitchAngle) {
            for (float heading = 0.0f; heading < 360.0f; heading += headingAngle) {
                glm::vec3 pos0 = sphericalToCartesian(radius, pitch, heading);
                VertexP1UV v0 = mapSphericalUV(pos0);

                glm::vec3 pos1 = sphericalToCartesian(radius, pitch, heading + headingAngle);
                VertexP1UV v1 = mapSphericalUV(pos1);

                glm::vec3 pos2 = sphericalToCartesian(radius, pitch + pitchAngle, heading);
                VertexP1UV v2 = mapSphericalUV(pos2);

                glm::vec3 pos3 = sphericalToCartesian(radius, pitch + pitchAngle, heading + headingAngle);
                VertexP1UV v3 = mapSphericalUV(pos3);

                VILLAIN_ASSERT(i + 6 <= numVertices, "Failed generating hemisphere for the SkyDome");

                vertices[i++] = v0;
                vertices[i++] = v1;
                vertices[i++] = v2;

                vertices[i++] = v1;
                vertices[i++] = v3;
                vertices[i++] = v2;
            }
        }

        skydomeVao = std::make_unique<VertexArray>();
        skydomeVbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(VertexP1UV));
        skydomeVao->addBuffer(*skydomeVbo, VertexP1UV::getVertexLayout());
    }

    glm::vec3 SkyDome::sphericalToCartesian(float radius, float inclinationDegrees, float azimuthDegrees) {
        float inclination = glm::radians(inclinationDegrees); // or pitch
        float azimuth = glm::radians(azimuthDegrees); // or heading

        float x = radius * sin(inclination) * cos(azimuth);
        float y = radius * cos(inclination);
        float z = radius * sin(inclination) * sin(azimuth);
        return glm::vec3(x, y, z);
    }
}
