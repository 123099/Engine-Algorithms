#include "OcTreeBase.hpp"
#include "mge\config.hpp"
#include <sstream>

Mesh* OcTreeBase::m_debugMesh = nullptr;
ColorMaterial* OcTreeBase::m_debugMaterial = nullptr;

OcTreeBase::OcTreeBase(unsigned layer, const glm::vec3 & center, const glm::vec3 & halfExtents) :
	m_layer(layer),
	m_bounds(center, halfExtents),
	m_isSplit(false)
{
	if (m_debugMesh == nullptr)
	{
		m_debugMesh = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");
	}

	if (m_debugMaterial == nullptr)
	{
		m_debugMaterial = new ColorMaterial(glm::vec3(0.0f));
	}
}

void OcTreeBase::Insert(GameObject * gameObject)
{
	//If the node has already been split, attempt to get a child node index where the provided game object would fit
	//If such a node is found, insert the provided game object into that node, otherwise, insert the object into ourselves
	if (IsSplit() == true)
	{
		const int childNodeIndex = GetNodeIndex(gameObject);
		if (childNodeIndex >= 0)
		{
			//Insert the object into the found node and finish
			m_nodes[childNodeIndex]->Insert(gameObject);
			return;
		}
	}

	//If we are here, we are inserting the provided object into ourselves
	//Add the object into our object list
	m_gameObjects.push_back(gameObject);

	//Attemp to split if the number of game objects we hold exceeds the maximum amount
	const size_t objectCount = m_gameObjects.size();
	if (objectCount > MAX_OBJECTS && m_layer < MAX_LAYERS)
	{
		//Make sure that we haven't been split yet
		if (IsSplit() == false)
		{
			Split();
		}

		//Allocate all of our objects into the child nodes that they can go into
		for (int i = objectCount - 1; i >= 0; --i)
		{
			//Get the object we are currently working on
			GameObject* gameObject = m_gameObjects[i];

			//Get the child node index the object will fit into
			const int childNodeIndex = GetNodeIndex(gameObject);

			//If there is a child node to fit this object, put it in, otherwise, the object should remain in this node
			if (childNodeIndex >= 0)
			{
				//Insert the object into the found node
				m_nodes[childNodeIndex]->Insert(gameObject);

				//Remove the object from this node
				m_gameObjects.erase(m_gameObjects.begin() + i);
			}
		}
	}
}

//TODO: Possible optimization - when adding the children of all the child nodes of this node, skip checking whether the provided game object is inside one of the child nodes of the children nodes
void OcTreeBase::RetrieveObjectsInSpaceOf(std::vector<GameObject*>& returnList, GameObject * gameObject)
{
	//If we are split, search the child nodes for the provided object and add their objects to the return list
	if (IsSplit() == true)
	{
		//Attempt to find the index of the child node the provided object is in
		const int childNodeIndex = GetNodeIndex(gameObject);

		//If a child node containing the object is found, add the objects of that node to the return list
		//Otherwise, add the objects of all the children nodes to the return list, since the provided object is in our space
		//and may collide with our objects and any of the children's objects
		if (childNodeIndex >= 0)
		{
			m_nodes[childNodeIndex]->RetrieveObjectsInSpaceOf(returnList, gameObject);
		}
		else
		{
			for (const auto& childNode : m_nodes)
			{
				childNode->RetrieveObjectsInSpaceOf(returnList, gameObject);
			}
		}
	}

	//Add all of our objects to the return list, since the objects in our node may collide with any of the objects in the children nodes
	returnList.insert(returnList.end(), m_gameObjects.begin(), m_gameObjects.end());

	//If the return list contains the provided game object, remove it from the list to avoid collision with itself
	const auto foundGameObjectIterator = std::find(returnList.begin(), returnList.end(), gameObject);
	if (foundGameObjectIterator != returnList.end())
	{
		returnList.erase(foundGameObjectIterator);
	}
}

//TODO: Possible optimization - child nodes don't need to be cleared, since they will be destroyed and the destructor will clear it
void OcTreeBase::Clear()
{
	//If we have child nodes, go through all our children nodes and clear them
	if (IsSplit() == true)
	{
		const size_t childNodeCount = m_nodes.size();
		for (size_t i = 0; i < childNodeCount; ++i)
		{
			m_nodes[i]->Clear();
			m_nodes[i] = nullptr;
		}
	}

	//Clear our game object list
	m_gameObjects.clear();

	//Mark ourselves as no longer split
	m_isSplit = false;
}

