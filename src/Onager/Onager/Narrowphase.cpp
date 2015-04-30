#include "Narrowphase.h"
#include "Broadphase.h"
#include "Body.h"
#include "Collider.h"
#include "SAT.h"
#include "BVH.h"
#include <float.h>


namespace ong
{
	void collideHullHull(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		SAT(a->getShape(), ta, b->getShape(), tb, manifold, feature);
	}


	void collideSphereSphere(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		const Sphere* sa = a->getShape();
		const Sphere* sb = b->getShape();

		vec3 ca = transformVec3(sa->c, *ta);
		vec3 cb = transformVec3(sb->c, *tb);

		float r = sa->r + sb->r;
		float dist = length(cb - ca) - r;

		if (dist < 0.0f)
		{
			manifold->normal = normalize(cb - ca);
			manifold->numPoints = 1;
			manifold->points[0].position = 0.5f * ((cb + sb->r*manifold->normal) + (ca - sb->r*manifold->normal));
			manifold->points[0].penetration = dist;

			feature->type = Feature::NONE;
		}
		else
		{
			manifold->numPoints = 0;
		}

	}

	void collideSphereCapsule(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		const Sphere* s = a->getShape();
		const Capsule* c = b->getShape();

		Transform t = invTransformTransform(*ta, *tb);

		vec3 ca = transformVec3(s->c, t);

		vec3 cb = closestPtPointSegment(ca, c->c1, c->c2);

		float r = s->r + c->r;
		float dist = length(cb - ca) - r;

		if (dist < 0.0f)
		{
			manifold->normal = rotate(normalize(cb - ca), tb->q);
			manifold->numPoints = 1;
			manifold->points[0].position = transformVec3(0.5f * ((cb + c->r*manifold->normal) + (ca - s->r*manifold->normal)), *tb);
			manifold->points[0].penetration = dist;

			feature->type = Feature::NONE;
		}
		else
		{
			manifold->numPoints = 0;
		}

	}

	void collideCapsuleCapsule(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		const Capsule* ca = a->getShape();
		const Capsule* cb = b->getShape();

		Transform t = invTransformTransform(*tb, *ta);

		vec3 p1 = ca->c1;
		vec3 q1 = ca->c2;

		vec3 p2 = transformVec3(cb->c1, t);
		vec3 q2 = transformVec3(cb->c2, t);

		float r = ca->r + cb->r;

		// todo check parallel
	
		//vec3 d1 = q1 - p1;
		//vec3 d2 = q2 - p2;

		//float epsilon = (3.0f*ong_MAX(ca->r, cb->r) + ong_MAX(abs(d1.x), abs(d2.x)) + ong_MAX(abs(d1.y), abs(d2.y)) + ong_MAX(abs(d1.z), abs(d2.z))) * FLT_EPSILON;

		//if (abs(dot(normalize(d1), normalize(d2))) - 1 >= -epsilon)
		//{
		//	Plane sidePlane[2];
		//	sidePlane[0].n = normalize(d1);
		//	sidePlane[0].d = dot(sidePlane[0].n, p1);

		//	sidePlane[1].n = normalize(-d1);
		//	sidePlane[1].d = dot(sidePlane[1].n, q1);

		//	manifold->normal = rotate(normalize(cross(d1, d2)), ta->q);
		//	manifold->numPoints = 0;

		//	for (int i = 0; i < 2; ++i)
		//	{
		//		if (distPointFatPlane(p2, sidePlane[i], epsilon) < 0)
		//			p2 = closestPtPointPlane(p2, sidePlane[i]);
		//		if (distPointFatPlane(q2, sidePlane[i], epsilon) < 0)
		//			q2 = closestPtPointPlane(q2, sidePlane[i]);
		//	}


		//	float dist1 = sqDistPointSegment(p2, p1, q1);
		//	if (dist1 < r*r)
		//	{
		//		manifold->points[0].penetration = sqrt(dist1);
		//		manifold->points[0].position = transformVec3(p2 - 0.5f*r * manifold->normal, *tb);
		//		manifold->numPoints++;
		//	}

		//	float dist2 = sqDistPointSegment(q2, p1, q1);
		//	if (dist2 < r*r)
		//	{
		//		manifold->points[manifold->numPoints].penetration = sqrt(dist2);
		//		manifold->points[manifold->numPoints].position = transformVec3(q2 - 0.5f*r * manifold->normal, *tb);
		//		manifold->numPoints++;
		//	}
		//	return;
		//}

		float s1, s2;
		vec3 c1, c2;
		float dist = sqrt(closestPtSegmentSegment(p1, q1, p2, q2, s1, s2, c1, c2)) - r;

		if (dist < 0.0f)
		{
			manifold->normal = rotate(normalize(c2 - c1), ta->q);
			manifold->numPoints = 1;
			manifold->points[0].position = transformVec3(0.5f * ((c2 + cb->r*manifold->normal) + (c1 - ca->r*manifold->normal)), *ta);
			manifold->points[0].penetration = dist;

			feature->type = Feature::NONE;
		}
		else
		{
			manifold->numPoints = 0;
		}

	}


