#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Gameplay/Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Gameplay/Transform.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Texture2DData.h"
#include "Utilities/InputHelpers.h"
#include "Utilities/MeshBuilder.h"
#include "Utilities/MeshFactory.h"
#include "Utilities/NotObjLoader.h"
#include "Utilities/ObjLoader.h"
#include "Utilities/VertexTypes.h"

#define LOG_GL_NOTIFICATIONS

///////////////////////////////
// Ethan Kowalchuk 100752686 //
///////////////////////////////
/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
	case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
	case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
#endif
	default: break;
	}
}

GLFWwindow* window;
Camera::sptr camera = nullptr;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera->ResizeWindow(width, height);
}

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Ethan Kowalchuk 100752686", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

void RenderVAO(
	const Shader::sptr& shader,
	const VertexArrayObject::sptr& vao,
	const Camera::sptr& camera,
	const Transform::sptr& transform)
{
	shader->SetUniformMatrix("u_ModelViewProjection", camera->GetViewProjection() * transform->LocalTransform());
	shader->SetUniformMatrix("u_Model", transform->LocalTransform());
	shader->SetUniformMatrix("u_NormalMatrix", transform->NormalMatrix());
	vao->Render();
}

void ManipulateTransformWithInput(const Transform::sptr& transform, float dt) {
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		transform->MoveLocal(-1.2f * dt, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		transform->MoveLocal(1.2f * dt, 0.0f, 0.0f);
	}
}

struct Material
{
	Texture2D::sptr Albedo;
	Texture2D::sptr Albedo2;
	float           Shininess;
	float			usingSecond;
};

int AABB(glm::vec3 pos1, glm::vec3 pos2, float width1, float width2, float height1, float height2)
{
	if (pos1.x + width1 / 2 >= pos2.x - width2 / 2 &&
		pos1.x - width1 / 2 <= pos2.x + width2 / 2 &&
		pos1.y + height1 / 2 >= pos2.y - height2 / 2 &&
		pos1.y - height1 / 2 <= pos2.y + height2 / 2)
	{
		if (pos1.x <= pos2.x - width2 / 2 || pos1.x >= pos2.x + width2 / 2)//side
			return 1;
		else if (pos1.y <= pos2.y - height2 / 2 || pos1.y >= pos2.y + height2 / 2)//top
			return 2;
		else return 1;
	}
	else
		return 0;
}

VertexArrayObject::sptr updateText(VertexArrayObject::sptr vao, int text)
{
	switch (text)
	{
	case 0: return vao = ObjLoader::LoadFromFile("models/numbers/0.obj");
	case 1: return vao = ObjLoader::LoadFromFile("models/numbers/1.obj");
	case 2: return vao = ObjLoader::LoadFromFile("models/numbers/2.obj");
	case 3: return vao = ObjLoader::LoadFromFile("models/numbers/3.obj");
	case 4: return vao = ObjLoader::LoadFromFile("models/numbers/4.obj");
	case 5: return vao = ObjLoader::LoadFromFile("models/numbers/5.obj");
	case 6: return vao = ObjLoader::LoadFromFile("models/numbers/6.obj");
	case 7: return vao = ObjLoader::LoadFromFile("models/numbers/7.obj");
	case 8: return vao = ObjLoader::LoadFromFile("models/numbers/8.obj");
	case 9: return vao = ObjLoader::LoadFromFile("models/numbers/9.obj");
	}
}

