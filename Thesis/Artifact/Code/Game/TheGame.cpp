#include "Engine/General/Profiler/Profiler.hpp"
#include "Engine/Renderer/Lights/LightSystem.hpp"
#include "Engine/Renderer/Lights/Light3D.hpp"
#include "Engine/Renderer/DeviceWindow.hpp"
#include "Engine/Renderer/Material/MaterialBank.hpp"
#include "Engine/Renderer/Cameras/PlayerCamera3D.hpp"
#include "Engine/Renderer/General/TextureCubemap.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"
#include "Engine/EventSystem/GlobalEventSystem.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/Particles/ParticleSystemManager.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Renderer/Shaders/ShaderProgramParser.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Game/LightManager.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Renderer/ReflectionProbes/DynamicReflectionProbeManager.hpp"
#include "Engine/Renderer/ReflectionProbes/DynamicReflectionProbe.hpp"
#include "Engine/Renderer/Effects/Tonemapping.hpp"
#include "Engine/Math/Objects/AABB3.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/Renderer/Structures/BeirusMeshCollection.hpp"
#include "Engine/Renderer/Effects/OIT/OIT.hpp"
#include "Game/DebugSceneTypes/SceneTypeSSAO.hpp"
#include "Game/DebugSceneTypes/SceneTypeOIT.hpp"
#include "Game/DebugSceneTypes/SceneTypeDRP.hpp"
#include "Game/DebugSceneTypes/SceneTypeHDR.hpp"
#include "Game/DebugSceneTypes/SceneTypeForwardPlus.hpp"
#include "Game/DebugSceneTypes/SceneTypeShadows.hpp"
#include "Engine/Scene/ClickableObject.hpp"
#include "Engine/Scene/PickRay.hpp"
#include "Game/ProbeManager.hpp"

#include "imgui.h"
#include "Game/UI/UI.hpp"

class Light3D;
class Model;