	void collideSphereHull(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		const Sphere* s = a->getShape();
		const Hull* h = b->getShape();

		Transform t = invTransformTransform(*ta, *tb);

		vec3 c = transformVec3(s->c, t);

		vec3 p = closestPointOnHull(c, h);


		if (c != p)
		{
			// shallow contact
			float dist = length(c - p) - s->r;

			if (dist < 0.0f)
			{
				manifold->normal = rotate(p - c, tb->q);
				manifold->numPoints = 1;
				manifold->points[0].position = transformVec3(p, *tb);
				manifold->points[0].penetration = dist;

				feature->type = Feature::NONE;

				return;
			}

			manifold->numPoints = 0;
			return;
		}


		// deep contact
		float minDist = FLT_MAX;
		Plane* minPlane = nullptr;
		for (int i = 0; i < h->numFaces; ++i)
		{
			float dist = abs(distPointPlane(c, h->pPlanes[i]));

			if (dist < minDist)
				minPlane = h->pPlanes + i, minDist = dist;
		}

		assert(minPlane != nullptr);

		manifold->normal = rotate(minPlane->n, tb->q);
		manifold->numPoints = 1;
		manifold->points[0].position = transformVec3(closestPtPointPlane(c, *minPlane), *tb);
		manifold->points[0].penetration = -minDist - s->r;

		feature->type = Feature::NONE;

	}



