#include "Body.h"
#include "World.h"
#include "Narrowphase.h"
#include "BVH.h"

namespace ong
{

	Body::Body(const BodyDescription& descr, World* pWorld, int idx)
		: m_pWorld(pWorld),
		m_index(idx),
		m_flags(0),
		m_pCollider(nullptr),
		m_cm(0.0f, 0.0f, 0.0f),
		m_numContacts(0),
		m_pContacts(nullptr),
		m_tree(nullptr),
		m_numCollider(0)
	{

		m_aabb = {vec3(0, 0, 0), vec3(0,0,0)};
		m_cpAABB = { vec3(0, 0, 0), vec3(0, 0, 0) };

		m_flags |= descr.type;

		m_pWorld->m_r[m_index].p = descr.transform.p;
		m_pWorld->m_r[m_index].q = descr.transform.q;
		m_pWorld->m_p[m_index].l = descr.linearMomentum;
		m_pWorld->m_p[m_index].a = descr.angularMomentum;

		if (descr.continuousPhysics)
			m_flags |= CP;
	}


	void Body::addCollider(Collider* pCollider)
	{

		assert(pCollider->getBody() == NULL);

		pCollider->setBody(this);

		m_numCollider++;

		pCollider->setNext(m_pCollider);
		pCollider->setPrev(nullptr);

		if (m_pCollider)
			m_pCollider->setPrev(pCollider);

		m_pCollider = pCollider;

		calculateMassData();

		if (m_numCollider >= 2)
		{
			calculateTree();
		}
		//calculateAABB();
		//m_pWorld->updateProxy(m_proxyID);
	}

	void Body::removeCollider(Collider* collider)
	{
		assert(collider->getBody() == this);

		m_numCollider--;

		if (collider->getNext())
			collider->getNext()->setPrev(collider->getPrev());

		if (collider->getPrev())
			collider->getPrev()->setNext(collider->getNext());

		if (m_pCollider == collider)
			m_pCollider = collider->getNext();
		
		// remove contacts
		ContactIter* iter = m_pContacts;
		while (iter)
		{
			if (iter->contact->colliderA == collider || iter->contact->colliderB == collider)
			{
				m_pWorld->removeContact(iter->contact);
			}
			iter = iter->next;
		}

		collider->setBody(nullptr);

		if (m_numCollider != 0)
			calculateMassData();

		if (m_numCollider >= 2)
		{
			calculateTree();
		}
		calculateAABB();
		m_pWorld->updateProxy(m_proxyID);

	}



	void Body::calculateMassData()
	{
		float m = 0.0f;
		mat3x3 I = mat3x3(
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f));

		vec3 cm = vec3(0.0f, 0.0f, 0.0f);


		//if (m_flags & STATIC)
		//{

		//	vec3 oldPos = getPosition();
		//	m_pWorld->m_r[m_index].p = oldPos + cm;
		//	m_pWorld->m_m[m_index].localInvI = I;
		//	m_pWorld->m_m[m_index].invM = m;

		//	m_cm = cm;

		//	return;
		//}

		Collider* c = m_pCollider;
		for (; c != nullptr; c = c->getNext())
		{
			//if (c->isSensor())
			//	continue;

			const MassData& data = c->getMassData();
			Transform& t = c->getTransform();

			vec3 _cm = transformVec3(data.cm, t);

			m += data.m;

			cm += data.m * _cm;

			mat3x3 rot = toRotMat(t.q);
			mat3x3 _I = rot * data.I * transpose(rot);

			// center inertia to bodies frame of  reference
			_I = _I + data.m * (dot(_cm, _cm) * identity() - outerproduct(_cm, _cm));

			I = I + _I;
		}

		if (m != 0.0f)
			cm = 1.0f / m* cm;

		// center inertia to center of mass
		I = I - m * (dot(cm, cm) * identity() - outerproduct(cm, cm));
	  



		vec3 oldPos = getPosition();
		m_pWorld->m_r[m_index].p = oldPos + rotate(cm, getOrientation());
		if (m_flags & STATIC)
		{
			memset(&m_pWorld->m_m[m_index], 0, sizeof(MassState));
		}
		else
		{
			m_pWorld->m_m[m_index].invM = 1.0f / m;
			m_pWorld->m_m[m_index].localInvI = inverse(I);
		}