TheGame* TheGame::s_theGame = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STATIC INIT DESTROY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Initialize() {

	s_theGame = new TheGame();
	BeirusEngine::RegisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
	PlayerCamera3D::Initialize(Vector3(0.f, 60.f, 0.f), 0.f, 90.f, 0.f);

	LightManager::Initialize();

	EventSystem::RegisterEventCallback("ShowHeatmapDisplay", &TheGame::SwitchToHeatmapView);
	EventSystem::RegisterEventCallback("ShowRegularDisplay", &TheGame::SwitchToRegularView);
	EventSystem::RegisterEventCallback("ShowDepthVisualizationDisplay", &TheGame::SwitchToDebugDepthView);

	Console::RegisterCommand("fbx_load",			"Loads models from fbx file.",						&LoadFBXFromFile);
	Console::RegisterCommand("load_mesh",			"Loads a Beirus mesh.",								&TheGame::LoadMesh);
	Console::RegisterCommand("save_mesh",			"Saves an FBX mesh out into a Beirus mesh.",		&TheGame::SaveMesh);
	Console::RegisterCommand("exposure_volume",		"Toggles exposure volume debug visualizer.",		&TheGame::ExposureVolumeToggle);
	Console::RegisterCommand("debug_scene_type",	"Change scene type. Options: none, ssao, hdr, oit",	&TheGame::SetDebugSceneType);
	Console::RegisterCommand("set_ambient_light",	"Sets the ambient light of the scene.",				&TheGame::SetAmbientLight);
	Console::RegisterCommand("exp",					"Sets exposure of the scene.",						&TheGame::SetExposure);




	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	// Setup style
	ImGui::StyleColorsDark();
	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Shutdown() {

	BeirusEngine::UnregisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
	delete s_theGame;

	LightManager::Shutdown();
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Render() {
	s_theGame->InternalRender();



	ImGui::NewFrame();
	ImGui::ShowDemoWindow(&show_demo_window);

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SWITCHING VIEW MODES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SwitchToHeatmapView(NamedProperties& eventData) {

	UNUSED(eventData);
	
	ShaderProgram* multiLightProg = ShaderProgram::CreateOrGetShaderProgram("BlinnPhong");
	Uniform* heatView = multiLightProg->GetUniform("gWhatToRender");
	heatView->SetData((void*)&MULTILIGHT_VIEW_MODE_HEATMAP);


	ShaderProgram* default2DProg = ShaderProgram::CreateOrGetShaderProgram("DefaultFullScreen");
	Uniform* debugView = default2DProg->GetUniform("gWhichView");
	debugView->SetData((void*)&FBO_VIEW_MODE_NORMAL);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SwitchToRegularView(NamedProperties& eventData) {

	UNUSED(eventData);

	ShaderProgram* multiLightProg = ShaderProgram::CreateOrGetShaderProgram("BlinnPhong");
	Uniform* heatView = multiLightProg->GetUniform("gWhatToRender");
	heatView->SetData((void*)&MULTILIGHT_VIEW_MODE_REGULAR);

	//WARNING: THIS IS REALLY DANGEROUS SETTING UNIFORMS UNTYPED, GIVE IT WRONG TYPE AND PROG MIGHT BLOW UP

	ShaderProgram* default2DProg = ShaderProgram::CreateOrGetShaderProgram("DefaultFullScreen");
	Uniform* debugView = default2DProg->GetUniform("gWhichView");
	debugView->SetData((void*)&FBO_VIEW_MODE_NORMAL);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SwitchToDebugDepthView(NamedProperties& eventData) {

	UNUSED(eventData);

	ShaderProgram* multiLightProg = ShaderProgram::CreateOrGetShaderProgram("BlinnPhong");
	Uniform* heatView = multiLightProg->GetUniform("gWhatToRender");
	heatView->SetData((void*)&MULTILIGHT_VIEW_MODE_REGULAR);


	ShaderProgram* default2DProg = ShaderProgram::CreateOrGetShaderProgram("DefaultFullScreen");
	Uniform* debugView = default2DProg->GetUniform("gWhichView");
	debugView->SetData((void*)&FBO_VIEW_MODE_DEPTH);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
static void RenderLoadingScreen() {

	AABB2 quad				= AABB2(Vector2(0.f, 0.f), Vector2(1920.f, 1080.f));
	Texture* loadingScreen	= Texture::CreateOrGetTexture("Data/Textures/LoadingScreen.png");
	Material* matToUse		= BeirusRenderer::GetOrGenerateMaterial(loadingScreen);

	StaticMesh2D newQuad;
	std::vector<Vertex2D_PCT> verts;

	AABB2 texCoords = AABB2(Vector2(1.f, 0.f), Vector2(0.f, 1.f));

	verts.push_back(Vertex2D_PCT(Vector2(quad.mins.x, quad.mins.y), RGBA::WHITE, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	verts.push_back(Vertex2D_PCT(Vector2(quad.maxs.x, quad.mins.y), RGBA::WHITE, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
	verts.push_back(Vertex2D_PCT(Vector2(quad.maxs.x, quad.maxs.y), RGBA::WHITE, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	verts.push_back(Vertex2D_PCT(Vector2(quad.mins.x, quad.maxs.y), RGBA::WHITE, Vector2(texCoords.mins.x, texCoords.mins.y)));

	newQuad.m_verts = verts;
	newQuad.m_material = matToUse;


	BeirusRenderer::DisableDepthTesting();

	MeshRenderer meshRenderer;

	MeshID smID = BeirusRenderer::GetScratch2DMeshID();
	Mesh* mesh = BeirusMeshCollection::Get()->GetMesh(smID);
	mesh->UpdateMesh(verts, newQuad.m_meshIndices);
	meshRenderer.RenderMeshWithMaterial(smID, matToUse, Matrix4::IDENTITY);

	SwapBuffers(g_displayDeviceContext);
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_hasMouseInput(false)
{

	RenderLoadingScreen();
	InputSystem::HideMouseCursor();

	m_scene = new Scene();

	#ifdef USING_SPONZA_SCENE
	InitializeSponzaScene();
	#endif

	InitializeParticleDefinitionsAndSystem();

	InitializeSkybox();

	//m_reflectionProbe = DynamicReflectionProbe::Create(SHADOWCASTING_LIGHT_POS + Vector3(10.f, 2.f, 0.f), m_scene);

	Tonemapping::Get()->AddExposureVolume(AABB3(Vector3(64.f, 3.f, -30.f), Vector3(-67.f, 40.f, -16.f)), 3.f);
	Tonemapping::Get()->AddExposureVolume(AABB3(Vector3(65.f, 3.f, -15.9f), Vector3(-70.f, 40.f, 7.f)), 1.f);
	Tonemapping::Get()->AddExposureVolume(AABB3(Vector3(-71.f, 3.f, 8.f), Vector3(66.f, 40.f, 28.f)), 3.f);
	Tonemapping::Get()->SetDefaultExposure(1.f);
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {
	delete m_skybox;
	m_skybox = nullptr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENE INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::InitializeSkybox() {

	m_skybox = new Model(Vector3::ZERO, PRIMITIVE_MESH_SKYBOX);

	const char* upTexture		= "Data/Textures/Skybox/Skybox_Miramar_UP.png";
	const char* downTexture		= "Data/Textures/Skybox/Skybox_Miramar_DOWN.png";
	const char* frontTexture	= "Data/Textures/Skybox/Skybox_Miramar_FRONT.png";
	const char* backTexture		= "Data/Textures/Skybox/Skybox_Miramar_BACK.png";
	const char* leftTexture		= "Data/Textures/Skybox/Skybox_Miramar_LEFT.png";
	const char* rightTexture	= "Data/Textures/Skybox/Skybox_Miramar_RIGHT.png";

	m_skyboxTex = new TextureCubemap(frontTexture, backTexture, upTexture, downTexture, rightTexture, leftTexture);
	m_skyboxTex->GenerateMipmap();

	m_skybox->m_model.Scale(Vector3(500.f));

	m_skybox->m_material = new Material("Skybox");
	m_skybox->m_material->CreateUniform("gCubemap",		UNIFORM_CUBEMAP,			1, 0, m_skyboxTex);
	m_skybox->m_material->m_renderState.m_backfaceCulling = false;

	m_scene->SetSkybox(m_skybox);
 }


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::InitializeSponzaScene() {

	MaterialBank::AddMaterialsFromXMLFile("Data/Models/Sponza/", "SponzaMaterials.xml");
	m_scene = Scene::LoadSceneFromDirectory("Data/Mesh/Sponza/");
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::InitializeParticleDefinitionsAndSystem() {

	ParticleSystemManager::LoadFile("Data/Particle/SmokeParticle_NoOIT.xml");
	ParticleSystemManager::LoadFile("Data/Particle/SmokeParticle.xml");
	ParticleSystemManager::SetScene(m_scene);
	//m_smokeParticleSystem = ParticleSystem::Create("LitSmoke", PARTICLE_SYSTEM_POS);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLICKABLES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::SelectClickables() {

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TICK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::UpdateDebugSceneType(float deltaSeconds) {

	if (InputSystem::GetKeyDown('1')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeForwardPlus();
	}
	else if (InputSystem::GetKeyDown('2')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeShadows();
	}
	else if (InputSystem::GetKeyDown('3')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeSSAO();
	}
	else if (InputSystem::GetKeyDown('4')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeHDR();
	}
	else if (InputSystem::GetKeyDown('5')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeDRP();
	}
	else if (InputSystem::GetKeyDown('6')) {
		delete s_theGame->m_debugSceneType;
		m_debugSceneType = new SceneTypeOIT();
	}


	if (m_debugSceneType) {
		m_debugSceneType->Update(deltaSeconds);
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds) {

	LightManager::Update(deltaSeconds);

	if (InputSystem::GetKeyDown('L')) {
		m_isCameraLocked = !m_isCameraLocked;
	}

	if (DeviceWindow::HasFocus()) {
		if (!m_isCameraLocked) {
			HandleMouseInputCheck();
		}
		if (!m_hasMouseInput) {
			if (!m_isCameraLocked) {
				HandlePlayerLook();
				HandlePlayerMovement(deltaSeconds);
			}
		}
	}


	if (InputSystem::GetKeyDown('F')) {
		BeirusRenderer::ToggleFXAA();
	}

	ShaderProgram* skyboxProg = ShaderProgram::CreateOrGetShaderProgram("Skybox");

	skyboxProg->GetUniform("gProj")->SetData((void*)BeirusRenderer::GetProjectionMatrix());
	skyboxProg->GetUniform("gView")->SetData((void*)BeirusRenderer::GetViewMatrix());

	UpdateDebugSceneType(deltaSeconds);

	ProbeManager::Get()->Update(deltaSeconds);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INPUT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::HandleMouseInputCheck() {

	if (InputSystem::GetKeyDown(VK_F2)) {
		m_hasMouseInput = !m_hasMouseInput;

		if (m_hasMouseInput) {
			InputSystem::ShowMouseCursor();
		}
		else {
			InputSystem::HideMouseCursor();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::HandlePlayerLook() {

	IntVector2 cursorDelta = DeviceWindow::GetCursorDelta();

	float pitch = BeirusRenderer::GetPlayerCamera()->m_pitchAboutX;

	if (pitch <= 89.f && cursorDelta.y > 0.f) {
		BeirusRenderer::GetPlayerCamera()->m_pitchAboutX += (float)cursorDelta.y * MOUSE_SENSITIVITY;
	}
	else if (pitch >= -89.f && cursorDelta.y < 0.f) {
		BeirusRenderer::GetPlayerCamera()->m_pitchAboutX += (float)cursorDelta.y * MOUSE_SENSITIVITY;
	}

	BeirusRenderer::GetPlayerCamera()->m_yawAboutY += (float)cursorDelta.x * MOUSE_SENSITIVITY;
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::HandlePlayerMovement(float deltaSeconds) {

	float moveSpeed = PLAYER_MOVE_SPEED * deltaSeconds;

	//FORWARD BACKWARD
	if (InputSystem::GetKey('W')) {
		BeirusRenderer::GetPlayerCamera()->m_position += BeirusRenderer::GetPlayerCamera()->GetForwardXZ() * moveSpeed; //Forward
	}
	else if (InputSystem::GetKey('S')) {
		BeirusRenderer::GetPlayerCamera()->m_position += BeirusRenderer::GetPlayerCamera()->GetForwardXZ() * -moveSpeed; //Backward
	}
	//LEFT RIGHT
	if (InputSystem::GetKey('A')) {
		BeirusRenderer::GetPlayerCamera()->m_position += BeirusRenderer::GetPlayerCamera()->GetLeftXZ() * -moveSpeed; //Left
	}
	else if (InputSystem::GetKey('D')) {
		BeirusRenderer::GetPlayerCamera()->m_position += BeirusRenderer::GetPlayerCamera()->GetLeftXZ() * moveSpeed; //Right
	}

	//UP DOWN
	if (InputSystem::GetKey(VK_SPACE)) {
		BeirusRenderer::GetPlayerCamera()->m_position += Vector3(0.f,  moveSpeed, 0.f); //Up
	}
	else if (InputSystem::GetKey('C')) {
		BeirusRenderer::GetPlayerCamera()->m_position += Vector3(0.f, -moveSpeed, 0.f); //Down
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::InternalRender() const {

	BeirusRenderer::RenderScene(m_scene);

	if (UISystem::IsHidden()) {
		return;
	}

	if (m_debugSceneType) {
		m_debugSceneType->Render();
	}

	ProbeManager::Get()->Render();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//COMMANDS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::LoadFBXFromFile(Command& args) {

	String fbxName;
	String filename;
	float scale = 1.f;
	float fps = 30.f;

	args.GetNextString(fbxName);
	args.GetNextFloat(scale);
	args.GetNextFloat(fps);

	filename = "Data/FBX/" + fbxName + ".FBX";

	if (filename.empty()) {
		return;
	}

	SceneImport* import = new SceneImport(filename, scale, fps);

	if (!import->m_importSuccessful) {
		Console::PrintOutput("ERROR: Failed to load file.", CONSOLE_WARNING);
	}
	else {
		int meshSize = import->m_meshBuilder.GetNumMeshes(); // import->m_meshes.size();
		String str = StringUtils::Stringf("Loaded %s. Had %i meshes.", filename.c_str(), meshSize);
		Console::PrintOutput(str, CONSOLE_VERIFY);
	}
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::LoadMesh(Command& args) {
	String filename;

	args.GetNextString(filename);

	String matName = "NULL";

	Console::PrintOutput("Mesh loaded successfully.", CONSOLE_VERIFY);
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::SaveMesh(Command& args) {
	String filename;

	args.GetNextString(filename);
	Console::PrintOutput("Mesh saved successfully.", CONSOLE_VERIFY);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::ExposureVolumeToggle(Command& args) {

	String toggle;

	args.GetNextString(toggle);

	if (toggle == "enable") {
		Tonemapping::Get()->EnableDebugVisualizer();
	}
	else if (toggle == "disable") {
		Tonemapping::Get()->DisableDebugVisualizer();
	}
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SetDebugSceneType(Command& args) {

	String sceneType;

	args.GetNextString(sceneType);

	if (sceneType == "none") {

	}
	else if (sceneType == "ssao") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeSSAO();
	}
	else if (sceneType == "hdr") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeHDR();
	}
	else if (sceneType == "oit") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeOIT();
	}
	else if (sceneType == "forward") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeForwardPlus();
	}
	else if (sceneType == "drp") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeDRP();
	}
	else if (sceneType == "shadows") {
		delete s_theGame->m_debugSceneType;
		s_theGame->m_debugSceneType = new SceneTypeShadows();
	}
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SetAmbientLight(Command& args) {

	float aLight;
	args.GetNextFloat(aLight);
	LIGHTS_AMBIENT_LIGHT.a = aLight;

	String output = StringUtils::Stringf("Ambient light level set to %.2f", aLight);
	Console::PrintOutput(output, CONSOLE_VERIFY);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::SetExposure(Command& args) {

	float exposure;
	args.GetNextFloat(exposure);
	Tonemapping::Get()->SetDefaultExposure(exposure);

	String output = StringUtils::Stringf("Scene exposure set to %.1f", exposure);
	Console::PrintOutput(output, CONSOLE_VERIFY);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::CreateReflectionProbe(Command& args) {

	DynamicReflectionProbe::Create(SHADOWCASTING_LIGHT_POS + Vector3(10.f, 2.f, 0.f), TheGame::GetScene());
}