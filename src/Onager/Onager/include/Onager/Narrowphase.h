#pragma once

#include "defines.h"
#include "myMath.h"
#include "Contact.h"
#include "Allocator.h"
#include <stack>
#include <vector>

namespace ong
{
	
	class Body;
	class Collider;
	struct BVTree;


	struct Pair;


	// todo persistent contacts
	class ContactManager
	{
	public:
		ContactManager();

		void generateContacts(Pair* pairs, int numPairs, int maxContacts);
		void generateContact(Pair* pair);
		void updateContact(Contact* contact);
		void removeBody(Body* body);
		void removeContact(Contact* pContact);

		Contact** getContacts(int* numContacts);
		Contact** getCpContacts;

	private:
		void collide(Body* a, Body* b);
		
		void collide(BVTree* tree1, BVTree* tree2, BVTree* a, BVTree* b, const vec3& t, const mat3x3& rot);
		void collide(BVTree* tree, BVTree* a, Collider* b, const vec3& t, const mat3x3& rot);
		void collide(Collider* c1, Collider* c2);

		void removeContact(int contact);

		uint32 m_tick;
		std::vector<Contact*> m_contacts;
		std::vector<Contact*> m_cpContacts;
		Allocator<Contact> m_contactAllocator;
		Allocator<ContactIter> m_contactIterAllocator;
	};


	inline Contact** ContactManager::getContacts(int* numContacts)
	{
		if (numContacts)
			*numContacts = m_contacts.size();
		return m_contacts.data();

	}

}