	void collideCapsuleHull(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature)
	{
		const Capsule* c = a->getShape();
		const Hull* h = b->getShape();

		Transform t = invTransformTransform(*ta, *tb);

		vec3 c1 = transformVec3(c->c1, t);
		vec3 c2 = transformVec3(c->c2, t);

		vec3 p1, p2;
		float dist = sqrt(closestPtSegmentHull(c1, c2, h, p1, p2));


		if (dist > c->r)
		{
			manifold->numPoints = 0;
			return;
		}

		if (dist != 0.0f)
		{
			// shallow contact

			dist -= c->r;

			if (dist < 0.0f)
			{
				vec3 normal = normalize(p2 - p1);

				// check for parrallel
				for (int i = 0; i < h->numFaces; ++i)
				{
					float d1 = 1 - dot(h->pPlanes[i].n, -normal);
					float d2 = dot(h->pPlanes[i].n, (c2 - c1));

					if (d1 > -h->epsilon && d1 < h->epsilon && d2 > -h->epsilon && d2 < h->epsilon)
					{
						// todo factor

						HalfEdge* e = h->pEdges + h->pFaces[i].edge;
						do
						{
							vec3 p1 = h->pVertices[e->tail];
							vec3 p2 = h->pVertices[h->pEdges[e->twin].tail];

							Plane sidePlane;
							sidePlane.n = normalize(cross(h->pPlanes[i].n, p2 - p1));
							sidePlane.d = dot(p1, sidePlane.n);

							if (distPointFatPlane(c1, sidePlane, h->epsilon) < 0.0f)
							{
								c1 = closestPtPointPlane(c1, sidePlane);
							}

							if (distPointFatPlane(c2, sidePlane, h->epsilon) < 0.0f)
							{
								c2 = closestPtPointPlane(c2, sidePlane);
							}

						} while (e != h->pEdges + h->pFaces[i].edge);

						float dist1 = distPointPlane(c1, h->pPlanes[i]);
						float dist2 = distPointPlane(c2, h->pPlanes[i]);
						c1 = closestPtPointPlane(c1, h->pPlanes[i]);
						c2 = closestPtPointPlane(c2, h->pPlanes[i]);


						manifold->normal = rotate(normal, tb->q);
						manifold->numPoints = 2;
						manifold->points[0].position = transformVec3(c1, *tb);
						manifold->points[0].penetration = dist1;
						manifold->points[1].position = transformVec3(c2, *tb);
						manifold->points[1].penetration = dist2;

						return;
					}
				}


				manifold->normal = rotate(normal, tb->q);
				manifold->numPoints = 1;
				manifold->points[0].position = transformVec3(p2, *tb);
				manifold->points[0].penetration = dist;

				feature->type = Feature::NONE;

				return;
			}

			manifold->numPoints = 0;
			return;
		}


		// deep contact
		float minPlaneDist = -FLT_MAX;
		int minPlaneIdx = -1;
		for (int i = 0; i < h->numFaces; ++i)
		{
			vec3 p = getSegmentSupport(-h->pPlanes[i].n, c1, c2);

			float dist = distPointPlane(p, h->pPlanes[i]);

			if (dist > minPlaneDist)
				minPlaneDist = dist, minPlaneIdx = i;

		}

		float minEdgeDist = -FLT_MAX;
		int minEdgeIdx = -1;
		for (int i = 0; i < h->numEdges; i += 2)
		{
			HalfEdge* e = h->pEdges + i;
			HalfEdge* t = h->pEdges + i + 1;
			vec3 p = h->pVertices[e->tail];
			vec3 q = h->pVertices[t->tail];

			vec3 n = cross(c2 - c1, q - p);
			float l = length(n);
			if (l < 0.005f * sqrt(lengthSq(c2 - c1) * lengthSq(p - q)))
				continue;
			n = 1.0f / l * n;


			vec3 p1 = getSegmentSupport(-n, c1, c2);
			vec3 p2 = getHullSupport(n, h);

			float dist = dot(n, (p2 - p1));

			if (dist > minEdgeDist)
				minEdgeDist = dist, minEdgeIdx = i;
		}


		if (minPlaneDist > minEdgeDist)
		{
			// clip segment against sideplane
			HalfEdge* e = h->pEdges + h->pFaces[minPlaneIdx].edge;
			do
			{
				vec3 p1 = h->pVertices[e->tail];
				vec3 p2 = h->pVertices[h->pEdges[e->twin].tail];

				Plane sidePlane;
				sidePlane.n = normalize(cross(p2 - p1, h->pPlanes[minEdgeIdx].n));
				sidePlane.d = dot(p1, sidePlane.n);

				if (distPointFatPlane(c1, sidePlane, h->epsilon) < 0.0f)
				{
					c1 = closestPtPointPlane(c1, sidePlane);
				}

				if (distPointFatPlane(c2, sidePlane, h->epsilon) < 0.0f)
				{
					c2 = closestPtPointPlane(c2, sidePlane);
				}

			} while (e != h->pEdges + h->pFaces[minPlaneIdx].edge);

			float dist1 = distPointPlane(c1, h->pPlanes[minPlaneIdx]);
			float dist2 = distPointPlane(c2, h->pPlanes[minPlaneIdx]);
			c1 = closestPtPointPlane(c1, h->pPlanes[minPlaneIdx]);
			c2 = closestPtPointPlane(c2, h->pPlanes[minPlaneIdx]);

			manifold->normal = rotate(h->pPlanes[minPlaneIdx].n, tb->q);
			manifold->numPoints = 2;
			manifold->points[0].position = transformVec3(c1, *tb);
			manifold->points[0].penetration = dist1;
			manifold->points[1].position = transformVec3(c2, *tb);
			manifold->points[1].penetration = dist2;
		}
		else
		{
			vec3 p1 = h->pVertices[h->pEdges[minEdgeIdx].tail];
			vec3 p2 = h->pVertices[h->pEdges[h->pEdges[minEdgeIdx].twin].tail];

			float s, t;
			vec3 c3, p3;
			float dist = -sqrt(closestPtSegmentSegment(c1, c2, p1, p2, s, t, c3, p3));

			manifold->normal = rotate(normalize(p3 - c3), tb->q);
			manifold->numPoints = 1;
			manifold->points[0].position = transformVec3(0.5f*(c3 + p3), *tb);
			manifold->points[0].penetration = dist;
		}
	}







	void ContactManager::collide(BVTree* tree1, BVTree* tree2, BVTree* a, BVTree* b, const vec3& t, const mat3x3& rot)
	{
		if (!overlap(a->aabb, b->aabb, t, rot))
			return;

		if (a->type == NodeType::LEAF)
		{
			if (b->type == NodeType::LEAF)
			{
				collide(a->collider, b->collider);
			}
			else
			{
				collide(tree1, tree2, a, tree2 + b->left, t, rot);
				collide(tree1, tree2, a, tree2 + b->right, t, rot);
			}
		}
		else
		{
			if (b->type == NodeType::LEAF)
			{
				collide(tree1, tree2, tree1 + a->left, b, t, rot);
				collide(tree1, tree2, tree1 + a->right, b, t, rot);
			}
			else
			{
				collide(tree1, tree2, tree1 + a->left, tree2 + b->left, t, rot);
				collide(tree1, tree2, tree1 + a->left, tree2 + b->right, t, rot);
				collide(tree1, tree2, tree1 + a->right, tree2 + b->left, t, rot);
				collide(tree1, tree2, tree1 + a->right, tree2 + b->right, t, rot);
			}
		}

	}

