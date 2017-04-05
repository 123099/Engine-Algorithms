#include <glm.hpp>
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

#include "mge/core/Renderer.hpp"

#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"

#include "mge/core/Camera.hpp"

#include "mge/core/GameObject.hpp"
#include "mge/materials/AbstractMaterial.hpp"

#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"

#include "mge/behaviours/RotatingBehaviour.hpp"
#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/behaviours/LookAt.hpp"
#include "mge\behaviours\BouncingMovement.hpp"
#include "mge\behaviours\FreeLookCamera.hpp"

#include "mge/util/DebugHud.hpp"

#include "mge/config.hpp"
#include "mge/MGEDemo.hpp"

#include "Engine Algorithms\OcTreeBase.hpp"
#include "Engine Algorithms\OcTreeFinal.hpp"

#include "Engine Algorithms\AABB.hpp"
#include "Engine Algorithms\OBB.hpp"

#define OCTREE_SIZE 100.0f

MGEDemo::MGEDemo()
{
	m_octree = new OcTreeFinal(0, glm::vec3(0.0f), glm::vec3(OCTREE_SIZE * 0.5f));
	//m_octree = new OcTreeBase(0, glm::vec3(0.0f), glm::vec3(10.0f));
}

void MGEDemo::initialize() 
{
    //setup the core part
    AbstractGame::initialize();

    //setup the custom part
	cout << "Initializing HUD" << endl;
	m_hud = new DebugHud(_window);
	cout << "HUD initialized." << endl << endl;
}

//build the game _world
void MGEDemo::_initializeScene()
{
    _renderer->setClearColor(0,0,0);

	Test(2500);
}

float timePassed = 0.0f;
int currentTest = 0;
int maxTests = 1;
void MGEDemo::_update(float deltaTime)
{
	//Clear the OcTree
	m_octree->Clear();

	UpdateOcTree();
	ProcessCollisions();

	AbstractGame::_update(deltaTime);

	timePassed += deltaTime;

	if (timePassed >= 30)
	{
		timePassed = 0;

		//Clear the world
		delete _world;
		_world = new World();

		//Start next test
		++currentTest;

		if(currentTest >= maxTests)
			_window->close();
		else
		{
			Sleep(1000);
			if (currentTest % 2 != 0)
				Test(25 * (int)std::pow(10, currentTest/2));
			else
				Test(10 * (int)std::pow(10, currentTest/2));
		}
	}
}

void MGEDemo::_render() {
    AbstractGame::_render();
	//m_octree->Draw(glm::inverse(_world->getMainCamera()->getWorldTransform()), _world->getMainCamera()->getProjection());
    _updateHud();
}

void MGEDemo::Test(unsigned objectCount)
{
	std::cout << "Starting testset " << objectCount << '\n';

	//add camera first (it will be updated last)
	Camera* camera = new Camera("camera", glm::vec3(0, 150, 150));
	camera->rotate(glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	FreeLookCamera* fc = new FreeLookCamera(*_window);
	fc->SetMoveSpeed(4);
	fc->SetRotationSpeed(0.005f);
	camera->setBehaviour(fc);
	_world->add(camera);
	_world->setMainCamera(camera);

	Mesh* teapotMeshS = Mesh::load(config::MGE_MODEL_PATH + "teapot_smooth.obj");

	//MATERIALS

	AbstractMaterial* colorMaterial = new ColorMaterial(glm::vec3(0.2f, 0, 0.2f));
	AbstractMaterial* textureMaterial2 = new TextureMaterial(Texture::load(config::MGE_TEXTURE_PATH + "bricks.jpg"));

	//SCENE SETUP
	for (unsigned i = 0; i < objectCount; ++i)
	{
		glm::vec3 position = (glm::vec3((float)std::rand(), (float)std::rand(), (float)std::rand()) / glm::vec3(RAND_MAX) - glm::vec3(0.5f)) * 2.0f * OCTREE_SIZE;
		GameObject* teapot = new GameObject("teapot", position);
		teapot->setMesh(teapotMeshS);
		teapot->setMaterial(textureMaterial2);
		teapot->setBehaviour(new BouncingMovement(OCTREE_SIZE));

		//Make half objects have AABB collider, and half an OBB
		if (i % 2 == 0)
		{
			teapot->SetCollider(new AABB(glm::vec3(0.5f)));
		}
		else
		{
			teapot->SetCollider(new OBB(glm::vec3(0.5f)));
		}

		_world->add(teapot);
	}

	//Wait a second for the system to be stable and avoid random outliers
	Sleep(1000);
}

void MGEDemo::UpdateOcTree()
{
	//Insert all the objects into the tree
	const int childCount = _world->getChildCount();
	for (int i = 0; i < childCount; ++i)
		m_octree->Insert(_world->getChildAt(i));
}

void MGEDemo::ProcessCollisions()
{
	std::vector<GameObject*> retrieved;
	for (int i = 0; i < _world->getChildCount(); ++i)
	{
		retrieved.clear();

		GameObject* oneObject = _world->getChildAt(i);
		m_octree->RetrieveObjectsInSpaceOf(retrieved, oneObject);

		for (size_t j = 0; j < retrieved.size(); ++j)
		{
			GameObject* otherObject = retrieved[j];
			if (oneObject != otherObject &&
				oneObject->GetCollider() != nullptr &&
				otherObject->GetCollider() != nullptr &&
				oneObject->GetCollider()->IsColliding(otherObject->GetCollider()))
			{
				oneObject->OnCollision(otherObject->GetCollider());
				otherObject->OnCollision(oneObject->GetCollider());
			}
		}
	}
}

void MGEDemo::_updateHud() {
    string debugInfo = "";
    debugInfo += string ("FPS:") + std::to_string((int)_fps)+"\n";
	debugInfo += "Total objects: " + std::to_string(_world->getChildCount()) + '\n';
	//debugInfo += m_octree.ToString();

    m_hud->setDebugInfo(debugInfo);
    m_hud->draw();
}

MGEDemo::~MGEDemo()
{
	//dtor
}
