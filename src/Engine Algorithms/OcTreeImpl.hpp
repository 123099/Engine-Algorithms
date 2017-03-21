#pragma once
#include <mge\core\GameObject.hpp>
#include "glm.hpp"

class OcTreeImpl
{
public:
	virtual void Insert(GameObject* gameObject) = 0;					//Inserts an object into the tree
	virtual void RetrieveObjectsInSpaceOf
	(std::vector<GameObject*>& returnList, GameObject* gameObject) = 0;	//Retrieves all the game objects that are in the same tree node as the provided object
	virtual void Clear() = 0;											//Clears the tree, clearing all the objects it referenced and all the layers it created
	virtual void Draw
	(const glm::mat4& viewMat, const glm::mat4& projectionMat) = 0;		//Draws the tree for debugging
	virtual std::string ToString() = 0;									//Returns a string with all the nodes and the amount of objects they have
private:
	virtual int GetNodeIndex(GameObject* gameObject) = 0;				//Retrieves the index of the child node the provided object is in, or a negative number otherwise
	virtual void Split() = 0;											//Splits this node into child nodes
	virtual bool IsSplit() = 0;											//Returns whether this node is split into child nodes
};