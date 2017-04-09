#pragma once
#include "mge\core\GameObject.hpp"
#include "mge\core\Mesh.hpp"
#include "mge\materials\ColorMaterial.hpp"

#include "OcTreeImpl.hpp"
#include "Bounds.hpp"
#include "glm.hpp"
#include <array>
#include <vector>
#include <string>
#include <memory>

//TODO: Possible optimization - switch object vector to object hash map
class OcTreeFinal final : public OcTreeImpl
{
public:
	OcTreeFinal(unsigned layer, const glm::vec3& center, const glm::vec3& halfExtents);

	void Insert(GameObject* gameObject) override final;									//Inserts an object into the tree
	void RetrieveObjectsInSpaceOf
	(std::vector<GameObject*>& returnList, GameObject* gameObject) override final;		//Retrieves all the game objects that are in the same tree node as the provided object
	void Clear() override final;														//Clears the tree, clearing all the objects it referenced and all the layers it created
	void Draw
	(const glm::mat4& viewMat, const glm::mat4& projectionMat) override final;			//Draws the tree for debugging
	std::string ToString() override final;												//Returns a string with all the nodes and the amount of objects they have
private:
	static const unsigned MAX_LAYERS = 9;												//Maximum number of layers the tree will split into
	static const unsigned MAX_OBJECTS = 5;												//Maximum number of objects per layer until it splits

	static Mesh* m_debugMesh;															//Drawable mesh used for debug drawing the tree
	static ColorMaterial* m_debugMaterial;												//Material to draw the debug mesh

	unsigned m_layer;																	//The layer of this node

	Bounds m_bounds;																	//The bounding box defining the size of this node

	std::vector<GameObject*> m_gameObjects;												//The game objects that are inside this node of the tree
	std::array<std::unique_ptr<OcTreeFinal>, 8> m_nodes;								//The child nodes that this tree node has split to

	bool m_isSplit;																		//Keeps track of whether this node has been split or not

	int GetNodeIndex(GameObject* gameObject) override final;							//Retrieves the index of the child node the provided object is in, or a negative number otherwise
	void Split() override final;														//Splits this node into child nodes
	bool IsSplit() override final;														//Returns whether this node is split into child nodes
};

