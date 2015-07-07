
#include "include\DestructionManager.h"
#include "include\DestructibleObject.h"
#include "TheBrick\BrickInstance.h"
#include "TheBrick\Conversion.h"
#include "include\Space.h"


namespace sba
{
	const float CDestructionManager::NUB_STRENGTH = 10.0f;

	struct CallbackData
	{
		CDestructionManager* manager;
		TheBrick::CBrickInstance* brick;
	};

	bool setUpJoints(ong::Collider* a_Collider, void* a_Data)
	{
		TheBrick::CBrickInstance* brick = ((CallbackData*)a_Data)->brick;
		TheBrick::CBrickInstance* brick2 = (TheBrick::CBrickInstance*)a_Collider->getUserData();

		// bricks are equal or brick connection is already proccesed
		if (brick == brick2 || brick > brick2)
			return true;

		SBrickDestruction* destrInstance = brick->GetDestructionInstance();
		SBrickDestruction* destrInstance2 = brick2->GetDestructionInstance();

		for (int i = 0; i < destrInstance->numConnections; ++i)
		{
			if (destrInstance->connections[i].other == destrInstance2)
				return true;
		}

		float capacityX = 0.0f;
		float numNubsX = 0;
		float capacityZ = 0.0f;
		float numNubsZ = 0;
		float minX = FLT_MAX, maxX = -FLT_MAX, minZ = FLT_MAX, maxZ = -FLT_MAX;
		int dir = 0, dir2 = 0;
		for (size_t i = 0; i < brick->m_pBrick->GetNubs().size(); ++i)
		{
			ong::vec3 nubPos = ong::transformVec3(TheBrick::PuReToOng(brick->m_pBrick->GetNubs()[i].Position), brick->GetTransform());
			for (size_t j = 0; j < brick2->m_pBrick->GetNubs().size(); ++j)
			{
				//check for opposite direction
				if (brick->m_pBrick->GetNubs()[i].Direction.Y * brick2->m_pBrick->GetNubs()[j].Direction.Y > 0)
					continue;

				ong::vec3 nubPos2 = ong::transformVec3(TheBrick::PuReToOng(brick2->m_pBrick->GetNubs()[j].Position), brick2->GetTransform());

				if (ong::lengthSq(nubPos2 - nubPos) < 0.1)
				{
					capacityX += nubPos.x;
					capacityZ += nubPos.z;
					++numNubsX;
					++numNubsZ;
					minX = ong_MIN(nubPos.x, minX);
					maxX = ong_MAX(nubPos.x, maxX);
					minZ = ong_MIN(nubPos.z, minZ);
					maxZ = ong_MAX(nubPos.z, maxZ);

					dir = brick->m_pBrick->GetNubs()[i].Direction.Y;
					dir2 = brick2->m_pBrick->GetNubs()[j].Direction.Y;
				}
			}
		}

		CDestructionManager* manager = ((CallbackData*)a_Data)->manager;

		SJointData* jointDataX0 = manager->m_JointData();
		SJointData* jointDataX1 = manager->m_JointData();
		SJointData* jointDataZ0 = manager->m_JointData();
		SJointData* jointDataZ1 = manager->m_JointData();

		jointDataX0->fulcrum = minX - 0.5f* TheBrick::CBrick::SEGMENT_WIDTH;
		jointDataX1->fulcrum = maxX + 0.5f* TheBrick::CBrick::SEGMENT_WIDTH;
		jointDataZ0->fulcrum = minZ - 0.5f* TheBrick::CBrick::SEGMENT_WIDTH;
		jointDataZ1->fulcrum = maxZ + 0.5f* TheBrick::CBrick::SEGMENT_WIDTH;

		jointDataX0->baseCapacity = (-numNubsX * jointDataX0->fulcrum + capacityX) * CDestructionManager::NUB_STRENGTH;
		jointDataX1->baseCapacity = (-numNubsX * jointDataX1->fulcrum + capacityX) * CDestructionManager::NUB_STRENGTH;
		jointDataZ0->baseCapacity = (-numNubsZ * jointDataZ0->fulcrum + capacityZ) * CDestructionManager::NUB_STRENGTH;
		jointDataZ1->baseCapacity = (-numNubsZ * jointDataZ1->fulcrum + capacityZ) * CDestructionManager::NUB_STRENGTH;



		assert(destrInstance->numConnections < SBrickDestruction::MAX_CONNECTIONS &&
			destrInstance2->numConnections < SBrickDestruction::MAX_CONNECTIONS);

		SConnection* connection = destrInstance->connections + destrInstance->numConnections++;
		SConnection* connection2 = destrInstance2->connections + destrInstance2->numConnections++;


		SJoint* jointX0 = connection->joints[0][0] = manager->m_Joints();
		SJoint* jointX1 = connection->joints[0][1] = manager->m_Joints();
		SJoint* jointZ0 = connection->joints[1][0] = manager->m_Joints();
		SJoint* jointZ1 = connection->joints[1][1] = manager->m_Joints();

		SJoint* joint2X0 = connection2->joints[0][0] = manager->m_Joints();
		SJoint* joint2X1 = connection2->joints[0][1] = manager->m_Joints();
		SJoint* joint2Z0 = connection2->joints[1][0] = manager->m_Joints();
		SJoint* joint2Z1 = connection2->joints[1][1] = manager->m_Joints();


		jointX0->data = joint2X0->data = jointDataX0;
		jointX1->data = joint2X1->data = jointDataX1;
		jointZ0->data = joint2Z0->data = jointDataZ0;
		jointZ1->data = joint2Z1->data = jointDataZ1;


		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				connection->joints[i][j]->connection = connection;
				connection2->joints[i][j]->connection = connection2;

				connection->joints[i][j]->twin = connection2->joints[i][j];
				connection2->joints[i][j]->twin = connection->joints[i][j];
			}
		}
		
		connection->dir = dir;
		connection2->dir = dir2;

		connection->brick = destrInstance;
		connection2->brick = destrInstance2;

		connection->other = destrInstance2;
		connection2->other = destrInstance;

	}

	bool setUpBlocking(ong::Collider* a_Collider, void* a_Data)
	{
		TheBrick::CBrickInstance* brick = ((CallbackData*)a_Data)->brick;
		TheBrick::CBrickInstance* brick2 = (TheBrick::CBrickInstance*)a_Collider->getUserData();

		if (brick == brick2)
			return true;

		float height = 0;


	}

	CDestructionManager::CDestructionManager()
		:m_Joints(256),
		m_JointData(128),
		m_BrickDestruction(64)
		
	{
	}

	SBrickDestruction* calcCenterBrick(ong::vec3 a_CenterOfMass, TheBrick::CBrickInstance** a_Bricks, int a_NumBricks)
	{
		SBrickDestruction* centerBrick = nullptr;
		float min = FLT_MAX;

		for (int i = 0; i < a_NumBricks; ++i)
		{
			float dist = ong::lengthSq(a_Bricks[i]->GetTransform().p - a_CenterOfMass);
			if (dist < min)
			{
				min = dist;
				centerBrick = a_Bricks[i]->GetDestructionInstance();
			}
		}

		return centerBrick;
	}


	void CDestructionManager::BuildDestruction(CDestructibleObject* a_pObject, TheBrick::CBrickInstance** a_Bricks, int a_NumBricks)
	{

		//create destruction instances
		for (int i = 0; i < a_NumBricks; ++i)
		{
			a_Bricks[i]->SetDestructionInstance(m_BrickDestruction());
		}

		//set up joints
		for (int i = 0; i < a_NumBricks; ++i)
		{
			TheBrick::CBrickInstance* brick = a_Bricks[i];

			for (size_t j = 0; j < brick->m_pCollider.size(); ++j)
			{
				CallbackData data;
				data.brick = brick;
				data.manager = this;

				ong::Transform transformUp = brick->m_pCollider[j]->getTransform();
				transformUp.p.y += 0.1f* TheBrick::CBrick::SEGMENT_HEIGHT;

				a_pObject->m_pBody->queryShape(brick->m_pCollider[j]->getShape(), ong::transformTransform(transformUp, a_pObject->GetTransform()), setUpJoints, &data);

				ong::Transform transformDown = brick->m_pCollider[j]->getTransform();
				transformUp.p.y -= 0.1f* TheBrick::CBrick::SEGMENT_HEIGHT;

				a_pObject->m_pBody->queryShape(brick->m_pCollider[j]->getShape(), ong::transformTransform(transformDown, a_pObject->GetTransform()), setUpJoints, &data);
			}


		}

		// check for bricks blocking joints
		for (int i = 0; i < a_NumBricks; ++i)
		{
			//todo
		}

		SBrickDestruction* centerBrick = calcCenterBrick(a_pObject->m_pBody->getLocalCenter(), a_Bricks, a_NumBricks);
		a_pObject->SetCenterBrick(centerBrick);
	}


	bool CDestructionManager::FindAugmentedPath(SBrickDestruction* a_pBrick, SBrickDestruction* a_pSink, int a_Axis, float* a_pMinCapacity, std::vector<SJoint*>& a_rAugmentedPath, int tick)
	{
		if (a_pBrick == a_pSink)
			return true;

		a_pBrick->tick = tick;

		for (size_t i = 0; i < a_pBrick->numConnections; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				SConnection* connection = a_pBrick->connections + i;
				SJoint* joint = a_pBrick->connections[i].joints[a_Axis][j];

				if (connection->other->tick != tick &&
					joint->capacity - joint->flow > 0)
				{
					a_rAugmentedPath.push_back(joint);
					if (FindAugmentedPath(connection->other, a_pSink, a_Axis, a_pMinCapacity, a_rAugmentedPath, tick))
					{
						if (joint->capacity - joint->flow < *a_pMinCapacity)
							*a_pMinCapacity = joint->capacity - joint->flow;
						return true;
					}
					else
					{
						a_rAugmentedPath.pop_back();
					}
				}
			}
		}

		return false;
	}


	float CDestructionManager::MaxFlow(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, int a_Axis)
	{
		std::vector<SJoint*> augmentedPath;

		// build maximum flow network
		while (true)
		{
			float minCapacity = FLT_MAX;
			if (!FindAugmentedPath(a_pBrick, a_pCenterBrick, a_Axis, &minCapacity, augmentedPath, m_Tick++))
				break;

			for (auto pJoint : augmentedPath)
			{
				pJoint->flow += minCapacity;
				pJoint->twin->flow -= minCapacity;
			}
			augmentedPath.clear();
		}

		// find maximum flow on source brick
		float maxFlow = 0;
		for (int i = 0; i < a_pBrick->numConnections; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				maxFlow += a_pBrick->connections[i].joints[a_Axis][j]->flow;
			}
		}

		return maxFlow;
	}


	bool CDestructionManager::CheckAxis(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse, int a_Axis)
	{
		// get maximum flow
		float maxFlow = MaxFlow(a_pBrick, a_pCenterBrick, a_Axis);
		 
		//if maximum flow is less than impulse break object
		if (maxFlow > 0 && maxFlow < 1.0f)
		{
			std::vector<SBrickDestruction*> selection;
			std::vector<SJoint*> front;
			
			int selectionTick = m_Tick++;
			if (!BreakGraph(a_pBrick, a_pCenterBrick, maxFlow, a_Axis, selection, front, selectionTick))
				return false;

			// check if any of the front brick is wedged between two bricks
			for (int i = 0; i < front.size(); ++i)
			{
				SBrickDestruction* brick = front[i]->connection->brick;
				for (int j = 0; j < 2; ++j)
				{
					int numUp = 0;
					int numDown = 0;

					for (int j = 0; j < brick->numConnections; ++j)
					{
						if (brick->connections[j].dir == 1)
						{
							if (numDown > 0)
								return false;
							else
								++numUp;
						}
						else if (brick->connections[j].dir == -1)
						{
							if (numUp > 0)
								return false;
							else
								++numDown;
						}
					}

					brick = front[i]->connection->other;
				}
			}

			

		}

		return false;
	}


	bool CDestructionManager::BreakGraph(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, float a_MaxFlow, int a_Axis, std::vector<SBrickDestruction*>& a_rSelection, std::vector<SJoint*>& a_rFront, int a_Tick)
	{
		a_pBrick->tick = a_Tick;
		a_rSelection.push_back(a_pBrick);

		for (int i = 0; i < a_pBrick->numConnections; ++i)
		{
			SConnection* pConnection = a_pBrick->connections + i;
			
			if (pConnection->other->tick != a_Tick)
			{
				int valid = 0;
				for (int j = 0; j < 2; ++j)
				{
					if (pConnection->joints[a_Axis][j]->capacity > 0)
					{
						if (pConnection->joints[a_Axis][j]->capacity >= pConnection->joints[a_Axis][j]->flow / a_MaxFlow)
						{
							if (!BreakGraph(pConnection->other, a_pCenterBrick, a_MaxFlow, a_Axis, a_rSelection, a_rFront, a_Tick))
								return false;
						}
						else
						{
							a_rFront.push_back(pConnection->joints[a_Axis][j]);
						}
						break;
					}
					else
					{
						valid++;
					}
				}

				// if connection has no valid path to next brick dont break
				if (valid >= 2)
					return false;
			}

		}

		return true;
	}

	void CDestructionManager::Destroy(SBrickDestruction* a_pBrick, const std::vector<SBrickDestruction*>& a_rSelection, const std::vector<SJoint*>& a_rFront, ong::vec3& a_rPoint, ong::vec3& a_rImpulse, int a_Axis, int a_Tick)
	{
		for (auto front : a_rFront)
		{
			SConnection* a = front->connection;
			SConnection* b = front->twin->connection;

			//todo fulcrum
		
			// remove connection
			*a = a->brick->connections[--a->brick->numConnections];
			*b = b->brick->connections[--b->brick->numConnections];
		}

		SImpulse newImpulse;
		newImpulse.brick = a_pBrick;
		newImpulse.point = a_rPoint;
		newImpulse.impulse = a_rImpulse;

		m_inpulses.push_back(newImpulse);

		//for (int i = 0; i < a_rSelection.size(); ++i)
		//{
		//	SBrickDestruction* brick = a_rSelection[i];

		//	for (int i = 0; i < brick->numBlocking; ++i)
		//	{
		//		//free blocks
		//		if (brick->blocking[i]->connection->brick->tick == a_Tick)
		//		{
		//			brick->blocking[i]->data->numBlocking--;
		//			brick->blocking[i] = brick->blocking[--brick->numBlocking];
		//		}

		//	}
		//}


	}

	void ClearFlowNetwork(SBrickDestruction* a_pBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse, int tick)
	{
		a_pBrick->tick = tick;
		for (int i = 0; i < a_pBrick->numConnections; ++i)
		{

			for (int d = 0; d < 2; ++d)
			{
				for (int j = 0; j < 2; ++j)
				{
					SJoint* joint = a_pBrick->connections[i].joints[d][j];
					joint->flow = 0;


					if (d == 0)
						joint->capacity =
						a_pBrick->connections[i].dir * joint->data->baseCapacity / ((a_rPoint.x - joint->data->fulcrum) * a_rImpulse.y - (a_rPoint.y - joint->data->posY) * a_rImpulse.x);
					else if (d == 1)
						joint->capacity =
						-a_pBrick->connections[i].dir * joint->data->baseCapacity / ((a_rPoint.y - joint->data->posY) * a_rImpulse.z - (a_rPoint.z - joint->data->fulcrum) * a_rImpulse.y);

					if (joint->data->numBlocking && joint->capacity > 0)
					{
						joint->capacity = 1.0f;
					}
					else
					{
						joint->capacity = ong_MIN(1.0f, ong_MAX(0.0f, joint->capacity));
					}
				}
			}

			if (a_pBrick->connections[i].other->tick != tick)
			{
				ClearFlowNetwork(a_pBrick->connections[i].other, a_rPoint, a_rImpulse, tick);
			}
		}
	}
	bool CDestructionManager::AddImpulse(TheBrick::CBrickInstance* a_pBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse)
	{
		CDestructibleObject* pObject = dynamic_cast<CDestructibleObject*>(a_pBrick->GetGameObject());
		assert(pObject);

		SBrickDestruction* destrInstance = a_pBrick->GetDestructionInstance();

		if (destrInstance == pObject->getCenterBrick())
			return false;

		//clear
		ong::Transform transformToBrick = ong::invTransform(pObject->GetTransform());
		ong::vec3 localPoint = ong::transformVec3(a_rPoint, transformToBrick);
		ong::vec3 localImpulse = ong::rotate(a_rImpulse, transformToBrick.q);

		ClearFlowNetwork(destrInstance, localPoint, localImpulse, m_Tick++);

		// check axis
		if (CheckAxis(destrInstance, pObject->getCenterBrick(), a_rPoint, a_rImpulse, 0))
		{
			return true;
		}
		else if (CheckAxis(destrInstance, pObject->getCenterBrick(), a_rPoint, a_rImpulse, 1))
		{
			return true;
		}
		else
		{
			return false;
		}
			 

	}



	void CDestructionManager::SetNewObject(SBrickDestruction* a_pBrick, TheBrick::CGameObject* a_pNewGameObject, int a_Tick)
	{
		a_pBrick->tick = a_Tick;
		a_pBrick->lastBroken = m_LastBrocken;
		
		a_pBrick->brick->GetGameObject()->RemoveBrickInstance(*a_pBrick->brick);
		a_pNewGameObject->AddBrickInstance(a_pBrick->brick, *sba_Space->World);
		
		for (int i = 0; i < a_pBrick->numConnections; ++i)
		{
			if (a_pBrick->connections[i].other->tick != a_Tick)
			{
				SetNewObject(a_pBrick, a_pNewGameObject, a_Tick);
			}
		}
	}
 
	void CDestructionManager::Update()
	{
		for (auto& impulse : m_inpulses)
		{
			TheBrick::CBrickInstance* brick = impulse.brick->brick;

			ong::BodyDescription descr;
			descr.transform = brick->GetGameObject()->GetTransform();
			descr.linearMomentum = brick->GetGameObject()->m_pBody->getLinearMomentum();
			descr.angularMomentum = brick->GetGameObject()->m_pBody->getAngularMomentum();
			descr.type = ong::BodyType::Dynamic;

			CDestructibleObject* oldObject = dynamic_cast<CDestructibleObject*>(brick->GetGameObject());
			assert(oldObject);

			CDestructibleObject* newObject = new CDestructibleObject(*sba_Space->World, &descr);

			SetNewObject(impulse.brick, newObject, m_Tick++);

			SBrickDestruction* pNewCenterBrick = calcCenterBrick(newObject->m_pBody->getLocalCenter(), newObject->m_pBricks.data(), newObject->m_pBricks.size());
			newObject->SetCenterBrick(pNewCenterBrick);

			SBrickDestruction* pOldCenterBrick = calcCenterBrick(oldObject->m_pBody->getLocalCenter(), oldObject->m_pBricks.data(), oldObject->m_pBricks.size());
			oldObject->SetCenterBrick(pOldCenterBrick);
		}

		m_inpulses.clear();

			
	}


}