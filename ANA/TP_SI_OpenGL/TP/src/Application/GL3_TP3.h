#pragma once

#include <Application/BaseWithKeyboard.h>
#include <glm/common.hpp>
#include <gl3/Camera.h>
#include <Config.h>
#include <gl3/MeshLoader.h>
#include <gl3/SkyboxGeometry.h>
#include <gl3/ShaderProgram.h>
#include <gl3/CubeMap.h>
#include <gl3/VertexArrayObject.h>
#include <gl3/FrameBuffer.h>
#include <gl3/ScreenQuad2D.h>
#include <gl3/GlobalState.h>

namespace Application
{
	class GL3_TP3 : public BaseWithKeyboard
	{
		// Data for the camera
		gl3::Camera m_camera;
		float m_cameraSpeed;
		float m_cameraRotationSpeed;
		float m_lastDt;

		// The mesh loader
		gl3::MeshLoader m_meshLoader;

		// The loaded meshes
		std::vector<gl3::Mesh *> m_meshes;
		std::vector<gl3::PhongMaterial*> m_materials;
		std::vector<gl3::VertexArrayObject> m_phongDirectionalVaos;

		// Shaders
		gl3::ShaderProgram m_phongDirectionalShader; 

		// Textures
		gl3::CubeMap m_skyboxTexture;

		// Clipping distance
		float m_clippingDistance;

	public:
		GL3_TP3()
		{}

		virtual ~GL3_TP3()
		{}

	protected:
		virtual void reshape(GLint width, GLint height)
		{
			BaseWithKeyboard::reshape(width, height);
		}

