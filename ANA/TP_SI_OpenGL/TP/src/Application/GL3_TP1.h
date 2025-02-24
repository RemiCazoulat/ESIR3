#pragma once

#include <Application/BaseWithKeyboard.h>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <glm/common.hpp>
#include <gl3/Camera.h>
#include <gl3/GlobalState.h>
#include <fstream>

namespace Application
{
	// -----------------
	// Premier TP OpenGL
	// -----------------
	class GL3_TP1 : public BaseWithKeyboard
	{
		gl3::Camera m_camera;
		float m_cameraSpeed, m_cameraRotationSpeed;
		float m_lastDt;

	public:
		GL3_TP1()
		{}

		virtual ~GL3_TP1()
		{}

	protected:
		/// <summary>
		/// Method called when the size of the window changes.
		/// </summary>
		/// <param name="width">The new width.</param>
		/// <param name="height">The new height.</param>
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

		/// <summary>
		/// Loads a text file into a string.
		/// </summary>
		/// <param name="file">The file to load.</param>
		/// <returns></returns>
		static std::string loadTextFile(const std::filesystem::path & file)
		{
			if (!std::filesystem::exists(file))
			{
				std::cerr << "File " << file.string() << " does not exists" << std::endl;
				throw std::ios_base::failure(file.string() + " does not exists");
			}
			std::stringstream result;
			std::ifstream input(file);
			while (!input.eof())
			{
				std::string tmp;
				std::getline(input, tmp);
				result << tmp << std::endl;
			}
			return result.str();
		}

		virtual void initializeRendering()
		{
			// For debug purpose
			bool outputMessages = true;
			bool breakpointOnMessage = false;
			gl3::GlobalState::getSingleton()->enableDebugMode(outputMessages, breakpointOnMessage);

			// 0 - Camera setup
			m_camera.setPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			m_cameraSpeed = 1.0f;
			m_cameraRotationSpeed = Math::pi / 5.0;
			m_lastDt = 0.1f;

			// Vertices positions
			std::vector<glm::vec3> vertices = {
				glm::vec3(-0.5f, -0.5f, 0.0f),
				glm::vec3(0.5f, -0.5f, 0.0f),
				glm::vec3(0.0f,  0.5f, 0.0f)
			};
			// Vertices colors
			std::vector<glm::vec3> colors = {
				glm::vec3(1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, 1.0)
			};
			// Indexes for the EBO
			std::vector<GLuint> indexes = { 0, 1, 2 };

			// The base path of the shader files
			std::filesystem::path shaderPath = Config::dataPath() / "Shaders";

		}

		virtual void render(double dt)
		{
			// We set the last m_dt
			m_lastDt = dt;
			// Handles interactions with the keyboard
			handleKeys();
		}

		virtual void keyPressed(unsigned char key, int x, int y)
		{
			BaseWithKeyboard::keyPressed(key, x, y);
		}
	};

}