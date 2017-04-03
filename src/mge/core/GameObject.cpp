#include <cassert>
#include <iostream>
using namespace std;

#include "mge/core/GameObject.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/behaviours/AbstractBehaviour.hpp"

GameObject::GameObject(std::string pName, glm::vec3 pPosition ):
	_name( pName ), m_localPosition(pPosition),
    _parent(NULL), _children(), _mesh( NULL ),_behaviour( NULL ), _material(NULL),
	m_bounds(glm::vec3(0.0f), glm::vec3(1.0f)), m_collider(nullptr) {}

GameObject::~GameObject()
{
    //detach all children
    cout << "GC running on:" << _name << '\n';

    while (_children.size() > 0) {
        GameObject* child = _children[0];
        remove (child);
        delete child;
    }

    //do not forget to delete behaviour, material, mesh, collider manually if required!
	delete _behaviour;
	delete m_collider;
}

void GameObject::setName (std::string pName)
{
    _name = pName;
}

std::string GameObject::getName() const
{
	return _name;
}

void GameObject::setTransform (const glm::mat4& pTransform)
{
    _transform = pTransform;
}

const glm::mat4& GameObject::getTransform()
{
	_transform[3] = glm::vec4(m_localPosition, 1);
    return _transform;
}

void GameObject::setLocalPosition (glm::vec3 pPosition)
{
	m_localPosition = pPosition;
}

const glm::vec3& GameObject::getLocalPosition() const
{	
	return m_localPosition;
}

void GameObject::setMaterial(AbstractMaterial* pMaterial)
{
    _material = pMaterial;
}

AbstractMaterial * GameObject::getMaterial() const
{
    return _material;
}

void GameObject::setMesh(Mesh* pMesh)
{
	_mesh = pMesh;
}

Mesh * GameObject::getMesh() const
{
    return _mesh;
}

void GameObject::setBehaviour(AbstractBehaviour* pBehaviour)
{
	_behaviour = pBehaviour;
	_behaviour->setOwner(this);
}

AbstractBehaviour * GameObject::getBehaviour() const
{
    return _behaviour;
}

void GameObject::SetCollider(Collider * collider)
{
	if (m_collider != nullptr)
	{
		m_collider->SetGameObject(nullptr);
	}

	m_collider = collider;
	m_collider->SetGameObject(this);
}

Collider * GameObject::GetCollider() const
{
	return m_collider;
}

void GameObject::setParent (GameObject* pParent) {
    //remove from previous parent
    if (_parent != NULL) {
        _parent->_innerRemove(this);
        _parent = NULL;
    }

    //set new parent
    if (pParent != NULL) {
        _parent = pParent;
        _parent->_innerAdd(this);
    }
}

GameObject* GameObject::getParent() {
    return _parent;
}

void GameObject::_innerAdd(GameObject* pChild)
{
    //set new parent
    pChild->_parent = this;
	_children.push_back(pChild);
}

void GameObject::_innerRemove (GameObject* pChild) {
    for (auto i = _children.begin(); i != _children.end(); ++i) {
        if (*i == pChild) {
            (*i)->_parent = NULL;
            _children.erase(i);
            return;
        }
    }
}

void GameObject::add (GameObject* pChild) {
    pChild->setParent(this);
}

void GameObject::remove (GameObject* pChild) {
    pChild->setParent(NULL);
}

////////////

//costly operation, use with care
glm::vec3 GameObject::getWorldPosition() const
{
	return glm::vec3(getWorldTransform()[3]);
}

//costly operation, use with care
glm::mat4 GameObject::getWorldTransform() const
{
    if (_parent == NULL) return _transform;
    else return _parent->getWorldTransform() * _transform;
}

void GameObject::SetBounds(const Bounds & bounds)
{
	m_bounds = bounds;
}

const Bounds & GameObject::GetBounds() const
{
	return m_bounds;
}

////////////

void GameObject::translate(glm::vec3 pTranslation)
{
	//setTransform(glm::translate(_transform, pTranslation));
	setLocalPosition(getLocalPosition() + pTranslation);
}

void GameObject::scale(glm::vec3 pScale)
{
	setTransform(glm::scale(_transform, pScale));
}

void GameObject::rotate(float pAngle, glm::vec3 pAxis)
{
	setTransform(glm::rotate(_transform, pAngle, pAxis));
}

//all game objects are updated in a backward loop, first the behaviour is updated, then all children are updated
void GameObject::update(float pStep)
{
	if (_behaviour) _behaviour->update(pStep);

    for (int i = _children.size()-1; i >= 0; --i ) {
        _children[i]->update(pStep);
    }

	m_bounds.SetCenter(m_localPosition);
	if(m_collider) m_collider->SetCenter(m_localPosition);
}

void GameObject::OnCollision(Collider * other)
{
	if (_behaviour)
	{
		_behaviour->OnCollision(other);
	}
}

int GameObject::getChildCount() {
    return _children.size();
}

GameObject* GameObject::getChildAt(int pIndex) {
    return _children[pIndex];
}

