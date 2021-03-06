#ifndef _DESTRUCTION_MANAGER_H_
#define _DESTRUCTION_MANAGER_H_

#include "TheBrick\BrickInstance.h"
#include "Onager\Allocator.h"
#include "Onager\myMath.h"
#include "Onager\defines.h"
#include <vector>


//namespace TheBrick
//{
//	class CBrickInstance;
//}

namespace ong
{
	class Collider;
}

namespace sba
{

	class CDestructibleObject;
	struct SBrickDestruction;
	struct SConnection;

	struct SJointData
	{
		float fulcrum;
		float posY;
		float baseCapacity;
	};

	struct SJoint
	{
		SJointData* data;
		SConnection* connection;
		SJoint* twin;
		float capacity;
		float flow;
	};


	struct SConnection
	{
		SBrickDestruction* brick;
		SBrickDestruction* other;
		int dir;
		SJoint* joints[2][2];
	};

	struct SBrickDestruction
	{
		static const int MAX_CONNECTIONS = 42;

		TheBrick::CBrickInstance* brick;

		unsigned int numConnections = 0;
		SConnection connections[MAX_CONNECTIONS];

		int tick = -1;
	};


	class CDestructionManager
	{
	private: 
		friend bool setUpJoints(ong::Collider*, void*);
		friend bool setUpBlocking(ong::Collider*, void*);

		static const float NUB_STRENGTH;
	
		struct SImpulse
		{
			SBrickDestruction* brick;
		};

		bool BreakGraph(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, float a_MaxFlow, int a_Axis, int a_Tick);
		bool FindAugmentedPath(SBrickDestruction* a_pBrick, SBrickDestruction* a_pSink, int a_Axis, float* a_pMinCapacity, std::vector<SJoint*>& a_rAugmentedPath, int a_Tick);
		float MaxFlow(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, int a_Axis);
		bool CheckAxis(SBrickDestruction* a_pBrick, SBrickDestruction* a_pCenterBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse, int a_Axis);
		void SetNewObject(SBrickDestruction* a_pBrick, TheBrick::CGameObject* a_pGameObject, int a_Tick);
		void SetNewObject(SBrickDestruction* a_pBrick, TheBrick::CGameObject* a_pGameObject);
		void Destroy(SBrickDestruction* a_pBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse, int a_Axis, int a_Tick);

		ong::Allocator<SJoint>* m_Joints;
		ong::Allocator<SJointData>* m_JointData;
		ong::Allocator<SBrickDestruction>* m_BrickDestruction;

		ong::uint32 m_Tick;
		ong::int32 m_DestructionCounter;
		std::vector<SImpulse> m_inpulses;

		std::vector<SBrickDestruction*> m_Selection;
		std::vector<SJoint*> m_Front;
		
	public:
		static const int DESTRUCTION_COOLDOWN = 50;

		
		CDestructionManager();

		void BuildDestruction(CDestructibleObject* a_Object, TheBrick::CBrickInstance** a_Bricks, int a_NumBricks);
		bool AddImpulse(TheBrick::CBrickInstance* a_pBrick, const ong::vec3& a_rPoint, const ong::vec3& a_rImpulse);

		void Update();
		void Reset();

		int GetDestructionCounter() const;

	};


}

#endif