void OcTreeBase::Draw(const glm::mat4& viewMat, const glm::mat4& projectionMat)
{
	if (IsSplit() == true)
	{
		for (const auto& childNode : m_nodes)
		{
			childNode->Draw(viewMat, projectionMat);
		}
	}

	//Set to draw wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((float)(MAX_LAYERS - m_layer));
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	//Draw ourselves
	if (m_gameObjects.size() > 0)
		m_debugMaterial->setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
	else
		m_debugMaterial->setDiffuseColor(glm::vec3(0.0f, 1.0f, 0.0f));
	
	m_debugMaterial->render(m_debugMesh, glm::translate(m_bounds.GetCenter() * 2) * glm::scale(m_bounds.GetExtents() * 2 * (1.0f - 0.04f * m_layer)), viewMat, projectionMat);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

std::string OcTreeBase::ToString()
{
	std::stringstream sstream;

	//Display our information
	sstream << "Layer: " << m_layer << ", Objects: " << m_gameObjects.size();

	//Display information from all the child nodes
	for(size_t i = 0; i < m_nodes.size(); ++i)
	{
		if (m_nodes[i] != nullptr)
		{
			sstream << '\n';
			for (size_t l = 0; l < m_layer; ++l) sstream << '\t';
			sstream << "Child Node " << i << ": " << m_nodes[i]->ToString();
		}
	}

	return sstream.str();
}

int OcTreeBase::GetNodeIndex(GameObject * gameObject)
{
	//TODO: game object Get World Position makes it SUPER slow
	//Determine where the object is in the node's space, looking towards the negative z axis
	//If the object is exactly on one of the edges of this node, then it doesn't fit in any child node
	const Bounds& objectBounds = gameObject->GetBounds();
	const glm::vec3& objectExtents = objectBounds.GetExtents();
	const glm::vec3& objectPosition = objectBounds.GetCenter();
	const glm::vec3& nodeCenter = m_bounds.GetCenter();

	const bool isInLeftHalf = objectPosition.x + objectExtents.x < nodeCenter.x;
	const bool isInRightHalf = objectPosition.x - objectExtents.x > nodeCenter.x;
	
	const bool isInBottomHalf = objectPosition.y + objectExtents.y < nodeCenter.y;
	const bool isInTopHalf = objectPosition.y - objectExtents.y > nodeCenter.y;
	
	const bool isInFrontHalf = objectPosition.z + objectExtents.z < nodeCenter.z;
	const bool isInBackHalf = objectPosition.z - objectExtents.z > nodeCenter.z;

	//Determine the node index based on the location of the object in our space
	if (isInTopHalf == true)
	{
		if (isInFrontHalf == true)
		{
			if (isInRightHalf == true)		//Top 1st quadrant
			{
				return 0;
			}
			else if (isInLeftHalf == true)	//Top 2nd quadrant
			{
				return 1;
			}
		}
		else if (isInBackHalf == true)
		{
			if (isInLeftHalf == true)		//Top 3rd quadrant
			{
				return 2;
			}
			else if (isInRightHalf == true)	//Top 4th quadrant
			{
				return 3;
			}
		}
	}
	else if (isInBottomHalf)
	{
		if (isInFrontHalf == true)
		{
			if (isInRightHalf == true)		//Bottom 1st quadrant
			{								  
				return 4;					  
			}								  
			else if (isInLeftHalf == true)	//Bottom 2nd quadrant
			{								  
				return 5;					  
			}								  
		}									  
		else if (isInBackHalf == true)		  
		{									  
			if (isInLeftHalf == true)		//Bottom 3rd quadrant
			{								  
				return 6;					  
			}								  
			else if (isInRightHalf == true)	//Bottom 4th quadrant
			{
				return 7;
			}
		}
	}

	return -1;
}

void OcTreeBase::Split()
{
	//Split the current node into 8 child nodes:
	//Index | Node Location, looking towards the negative z axis
	//0		| Top 1st quadrant
	//1		| Top 2nd quadrant
	//2		| Top 3rd quadrant
	//3		| Top 4th quadrant
	//4		| Bottom 1st quadrant
	//5		| Bottom 2nd quadrant
	//6		| Bottom 3rd quadrant
	//7		| Bottom 4th quadrant

	const glm::vec3& quarterExtents = m_bounds.GetExtents() * 0.5f;

	m_nodes[0] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3( quarterExtents.x, quarterExtents.y, -quarterExtents.z), quarterExtents);
	m_nodes[1] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3(-quarterExtents.x, quarterExtents.y, -quarterExtents.z), quarterExtents);
	m_nodes[2] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3(-quarterExtents.x, quarterExtents.y,  quarterExtents.z), quarterExtents);
	m_nodes[3] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3( quarterExtents.x, quarterExtents.y,  quarterExtents.z), quarterExtents);

	m_nodes[4] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3( quarterExtents.x, -quarterExtents.y, -quarterExtents.z), quarterExtents);
	m_nodes[5] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3(-quarterExtents.x, -quarterExtents.y, -quarterExtents.z), quarterExtents);
	m_nodes[6] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3(-quarterExtents.x, -quarterExtents.y,  quarterExtents.z), quarterExtents);
	m_nodes[7] = std::make_unique<OcTreeBase>(m_layer + 1, m_bounds.GetCenter() + glm::vec3( quarterExtents.x, -quarterExtents.y,  quarterExtents.z), quarterExtents);

	m_isSplit = true;
}

bool OcTreeBase::IsSplit()
{
	return m_isSplit;
}