		m_cm = cm;
	}




	void Body::calculateAABB()
	{
		if (m_numCollider > 1)
			m_aabb = transformAABB(&m_tree->aabb, &getTransform());
		else if (m_numCollider == 1)
			m_aabb = transformAABB(&m_pCollider->getAABB(), &getTransform());
		else
			m_aabb = { getTransform().p, vec3(0, 0, 0) };

		if (m_flags & CP)
		{
			m_cpAABB.c = m_pWorld->m_cp[m_cpIndex].p1;
			m_cpAABB.e = m_aabb.e;
			mergeAABBAABB(&m_cpAABB, &m_aabb);
		}
		else
		{
			m_cpAABB = m_aabb;
		}
	}

	void Body::calculateTree()
	{	
		if (m_numCollider <= 1)
			return;

		if (m_tree)
			delete[] m_tree;
		m_tree = new BVTree[m_numCollider + m_numCollider - 1];

		constructBVTree(m_pCollider, m_numCollider, m_tree);
	}



	//void intersectTree(BVTree* tree, BVTree* n, const vec3& o, const vec3& d, float tmax, float tmin, Collider** c)
	//{
	//	float t;
	//	vec3 p;
	//	if (!intersectRayAABB(o, d, n->aabb, t, p) || t > tmax)
	//		return;
	//
	//	if (n->type == NodeType::LEAF)
	//	{
	//		Collider* _c = n->collider;
	//		switch (_c->getShapeType())
	//		{
	//		case ShapeType::HULL:
	//			const Transform& _t = _c->getTransform();
	//			vec3 _o = invTransformVec3(o, _t);
	//			vec3 _d = rotate(d, conjugate(_t.q));
	//			
	//			if (intersectRayHull(_o, _d, _c->getHull(), t, p) && t < tmax && t < tmin)
	//			{
	//				tmin = t;
	//				*c = _c;
	//			}
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		intersectTree(tree, tree + n->left, o, d, tmax, tmin, c);
	//		intersectTree(tree, tree + n->right, o, d, tmax, tmin, c);
	//	}
	//
	//}

	void intersectTree(BVTree* tree, BVTree* n, const vec3& o, const vec3& d, float tmax, float& tmin, RayQueryResult* hit)
	{

		std::stack<BVTree*> s;
		s.push(n);

		while (true)
		{
			float t;
			vec3 p;
			if (!intersectRayAABB(o, d, n->aabb, t, p) || t > tmax)
			{
				if (s.empty())
					break;
				n = s.top();
				s.pop();
			}


			if (n->type == NodeType::LEAF)
			{
				Collider* _c = n->collider;
				vec3 n = vec3(0,1,0);
				const Transform& _t = _c->getTransform();
				vec3 _o = invTransformVec3(o, _t);
				vec3 _d = rotate(d, conjugate(_t.q));

				switch (_c->getShape().getType())
				{
				case ShapeType::HULL:

					if (intersectRayHull(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				case ShapeType::SPHERE:
					if (intersectRaySphere(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				case ShapeType::CAPSULE:
					if (intersectRayCapsule(_o, _d, _c->getShape(), t, p, n) && t < tmax && t < tmin)
					{
						tmin = t;
						hit->t = tmin;
						hit->normal = rotate(n, _t.q);
						hit->collider = _c;
					}
					break;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}


			if (s.empty())
				break;

			n = s.top();
			s.pop();
		}

	}





	bool Body::queryRay(const vec3& origin, const vec3& dir, RayQueryResult* result,  float tmax)
	{
		memset(result, 0, sizeof(RayQueryResult));
        result->t = FLT_MAX;

		Transform t = getTransform();

		vec3 o = invTransformVec3(origin, t);
		vec3 d = rotate(dir, conjugate(t.q));


		float tmin = FLT_MAX; 

		if (m_numCollider > 1)
		{
			intersectTree(m_tree, m_tree, o, d, tmax, tmin, result);
		}
		else if (m_numCollider == 1)
		{
			vec3 p;
			if (intersectRayAABB(o, d, m_pCollider->getAABB(), tmin, p) && tmin < tmax)
			{
				vec3 n = vec3(0,1,0);

				const Transform& _t = m_pCollider->getTransform();
				vec3 _o = invTransformVec3(o, _t);
				vec3 _d = rotate(d, conjugate(_t.q));
				switch (m_pCollider->getShape().getType())
				{
				case ShapeType::HULL:
					if (intersectRayHull(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;
				case ShapeType::SPHERE:
					if (intersectRaySphere(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;
				case ShapeType::CAPSULE:
					if (intersectRayCapsule(_o, _d, m_pCollider->getShape(), tmin, p, n) && tmin < tmax)
					{
						result->collider = m_pCollider;
						result->normal = rotate(n, _t.q);
					}
					break;

				}
			}
		}

		if (result->collider != nullptr)
		{
			result->t = tmin;
			result->point = origin + tmin*dir;
            if (ong::lengthSq(result->normal) == 0)
            {
                printf("LOL");
            }
			result->normal = normalize(rotate(result->normal, t.q));
		}

		return (result->collider != nullptr);
	}

	bool overlap(const Collider* a, const Collider* b)
	{
		Transform t2;
		Transform t1;

		if (a->getBody())
			t1 = transformTransform(a->getTransform(), a->getBody()->getTransform());
		else
			t1 = a->getTransform();

		if (b->getBody())
			t2 = transformTransform(b->getTransform(), b->getBody()->getTransform());
		else
			t2 = b->getTransform();

		return overlap(a->getShape(), b->getShape(), t1, t2);
	}

	bool overlapTree(BVTree* tree, const Collider* collider, const Transform& transformA, const Transform& transformB)
	{
		Transform t = invTransformTransform(transformB, transformA);
		mat3x3 rot = toRotMat(t.q);

		BVTree* n = tree;
		std::stack<BVTree*> s;

		while (true)
		{
			if (!overlap(n->aabb, collider->getAABB(), t.p, rot))
			{
				if (s.empty())
					return false;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider->getShape(), collider->getShape(),
					transformTransform(n->collider->getTransform(), transformA), transformTransform(collider->getTransform(), transformB)))

					return true;
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return false;
			n = s.top();
			s.pop();
		}
	}

	bool overlapTree(BVTree* tree, BVTree* n, Collider* collider, const vec3& t, const mat3x3& rot, ColliderQueryCallBack callback)
	{
		std::stack<BVTree*> s;
		while (true)
		{
			if (!overlap(n->aabb, collider->getAABB(), t, rot))
			{
				if (s.empty())
					return true;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider, collider))
				{
					bool stop = !callback(collider, n->collider);
					if (stop)
						return false;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return true;
			n = s.top();
			s.pop();
		}
	}

	bool overlapTree(BVTree* tree, BVTree* n, ShapePtr shape, const Transform& transformA, const Transform& transformB)
	{
		Transform t = invTransformTransform(transformB, transformA);
		mat3x3 rot = toRotMat(t.q);

		std::stack<BVTree*> s;

		AABB shapeAABB = calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) });

		while (true)
		{
			if (!overlap(n->aabb, shapeAABB, t.p, rot))
			{
				if (s.empty())
					return false;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider->getShape(), shape,
					transformTransform(n->collider->getTransform(), transformA), transformB))	
				{
					return true;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return false;
			n = s.top();
			s.pop();
		}
	}


	bool overlapTree(BVTree* treeA, BVTree* treeB, const Transform& transformA, const Transform& transformB)
	{
		BVTree* a = treeA;
		BVTree* b = treeB;

		struct TreePair
		{
			BVTree* a;
			BVTree* b;
		};

		std::stack<TreePair> s;

		Transform t = invTransformTransform(transformB, transformA);
		mat3x3 rot = toRotMat(t.q);

		while (true)
		{
			if (!overlap(a->aabb, b->aabb, t.p, rot))
			{
				if (s.empty())
					return false;
				a = s.top().a;
				b = s.top().b;
				s.pop();
			}
			
			if (a->type == NodeType::LEAF)
			{
				if (b->type == NodeType::LEAF)
				{
					if (overlap(a->collider->getShape(), b->collider->getShape(),
						transformTransform(a->collider->getTransform(), transformA), transformTransform(b->collider->getTransform(), transformB)))
						return true;
				}
				else
				{
					b = treeB + b->left;
					s.push({ a, treeB + b->right });
					continue;
				}
			}
			else
			{
				if (b->type == NodeType::LEAF)
				{
					a = treeB + a->left;
					s.push({treeA + a->right, b});
					continue;
				}
				else
				{
					a = treeA + a->left;
					b = treeB + b->left;
					
					s.push({ treeA + a->right, treeB + b->right });
					s.push({ treeA + a->right, treeB + b->left});
					s.push({ treeA + a->left, treeB + b->right });
					continue;
				}
			}

			if (s.empty())
				return false;
			a = s.top().a;
			b = s.top().b;	 
			s.pop();

		}


	}

	bool overlapTree(BVTree* tree, BVTree* n, ShapePtr shape,const Transform& transform, const vec3& t, const mat3x3& rot, ShapeQueryCallBack callback, void* userData)
	{
		std::stack<BVTree*> s;
		
		AABB shapeAABB = calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) });

		while (true)
		{
			if (!overlap(n->aabb, shapeAABB, t, rot))
			{
				if (s.empty())
					return true;
				n = s.top();
				s.pop();
			}

			if (n->type == NodeType::LEAF)
			{
				if (overlap(n->collider->getShape(), shape,transformTransform(n->collider->getTransform(), n->collider->getBody()->getTransform()), transform))
				{
					bool stop = !callback(n->collider, userData);
					if (stop)
						return false;
				}
			}
			else
			{
				s.push(tree + n->right);
				n = tree + n->left;
				continue;
			}

			if (s.empty())
				return true;
			n = s.top();
			s.pop();
		}
	}


	bool overlap(Body* a, Body* b, const Transform& transformA, const Transform& transformB)
	{
		Transform ta = transformTransform(a->getTransform(), transformA);
		Transform tb = transformTransform(b->getTransform(), transformB);

		if (a->getNumCollider() > 1 && b->getNumCollider() > 1)
		{
			return overlapTree(a->getBVTree(), b->getBVTree(), ta, tb);
		}
		else if (a->getNumCollider() > 1)
		{
			return overlapTree(a->getBVTree(), b->getCollider(), ta, tb);
		}
		else if (b->getNumCollider() > 1)
		{
			return overlapTree(b->getBVTree(), a->getCollider(), tb, ta);
		}
		else
		{
			Transform t = invTransformTransform(tb, ta);
			mat3x3 rot = toRotMat(t.q);
			if (overlap(a->getCollider()->getAABB(), b->getCollider()->getAABB(), t.p, rot))
			{
				return (overlap(a->getCollider()->getShape(), b->getCollider()->getShape(),
					transformTransform(a->getCollider()->getTransform(), ta), transformTransform(b->getCollider()->getTransform(), tb)));
			}
		}

		return false;
	}

	bool Body::queryCollider(const Collider* collider)
	{
		Transform tCollider = Transform(vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1));

		if (collider->getBody())
		{
			tCollider = collider->getBody()->getTransform();
		}

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, collider, getTransform(), tCollider);
		}
		else if (m_numCollider == 1)
		{

			Transform t = invTransformTransform(tCollider, getTransform());
			mat3x3 rot = toRotMat(t.q);

			if (overlap(m_pCollider->getAABB(), collider->getAABB(), t.p, rot))
			{
				return overlap(m_pCollider, collider);
			}
		}

		return false;
	}

	bool Body::queryCollider(Collider* collider, ColliderQueryCallBack callback)
	{
		Transform tCollider = Transform(vec3(0,0,0), Quaternion(vec3(0,0,0), 1));

		if (collider->getBody())
		{
			tCollider = collider->getBody()->getTransform();
		}

		Transform t = invTransformTransform(tCollider, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, collider, t.p, rot, callback);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), collider->getAABB(), t.p, rot))
			{
				if (overlap(m_pCollider, collider))
				{
					return callback(collider, m_pCollider);;
				}
			}
		}

		return true;
	}

	bool Body::queryShape(ShapePtr shape, const Transform& transform)
	{

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, shape, getTransform(), transform);
		}
		else if (m_numCollider == 1)
		{
			Transform t = invTransformTransform(transform, getTransform());
			mat3x3 rot = toRotMat(t.q);

			if (overlap(m_pCollider->getAABB(), ong::calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) }), t.p, rot))
			{
				return overlap(m_pCollider->getShape(), shape, transformTransform(m_pCollider->getTransform(), getTransform()), transform);
			}
		}

		return false;
	}


	bool Body::queryShape(ShapePtr shape, const Transform& transform, ShapeQueryCallBack callback, void* userData)
	{
		Transform t = invTransformTransform(transform, getTransform());
		mat3x3 rot = toRotMat(t.q);

		if (m_numCollider > 1)
		{
			return overlapTree(m_tree, m_tree, shape, transform, t.p, rot, callback, userData);
		}
		else if (m_numCollider == 1)
		{
			if (overlap(m_pCollider->getAABB(), ong::calculateAABB(shape, { vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1) }), t.p, rot))
			{
				if ((overlap(m_pCollider->getShape(), shape, transformTransform(m_pCollider->getTransform(), getTransform()), transform)))
				{
					return callback(m_pCollider, userData);
				}
			}
		}

		return true;
	}

	vec3 Body::closestPoint(const vec3& p) const
	{
		
		if (m_numCollider == 1)
		{
			Transform t = transformTransform(m_pCollider->getTransform(), getTransform());
			vec3 _p = invTransformVec3(p, t);
			_p = closestPointOnShape(p, m_pCollider->getShape());
			_p = transformVec3(_p, t);
			return _p;
		}
		else if (m_numCollider > 1)
		{
			// query tree with sphere
			// find closest point on collidern and reduce
			// sphere radius until closest point is found

			vec3 minP = p;
			float minDist = FLT_MAX;

			Sphere s;
			s.c = p;
			// todo better estimate
			s.r = 0.5f*length(p - m_aabb.c);
			std::stack<BVTree*> stack;
				
			BVTree* n = m_tree;

			while (true)
			{
				if (!(overlap(&s, &n->aabb)))
				{
					if (stack.empty())
						return minP;
					n = stack.top();
					stack.pop();
				}
				if (n->type == NodeType::LEAF)
				{
					Transform t = transformTransform(m_pCollider->getTransform(), getTransform());
					vec3 _p = invTransformVec3(p, t);
					_p = closestPointOnShape(p, m_pCollider->getShape());
					_p = transformVec3(_p, t);
					
					float dist = lengthSq(p - _p);
					if (dist < minDist)
					{
						minDist = dist;
						minP = _p;
						s.r = sqrt(dist);
					}
				}
				else
				{
					stack.push(m_tree + n->right);
					n = m_tree + n->left;
					continue;
				}

				if (stack.empty())
					return minP;
				n = stack.top();
				stack.pop();
			}
		}
		else
		{
			return p;
		}
	}

	AABB Body::getMovingAABB()
	{
		return m_cpAABB;
	}

	const AABB& Body::getAABB()
	{
		return m_aabb;
	}

	Transform Body::getTransform() const
	{
		return Transform(getPosition(), getOrientation());
	}

	const vec3& Body::getWorldCenter() const
	{
		return m_pWorld->m_r[m_index].p;
	}


	vec3 Body::getPosition() const
	{
		return m_pWorld->m_r[m_index].p - rotate(m_cm, getOrientation());
		//return m_pWorld->getPosition(m_index) - m_cm;
	}

	const Quaternion& Body::getOrientation() const
	{
		return m_pWorld->m_r[m_index].q;
		//return m_pWorld->getOrientation(m_index);
	}


	vec3 Body::getLinearMomentum()
	{
		return m_pWorld->m_p[m_index].l;
	}

	vec3 Body::getRelativeLinearMomentum()
	{
		return rotate(getLinearMomentum(), conjugate(m_pWorld->m_r[m_index].q));
	}

	vec3 Body::getAngularMomentum()
	{
		return m_pWorld->m_p[m_index].a;
	}


	vec3 Body::getRelativeAngularMomentum()
	{
		return rotate(getAngularMomentum(), conjugate(m_pWorld->m_r[m_index].q));
	}


	vec3 Body::getLinearVelocity()
	{
		return m_pWorld->m_v[m_index].v;
	}


	vec3 Body::getRelativeLinearVelocity()
	{
		return rotate(getLinearVelocity(), conjugate(m_pWorld->m_r[m_index].q));
	}


	vec3 Body::getAngularVelocity()
	{
		mat3x3 q = toRotMat(m_pWorld->m_r[m_index].q);
		mat3x3 invI = q * m_pWorld->m_m[m_index].localInvI * transpose(q);

		/*return m_pWorld->m_p[m_index].a * invI;*/
		return invI * m_pWorld->m_p[m_index].a;
	}

	vec3 Body::getRelativeAngularVelocity()
	{
		return rotate(getAngularVelocity(), conjugate(m_pWorld->m_r[m_index].q));
	}


	float Body::getInverseMass()
	{
		return m_pWorld->m_m[m_index].invM;
		//return m_pWorld->getInvMass(m_index);
	}

	const mat3x3& Body::getInverseInertia()
	{
		return m_pWorld->m_m[m_index].localInvI;
		//return m_pWorld->getLocalInvInertia(m_index);
	}




	void Body::addContact(ContactIter* contact)
	{
		contact->next = m_pContacts;
		contact->prev = nullptr;

		if (m_pContacts)
			m_pContacts->prev = contact;

		m_pContacts = contact;

		m_numContacts++;
	}


	ContactIter* Body::removeContact(Contact* contact)
	{
		for (ContactIter* c = m_pContacts; c != 0; c = c->next)
		{
			if (c->contact == contact)
			{
				if (c->prev)
					c->prev->next = c->next;
				if (c->next)
					c->next->prev = c->prev;

				if (m_pContacts == c)
					m_pContacts = c->next;
				m_numContacts--;

				return c;
			}
		}

		assert(true);
		return 0;
	}


	void Body::setPosition(const vec3& position)
	{
		m_pWorld->m_r[m_index].p = position + rotate(m_cm, getOrientation());
		//m_pWorld->setPosition(m_index, position + m_cm);

		m_pWorld->updateProxy(m_proxyID);
	}

	void Body::setOrientation(const Quaternion& orientation)
	{
		m_pWorld->m_r[m_index].q = orientation;

		m_pWorld->updateProxy(m_proxyID);
	}



	void Body::applyImpulse(const vec3& impulse, const vec3& point)
	{
		m_pWorld->m_p[m_index].l += impulse;
		m_pWorld->m_p[m_index].a += cross(point - m_pWorld->m_r[m_index].p, impulse);
	}

	void Body::applyImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].l += impulse;

	}

	void Body::applyRelativeImpulse(const vec3& impulse)
	{
		applyImpulse(rotate(impulse, getOrientation()));
	}

	void Body::applyRelativeImpulse(const vec3& impulse, const vec3& point)
	{
		vec3 world = transformVec3(point, getTransform());
		applyImpulse(rotate(impulse, getOrientation()), world);
	}

	void Body::applyAngularImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].a += impulse;
	}

	void Body::applyRelativeAngularImpulse(const vec3& impulse)
	{
		m_pWorld->m_p[m_index].a += rotate(impulse, getOrientation());
	}

	void Body::applyForce(const vec3& force, float t)
	{
		applyImpulse(t * force);
	}

	void Body::applyForce(const vec3& force, const vec3& point, float t)
	{
		applyImpulse(t * force, point);
	}


	void Body::applyRelativeForce(const vec3& force, float t)
	{
		applyRelativeImpulse(t * force);
	}

	void Body::applyRelativeForce(const vec3& force, const vec3& point, float t)
	{
		applyRelativeImpulse(t * force, point);
	}

	void Body::applyTorque(const vec3& torque, float t)
	{
		applyAngularImpulse(t * torque);
	}


	void Body::applyRelativeTorque(const vec3& torque, float t)
	{
		applyRelativeAngularImpulse(t * torque);
	}


	void Body::setLinearMomentum(const vec3& momentum)
	{
		m_pWorld->m_p[m_index].l = momentum;
	}

	void Body::setAngularMomentum(const vec3& momentum)
	{
		m_pWorld->m_p[m_index].a = momentum;
	}


}