		void handleKeys()
		{
			glm::vec3 xAxis(1.0, 0.0, 0.0);
			glm::vec3 yAxis(0.0, 1.0, 0.0);
			if (m_keyboard.isPressed('!')) { quit(); }
			if (m_keyboard.isPressed('r')) { m_camera.translateFront(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('f')) { m_camera.translateFront(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('d')) { m_camera.translateRight(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('q')) { m_camera.translateRight(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('z')) { m_camera.translateUp(m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('s')) { m_camera.translateUp(-m_cameraSpeed * m_lastDt); }
			if (m_keyboard.isPressed('g')) { m_camera.rotateLocal(yAxis, m_cameraRotationSpeed*m_lastDt); }
			if (m_keyboard.isPressed('j')) { m_camera.rotateLocal(yAxis, -m_cameraRotationSpeed * m_lastDt); }
			if (m_keyboard.isPressed('y')) { m_camera.rotateLocal(xAxis, m_cameraRotationSpeed*m_lastDt); }
			if (m_keyboard.isPressed('h')) { m_camera.rotateLocal(xAxis, -m_cameraRotationSpeed * m_lastDt); }
		}

		void forceTextureCoordinates()
		{
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				gl3::Mesh * mesh = m_meshes[cpt];
				if (!mesh->getTextureCoordinates())
				{
					std::cout << "Forcing texture coordinates on mesh " << cpt << "/" << m_meshes.size() << std::endl;
					std::vector<glm::vec2> textureCoordinates(mesh->getVertices()->size());
					std::fill(textureCoordinates.begin(), textureCoordinates.end(), glm::vec2(0, 0));
					mesh->setTextureCoodinates(textureCoordinates);
				}
			}
		}

		void computePhongDirectionalVAOs()
		{
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				gl3::Mesh * mesh = m_meshes[cpt];
				std::vector<std::pair<std::string, const gl3::VertexBufferObject *>> configuration =
				{
					{"in_position", mesh->getVerticesVbo()},
					{"in_normal", mesh->getNormalsVbo()},
					{"in_textureCoordinates", mesh->getTextureCoordinatesVbo()}
				};
				gl3::VertexArrayObject vao(m_phongDirectionalShader, configuration, mesh->getIndicesEbo());
				m_phongDirectionalVaos.push_back(std::move(vao));
			}
		}

		/// <summary>
		/// Computes the environment bounding box.
		/// </summary>
		/// <returns></returns>
		gl3::BoundingBox computeEnvironmentBoundingBox()
		{
			gl3::BoundingBox result;
			for (size_t cpt = 0; cpt < m_meshes.size(); ++cpt)
			{
				result = result + m_meshes[cpt]->boundingBox();
			}
			return result;
		}

		virtual void initializeRendering()
		{
#ifndef NDEBUG
			// Debug mode will be used in Debug configuration, not in release one.
			// The second parameter decides if the debug mode should automatically trigger
			// a breakpoint when an error / warning is reported.
			gl3::GlobalState::getSingleton()->enableDebugMode(true, true);
#endif

			// 0 - Camera setup
			m_camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
			m_cameraSpeed = 1.0f;
			m_cameraRotationSpeed = Math::pi / 5.0;
			m_lastDt = 0.1f;

			// 1 - We load the shaders
			{
				std::filesystem::path shaderPath = Config::dataPath() / "Shaders" / "Correction" / "TP3";
				m_phongDirectionalShader = gl3::ShaderProgram(shaderPath / "PhongDirectional.vert", shaderPath / "PhongDirectional.frag");
			}

			// 2 - We load the meshes
			{
				std::filesystem::path meshPath = Config::dataPath() / "lake" / "lake.dae";
				std::pair<gl3::MeshLoader::const_iterator, gl3::MeshLoader::const_iterator> range = m_meshLoader.load(meshPath);
				auto meshSelector = [this](const std::pair<gl3::Mesh*, gl3::PhongMaterial*> & p) { return p.first; };
				auto materialSelector = [this](const std::pair<gl3::Mesh*, gl3::PhongMaterial*> & p) { return p.second; };
				std::transform(range.first, range.second, std::back_inserter(m_meshes), meshSelector);
				std::transform(range.first, range.second, std::back_inserter(m_materials), materialSelector);
				// We force the meshes to have textures coordinates
				forceTextureCoordinates();
				// We initialize the VAOs used for the Phong shader
				computePhongDirectionalVAOs();
			}

			// 3 - We load the skybox texture
			{
				std::filesystem::path skyboxPath = Config::dataPath() / "textures" / "cubemaps" / "sky";
				std::unordered_map<gl3::CubeMapPosition, std::filesystem::path> files =
				{
					{ gl3::CubeMapPosition::front,  skyboxPath / "cloudtop_ft.jpg"},
					{ gl3::CubeMapPosition::back,  skyboxPath / "cloudtop_bk.jpg"},
					{ gl3::CubeMapPosition::left,  skyboxPath / "cloudtop_lf.jpg"},
					{ gl3::CubeMapPosition::right,  skyboxPath / "cloudtop_rt.jpg"},
					{ gl3::CubeMapPosition::bottom,  skyboxPath / "cloudtop_dn.jpg"},
					{ gl3::CubeMapPosition::top,  skyboxPath / "cloudtop_up.jpg"}
				};
				m_skyboxTexture = gl3::CubeMap(files);
			}

			// 3 - We modify the parameters of the camera based on the bounding box
			gl3::BoundingBox envBB = computeEnvironmentBoundingBox();
			glm::vec3 cameraPosition = glm::vec3(envBB.min().x + envBB.extent().x*0.5, envBB.min().y + envBB.extent().y*0.5, envBB.max().z);
			m_camera.setPosition(cameraPosition);
			m_cameraSpeed = glm::length(glm::vec2(envBB.extent())) / 40.0;

			// 4 - We create a sub menu for the clipping distance
			Application::Menu * menu = new Application::Menu("Clipping distance");
			float extent = glm::length(envBB.extent());
			menu->addItem("10% extent", [this, extent]() {m_clippingDistance = 0.1 * extent; });
			menu->addItem("20% extent", [this, extent]() {m_clippingDistance = 0.2 * extent; });
			menu->addItem("30% extent", [this, extent]() {m_clippingDistance = 0.3 * extent; });
			menu->addItem("40% extent", [this, extent]() {m_clippingDistance = 0.4 * extent; });
			menu->addItem("50% extent", [this, extent]() {m_clippingDistance = 0.5 * extent; });
			menu->addItem("60% extent", [this, extent]() {m_clippingDistance = 0.6 * extent; });
			menu->addItem("70% extent", [this, extent]() {m_clippingDistance = 0.7 * extent; });
			menu->addItem("80% extent", [this, extent]() {m_clippingDistance = 0.8 * extent; });
			menu->addItem("90% extent", [this, extent]() {m_clippingDistance = 0.9 * extent; });
			menu->addItem("100% extent", [this, extent]() {m_clippingDistance = extent; });
			getMenu()->addSubMenu(menu);
			m_clippingDistance = extent;
		}

		virtual void render(double dt)
		{
			// We set the last dt
			m_lastDt = dt;
			// Handles interactions with the keyboard
			handleKeys();

			// 0 - Matrices and initialisations
			glm::mat4 projectionMatrix = glm::perspective(glm::radians<float>(90), (float)getConfiguration().width / (float)getConfiguration().height, 0.01f, m_clippingDistance);
			glm::mat4 viewMatrix = m_camera.getInverseTransform();
			glm::mat4 modelMatrix = glm::mat4(1.0);
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
			glm::vec3 lightDirection = glm::normalize(glm::vec3(0.4f, -0.3f, -1.0f));
			glm::vec3 lightColor = glm::vec3(1, 1, 1);

			// 1 - We draw the scene
			{
				m_phongDirectionalShader.use();
				// We initialize the uniforms shared by every mesh
				m_phongDirectionalShader.trySetUniform("uni_mat_model", modelMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_view", viewMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_projection", projectionMatrix);
				m_phongDirectionalShader.trySetUniform("uni_mat_normal", normalMatrix);
				m_phongDirectionalShader.trySetUniform("uni_viewerPosition", m_camera.getPosition());
				m_phongDirectionalShader.trySetUniform("uni_light.direction", lightDirection);
				m_phongDirectionalShader.trySetUniform("uni_light.color", lightColor);
				// We draw every mesh 
				for (int cpt = 0; cpt < m_meshes.size(); ++cpt)
				{
					gl3::PhongMaterial * material = m_materials[cpt];
					material->getDiffuseTexture()->associateWithTextureUnit(0);
					m_phongDirectionalShader.trySetUniform("uni_diffuseTexture", 0);
					material->getSpecularTexture()->associateWithTextureUnit(1);
					m_phongDirectionalShader.trySetUniform("uni_specularTexture", 1);
					m_phongDirectionalShader.trySetUniform("uni_material.diffuseColor", material->getDiffuse());
					m_phongDirectionalShader.trySetUniform("uni_material.specularColor", material->getSpecular());
					m_phongDirectionalShader.trySetUniform("uni_material.shininess", material->getShininess());
					gl3::VertexArrayObject * vao = &m_phongDirectionalVaos[cpt];
					vao->bind();
					glDrawElements(GL_TRIANGLES, vao->eboSize(), GL_UNSIGNED_INT, nullptr);
					vao->unbind();
				}
				m_phongDirectionalShader.unuse();
			}
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}