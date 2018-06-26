#pragma once

#include <vector>
#include <typeinfo>


class Component;

class MessageInterface {
public:
	
};

struct ComponentPtr {
	Component* pComponent;
	size_t type;
};

class Entity {
public:
	Entity ( int id);
	~Entity ( );

	static void setMessageInterface (MessageInterface* interface) { m_msgI = interface; }

	template <class ComponentType>
	void addComponent (ComponentType* pComponent) {
		ComponentPtr ptr;
		ptr.pComponent	= (Component*)pComponent;
		ptr.type		= typeid(ComponentType).hash_code ( );

		//send message to messaging interface
	}

private:
	int m_id;
	std::vector<ComponentPtr>	m_components;
	static MessageInterface*	m_msgI;
};

