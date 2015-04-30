#pragma once

#include "myMath.h"
#include "defines.h"
#include "MassProperties.h"

#include <float.h>


namespace ong
{
	class World;

	class Collider;

	struct ContactIter;
	struct BVTree;


	struct BodyType
	{
		enum Type
		{
			Dynamic = 1,
			Static = 2
		};
	};

	struct BodyDescription
	{
		BodyType::Type type;
		Transform transform;
		vec3 linearMomentum;
		vec3 angularMomentum;
	};



	struct Proxy;

	class Body
	{
	public:

		//	--MANIPULATORS--

		void calculateMassData();

		void calculateProxy();

		void addCollider(Collider* collider);
		void removeCollider(Collider* collider);


		void setPosition(const vec3& position);

		void setUserData(void* pUserData);

		void applyImpulse(const vec3& impulse);
		void applyImpulse(const vec3& impulse, const vec3& point);
		void applyRelativeImpulse(const vec3& impulse);
		void applyRelativeImpulse(const vec3& impulse, const vec3& point);
		void applyAngularImpulse(const vec3& impulse);
		void applyRelativeAngularImpulse(const vec3& impulse);

		void applyForce(const vec3& force, float t);
		void applyForce(const vec3& force, const vec3& point, float t);
		void applyRelativeForce(const vec3& force, float t);
		void applyRelativeForce(const vec3& force, const vec3& point, float t);
		void applyTorque(const vec3& torque, float t);
		void applyRelativeTorque(const vec3& torque, float t);

		//	--ACCESORS--

		//return 0 if nothing got hit
		Collider* queryRay(const vec3& origin, const vec3& dir, float tmax = FLT_MAX);
		bool queryCollider(const Collider* collider);

		const Proxy& getProxy();

		Collider* getCollider();
		int getNumCollider();

		BVTree* getBVTree();

		Transform getTransform() const;

		void* getUserData() const;

		vec3 getPosition() const;
		const vec3& getWorldCenter() const;
		const vec3& getLocalCenter() const;

		const Quaternion& getOrientation() const;

		vec3 getLinearMomentum();
		vec3 getRelativeLinearMomentum();
		vec3 getAngularMomentum();
		vec3 getRelativeAngularMomentum();
		vec3 getLinearVelocity();
		vec3 getRelativeLinearVelocity();
		vec3 getAngularVelocity();
		vec3 getRelativeAngularVelocity();

		World* getWorld();

		Body* getNext();
		Body* getPrevious();

		BodyType::Type getType();

		int getNumContacts();
		ContactIter* getContacts();

		float getInverseMass();
		const mat3x3& getInverseInertia();


	ong_internal:

		//	 --CREATORS--

		Body(const BodyDescription& bodyDescription, World* pWorld, int idx);

		//	--MANIPULATORS--
		void setNext(Body*);
		void setPrevious(Body* body);

		void setIndex(int idx);

		void clearContacts();
		void addContact(ContactIter* iter);

		void setProxyID(int proxyID);
		int getProxyID();

		//	--ACCESSORS--

		int getIndex();

	private:
		enum
		{
			DYNAMIC = 1,
			STATIC = 2,
			TYPE = DYNAMIC + STATIC,
		};

		World* m_pWorld;

		int m_index;

		int m_flags;

		vec3 m_cm;

		int m_numCollider;
		BVTree* m_tree;
		Collider* m_pCollider;

		int m_numContacts;
		ContactIter* m_pContacts;

		int m_proxyID;

		void* m_pUserData;

		Body* m_pNext;
		Body* m_pPrev;
	};



	inline void Body::setIndex(int idx)
	{
		m_index = idx;
	}

	inline void Body::setProxyID(int idx)
	{
		m_proxyID = idx;
	}

	inline void Body::setPrevious(Body* pBody)
	{
		m_pPrev = pBody;
	}


	inline void Body::setNext(Body* pBody)
	{
		m_pNext = pBody;
	}


	inline void Body::setUserData(void* pUserData)
	{
		m_pUserData = pUserData;
	}


	inline int Body::getIndex()
	{
		return m_index;
	}


	inline Collider* Body::getCollider()
	{
		return m_pCollider;
	}

	inline int Body::getNumCollider()
	{
		return m_numCollider;
	}

	inline BVTree* Body::getBVTree()
	{
		return m_tree;
	}

	inline World* Body::getWorld()
	{
		return m_pWorld;
	}

	inline Body* Body::getNext()
	{
		return m_pNext;
	}

	inline Body* Body::getPrevious()
	{
		return m_pPrev;
	}

	inline void* Body::getUserData() const
	{
		return m_pUserData;
	}

	inline int Body::getProxyID()
	{
		return m_proxyID;
	}


	inline BodyType::Type Body::getType()
	{
		return (BodyType::Type)(m_flags & TYPE);
	}

	inline int Body::getNumContacts()
	{
		return  m_numContacts;
	}

	inline ContactIter* Body::getContacts()
	{
		return m_pContacts;
	}

	inline const vec3& Body::getLocalCenter() const
	{
		return m_cm;
	}

	inline void Body::clearContacts()
	{
		m_pContacts = 0;
		m_numContacts = 0;
	}
}