	void ContactManager::collide(BVTree* tree, BVTree* a, Collider* b, const vec3& t, const mat3x3& rot)
	{
		if (!overlap(a->aabb, b->getAABB(), t, rot))
			return;

		if (a->type == NodeType::LEAF)
		{
			collide(a->collider, b);
		}
		else
		{
			collide(tree, tree + a->left, b, t, rot);
			collide(tree, tree + a->right, b, t, rot);
		}
	}


	void ContactManager::collide(Collider* ca, Collider* cb)
	{
		typedef void(*CollisionFunc)(Collider* a, Transform* ta, Collider* b, Transform* tb, ContactManifold* manifold, Feature* feature);
		static const CollisionFunc collisionFuncMatrix[ShapeType::COUNT][ShapeType::COUNT]
		{
			{collideSphereSphere, collideSphereCapsule, collideSphereHull},
			{ nullptr, collideCapsuleCapsule, collideCapsuleHull },
			{ nullptr, nullptr, collideHullHull }
		};

		if (cb->getShape().getType() < ca->getShape().getType())
			std::swap(ca, cb);


		Body* a = ca->getBody();
		Body* b = cb->getBody();


		Transform ta = transformTransform(ca->getTransform(), a->getTransform());
		Transform tb = transformTransform(cb->getTransform(), b->getTransform());


		m_contacts.push_back(Contact());
		Contact& contact = m_contacts.back();

		collisionFuncMatrix[ca->getShape().getType()][cb->getShape().getType()](ca, &ta, cb, &tb, &contact.manifold, &contact.feature);

		contact.colliderA = ca;
		contact.colliderB = cb;

		m_contactIters.push_back(ContactIter());
		ContactIter& iterA = m_contactIters.back();

		m_contactIters.push_back(ContactIter());
		ContactIter& iterB = m_contactIters.back();

		iterA.other = b;
		iterA.dir = 1;
		iterA.contact = &contact;
		iterB.other = a;
		iterB.dir = -1;
		iterB.contact = &contact;

		a->addContact(&iterA);
		b->addContact(&iterB);

		for (int i = 0; i < contact.manifold.numPoints; ++i)
		{
			contact.accImpulseN[i] = 0.0f;
			contact.accImpulseT[i] = 0.0f;
			contact.accImpulseBT[i] = 0.0f;
		}

		// warm starting
		// todo optimize me!

		for (Contact& oldContact : m_oldContacts)
		{
			if (oldContact.colliderA == contact.colliderA &&
				oldContact.colliderB == contact.colliderB &&
				oldContact.feature == contact.feature)
			{
				// copy old accumulated impulses over to new contact
				// todo make sure points are the same
				for (int i = 0; i < oldContact.manifold.numPoints && i < contact.manifold.numPoints; ++i)
				{
					contact.accImpulseN[i] = oldContact.accImpulseN[i];
					contact.accImpulseT[i] = oldContact.accImpulseT[i];
					contact.accImpulseBT[i] = oldContact.accImpulseBT[i];
				}

			}
		}
		cb = cb->getNext();


	}

	void ContactManager::collide(Body*a, Body*b)
	{
		if (a->getNumCollider() > 1 && b->getNumCollider() > 1)
		{
			Transform t = invTransformTransform(b->getTransform(), a->getTransform());
			mat3x3 rot = toRotMat(t.q);

			collide(a->getBVTree(), b->getBVTree(), a->getBVTree(), b->getBVTree(), t.p, rot);

		}
		else if (a->getNumCollider() > 1)
		{
			Transform t = invTransformTransform(b->getTransform(), a->getTransform());
			mat3x3 rot = toRotMat(t.q);

			collide(a->getBVTree(), a->getBVTree(), b->getCollider(), t.p, rot);

		}
		else if (b->getNumCollider() > 1)
		{
			Transform t = invTransformTransform(a->getTransform(), b->getTransform());
			mat3x3 rot = toRotMat(t.q);

			collide(b->getBVTree(), b->getBVTree(), a->getCollider(), t.p, rot);
		}
		else
		{
			collide(a->getCollider(), b->getCollider());
		}
	}

	void ContactManager::generateContacts(Pair* pairs, int numPairs, int maxContacts)
	{


		m_oldContacts = std::move(m_contacts);

		m_contacts.clear();
		m_contactIters.clear();

		m_contacts.reserve(maxContacts);
		m_contactIters.reserve(2 * maxContacts);


		for (int i = 0; i < numPairs; ++i)
		{
			Body* a = pairs[i].A;
			Body* b = pairs[i].B;

			collide(a, b);
		}
	}

}