int main() {
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	//VAOs
	VertexArrayObject::sptr brickVAO = ObjLoader::LoadFromFile("models/cube.obj");
	VertexArrayObject::sptr ballVAO = ObjLoader::LoadFromFile("models/sphere.obj");
	VertexArrayObject::sptr paddleVAO = ObjLoader::LoadFromFile("models/cube.obj");
	VertexArrayObject::sptr scoreOnesVAO = ObjLoader::LoadFromFile("models/numbers/0.obj");
	VertexArrayObject::sptr scoreTensVAO = ObjLoader::LoadFromFile("models/numbers/0.obj");
	VertexArrayObject::sptr livesVAO = ObjLoader::LoadFromFile("models/numbers/3.obj");
	VertexArrayObject::sptr backgroundVAO = ObjLoader::LoadFromFile("models/plane.obj");

	VertexArrayObject::sptr brickVAOs[25];
	int counter = 0;
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			brickVAOs[counter] = ObjLoader::LoadFromFile("models/cube.obj");
			counter++;
		}
	}

	// Load our shaders
	Shader::sptr shader = Shader::Create();
	shader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	shader->LoadShaderPartFromFile("shaders/frag_blinn_phong_textured.glsl", GL_FRAGMENT_SHADER);
	shader->Link();

	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, -5.0f);
	glm::vec3 lightCol = glm::vec3(0.9f, 0.85f, 0.5f);
	float     lightAmbientPow = 1.0f;
	float     lightSpecularPow = 1.0f;
	glm::vec3 ambientCol = glm::vec3(1.0f);
	float     ambientPow = 0.5f;
	float     shininess = 4.0f;
	float     lightLinearFalloff = 0.09f;
	float     lightQuadraticFalloff = 0.032f;
	float     textureMix = 0.0f;

	// These are our application / scene level uniforms that don't necessarily update
	// every frame
	shader->SetUniform("u_LightPos", lightPos);
	shader->SetUniform("u_LightCol", lightCol);
	shader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
	shader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
	shader->SetUniform("u_AmbientCol", ambientCol);
	shader->SetUniform("u_AmbientStrength", ambientPow);
	shader->SetUniform("u_Shininess", shininess);
	shader->SetUniform("u_LightAttenuationConstant", 1.0f);
	shader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
	shader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);
	shader->SetUniform("u_TextureMix", textureMix);

	// GL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// NEW STUFF

	// Create some transforms and initialize them
	Transform::sptr brickTransform = Transform::Create();
	Transform::sptr ballTransform = Transform::Create();
	Transform::sptr paddleTransform = Transform::Create();
	Transform::sptr scoreOnesTransform = Transform::Create();
	Transform::sptr scoreTensTransform = Transform::Create();
	Transform::sptr livesTransform = Transform::Create();
	Transform::sptr backgroundTransform = Transform::Create();

	Transform::sptr brickTransforms[25];
	for (int i = 0; i < 25; i++)
		brickTransforms[i] = Transform::Create();

	// We can use operator chaining, since our Set* methods return a pointer to the instance, neat!
	brickTransform->SetLocalPosition(0.0f, 1.0f, 0.0f)->SetLocalScale(0.2f, 0.1f, 0.1f);
	ballTransform->SetLocalPosition(-1.0f, -0.5f, 0.0f)->SetLocalScale(0.1f, 0.1f, 0.1f);
	paddleTransform->SetLocalPosition(-1.0f, -2.0f, 0.0f)->SetLocalScale(0.5f, 0.1f, 0.1f);
	scoreOnesTransform->SetLocalPosition(2.5f, 2.0f, 0.0f)->SetLocalScale(0.5f, 0.5f, 0.5f)->SetLocalRotation(90.0f, 0.0f, 0.0f);
	scoreTensTransform->SetLocalPosition(2.3f, 2.0f, 0.0f)->SetLocalScale(0.5f, 0.5f, 0.5f)->SetLocalRotation(90.0f, 0.0f, 0.0f);
	livesTransform->SetLocalPosition(-2.5f, 2.0f, 0.0f)->SetLocalScale(0.5f, 0.5f, 0.5f)->SetLocalRotation(90.0f, 0.0f, 0.0f);
	backgroundTransform->SetLocalPosition(0.0f, 0.0f, 0.0f)->SetLocalScale(3.0f, 3.0f, 3.0f)->SetLocalRotation(90.0f, 0.0f, 0.0f);
	
	// TODO: load textures
	// Load our texture data from a file
	Texture2DData::sptr brick1Map = Texture2DData::LoadFromFile("images/brick_texture_1.png");
	Texture2DData::sptr brick2Map = Texture2DData::LoadFromFile("images/brick_texture_2.png");
	Texture2DData::sptr paddleMap = Texture2DData::LoadFromFile("images/paddle_texture.png");
	Texture2DData::sptr ballMap = Texture2DData::LoadFromFile("images/ball_texture.png");
	Texture2DData::sptr textMap = Texture2DData::LoadFromFile("images/black.png");
	Texture2DData::sptr backgroundMap = Texture2DData::LoadFromFile("images/background.png");
	// Create a texture from the data

	Texture2D::sptr brick1 = Texture2D::Create();
	brick1->LoadData(brick1Map);
	Texture2D::sptr brick2 = Texture2D::Create();
	brick2->LoadData(brick2Map);
	Texture2D::sptr ball = Texture2D::Create();
	ball->LoadData(ballMap);
	Texture2D::sptr paddle = Texture2D::Create();
	paddle->LoadData(paddleMap);
	Texture2D::sptr text = Texture2D::Create();
	text->LoadData(textMap);
	Texture2D::sptr background = Texture2D::Create();
	background->LoadData(backgroundMap);
	

	// TODO: store some info about our materials for each object
	// We'll use a temporary lil structure to store some info about our material (we'll expand this later)
	Material brickMat;
	Material ballMat;
	Material paddleMat;
	Material textMat;
	Material backgroundMat;

	Material brickMats[25];
	for (int i = 0; i < 25; i++)
	{
		brickMats[i].Albedo = brick1;
		brickMats[i].Albedo2 = brick2;
		brickMats[i].Shininess = 64.0f;
		brickMats[i].usingSecond = 0.0f;
	}
		

	brickMat.Albedo = brick1;
	brickMat.Albedo2 = brick2;
	brickMat.Shininess = 64.0f;
	brickMat.usingSecond = 0.0f;

	ballMat.Albedo = ball;
	ballMat.Albedo2 = ball;
	ballMat.Shininess = 64.0f;
	ballMat.usingSecond = 0.0f;

	paddleMat.Albedo = paddle;
	paddleMat.Albedo2 = paddle;
	paddleMat.Shininess = 64.0f;
	paddleMat.usingSecond = 0.0f;

	textMat.Albedo = text;
	textMat.Albedo2 = text;
	textMat.Shininess = 64.0f;
	textMat.usingSecond = 0.0f;

	backgroundMat.Albedo = background;
	backgroundMat.Albedo2 = background;
	backgroundMat.Shininess = 64.0f;
	backgroundMat.usingSecond = 0.0f;

	camera = Camera::Create();
	camera->SetPosition(glm::vec3(0, 0, 10)); // Set initial position
	camera->SetUp(glm::vec3(0, 1, 0)); // Use a z-up coordinate system
	camera->LookAt(glm::vec3(0.0f)); // Look at center of the screen
	camera->SetFovDegrees(90.0f); // Set an initial FOV
	camera->SetOrthoHeight(3.0f);

	// We'll use a vector to store all our key press events for now
	std::vector<KeyPressWatcher> keyToggles;
	// This is an example of a key press handling helper. Look at InputHelpers.h an .cpp to see
	// how this is implemented. Note that the ampersand here is capturing the variables within
	// the scope. If you wanted to do some method on the class, your best bet would be to give it a method and
	// use std::bind
	
	//Make sure we are ortho
	camera->ToggleOrtho();

	//Object variables
	float ballSpeedX = 1.0f;
	float ballSpeedY = 1.0f;
	float brickWidth = 0.3f;
	float brickHeight = 0.1f;
	float paddleWidth = 1.0f;
	float paddleHeight = 0.1f;
	float ballRadius = 0.1f;
	int score = 0;
	int lives = 3;

	// Our high-precision timer
	double lastFrame = glfwGetTime();

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		//Check if game ends
		if (lives == 0)
			return 0;
		else if (score == 25)
			return 0;

		//Boundaries
		if (ballTransform->GetLocalPosition().x >= 3.0f)//Right
			ballSpeedX = -ballSpeedX;
		if (ballTransform->GetLocalPosition().x <= -3.0f)//Left
			ballSpeedX = -ballSpeedX;
		if (ballTransform->GetLocalPosition().y >= 3.0f)//Top
			ballSpeedY = -ballSpeedY;
		if (ballTransform->GetLocalPosition().y <= -3.0f)//Bottom
		{
			ballSpeedY = -ballSpeedY;
			ballTransform->SetLocalPosition(0.0f, 0.0f, 0.0f);
			lives--;
			livesVAO = updateText(livesVAO, lives);
		}
		//Move ball
		ballTransform->SetLocalPosition(glm::vec3(ballTransform->GetLocalPosition().x + ballSpeedX * dt,
												  ballTransform->GetLocalPosition().y + ballSpeedY * dt, 
												  0.0f));
		//Collision for paddle
		if (AABB(ballTransform->GetLocalPosition(),
			paddleTransform->GetLocalPosition(),
			ballRadius, paddleWidth, ballRadius, paddleHeight) == 2)
		{
			ballSpeedY = -ballSpeedY;
		}
		else if (AABB(ballTransform->GetLocalPosition(),
			paddleTransform->GetLocalPosition(),
			ballRadius, paddleWidth, ballRadius, paddleHeight) == 1)
		{
			ballSpeedX = -ballSpeedX;
		}

		ManipulateTransformWithInput(paddleTransform, dt);
		
		//Make sure paddle doesn't go off screen
		if (paddleTransform->GetLocalPosition().x > 2.5f)
			paddleTransform->SetLocalPosition(paddleTransform->GetLocalPosition().x - 1.2f * dt,
											  paddleTransform->GetLocalPosition().y,
											  paddleTransform->GetLocalPosition().z);
		if (paddleTransform->GetLocalPosition().x < -2.5f)
			paddleTransform->SetLocalPosition(paddleTransform->GetLocalPosition().x + 1.2f * dt,
											  paddleTransform->GetLocalPosition().y,
											  paddleTransform->GetLocalPosition().z);

		//Set background colour
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->Bind();
		// These are the uniforms that update only once per frame
		shader->SetUniformMatrix("u_View", camera->GetView());
		shader->SetUniform("u_CamPos", camera->GetPosition());

		// Tell OpenGL that slot 0 will hold the diffuse, and slot 1 will hold the specular
		shader->SetUniform("s_Diffuse", 0);
		shader->SetUniform("s_Diffuse2", 1);
		
		//Whole lotta brick stuff
		int counter = 0;
		//Nested for loops to create a 5x5 grid of bricks
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				//Setting up materials and shader stuff
				shader->SetUniform("u_usingSecond", brickMats[counter].usingSecond);
				shader->SetUniform("u_Shininess", brickMats[counter].Shininess);
				brickMats[counter].Albedo->Bind(0);
				brickMats[counter].Albedo2->Bind(1);

				//Creating bricks
				brickTransforms[counter]->SetLocalPosition(brickTransform->GetLocalPosition().x + x * 0.5f - 1.0f, 
														   brickTransform->GetLocalPosition().y + y * 0.3f, 
														   0.0f);
				brickTransforms[counter]->SetLocalScale(brickTransform->GetLocalScale());

				//Brick collision
				if (AABB(ballTransform->GetLocalPosition(),
					brickTransforms[counter]->GetLocalPosition(),
					ballRadius, brickWidth, ballRadius, brickHeight) == 2 && brickMats[counter].usingSecond < 3)
				{
					//Bounce
					ballSpeedY = -ballSpeedY;
					//If healthy, become unhealthy
					if (brickMats[counter].usingSecond == 0)
						brickMats[counter].usingSecond = 1;
					//If unhealthy, die
					else if (brickMats[counter].usingSecond == 1)
					{
						brickMats[counter].usingSecond = 3;
						score++;
						scoreOnesVAO = updateText(scoreOnesVAO, score % 10);
						scoreTensVAO = updateText(scoreTensVAO, score/10);
					}
				}
				else if (AABB(ballTransform->GetLocalPosition(),
					brickTransforms[counter]->GetLocalPosition(),
					ballRadius, brickWidth, ballRadius, brickHeight) == 1 && brickMats[counter].usingSecond < 3)
				{
					//Bounce
					ballSpeedX = -ballSpeedX;
					//If healthy, become unhealthy
					if (brickMats[counter].usingSecond == 0)
						brickMats[counter].usingSecond = 1;
					//If unhealthy, die
					else if (brickMats[counter].usingSecond == 1)
					{
						brickMats[counter].usingSecond = 3;
						score++;
						scoreOnesVAO = updateText(scoreOnesVAO, score % 10);
						scoreTensVAO = updateText(scoreTensVAO, score / 10);
					}
				}
				//If dead, don't render
				if(brickMats[counter].usingSecond < 3)
					RenderVAO(shader, brickVAOs[counter], camera, brickTransforms[counter]);
				counter++;
			}
		}
		//Ball VAO
		shader->SetUniform("u_usingSecond", ballMat.usingSecond);
		shader->SetUniform("u_Shininess", ballMat.Shininess);
		ballMat.Albedo->Bind(0);
		ballMat.Albedo2->Bind(1);
		RenderVAO(shader, ballVAO, camera, ballTransform);
		//Paddle VAO
		shader->SetUniform("u_usingSecond", paddleMat.usingSecond);
		shader->SetUniform("u_Shininess", paddleMat.Shininess);
		paddleMat.Albedo->Bind(0);
		paddleMat.Albedo2->Bind(1);
		RenderVAO(shader, paddleVAO, camera, paddleTransform);
		//Score VAO
		shader->SetUniform("u_usingSecond", textMat.usingSecond);
		shader->SetUniform("u_Shininess", textMat.Shininess);
		textMat.Albedo->Bind(0);
		textMat.Albedo2->Bind(1);
		RenderVAO(shader, scoreOnesVAO, camera, scoreOnesTransform);
		RenderVAO(shader, scoreTensVAO, camera, scoreTensTransform);
		//Lives VAO
		shader->SetUniform("u_usingSecond", textMat.usingSecond);
		shader->SetUniform("u_Shininess", textMat.Shininess);
		textMat.Albedo->Bind(0);
		textMat.Albedo2->Bind(1);
		RenderVAO(shader, livesVAO, camera, livesTransform);
		//Background VAO
		shader->SetUniform("u_usingSecond", backgroundMat.usingSecond);
		shader->SetUniform("u_Shininess", backgroundMat.Shininess);
		backgroundMat.Albedo->Bind(0);
		backgroundMat.Albedo2->Bind(1);
		RenderVAO(shader, backgroundVAO, camera, backgroundTransform);
		//

		glfwSwapBuffers(window);
		lastFrame = thisFrame;
	}

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}
