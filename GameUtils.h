#pragma once

#ifndef SP
constexpr float MIN_DISTANCE = 30.0f;
constexpr float headingOffset = 0.062416f;
#else
constexpr float MIN_DISTANCE = 0.1f;
constexpr float headingOffset = 0.072416f;
#endif // !SP

constexpr uint DEAD = 54;
constexpr uint DEAD2 = 55;

constexpr float DEFAULT_FOV = 70.0f;
constexpr float FOV_ADJUSTMENT = 0.008726646f;
constexpr float DEFAULT_RATIO = 1.333333f;
constexpr float MAX_DISTANCE = 350.0f;

constexpr double M_PI = 3.14159265358979323846264338327950288;

bool SniperMode = true;

namespace GameUtils
{
	struct Vector2D
	{
		float x, y;
	};

	CPlayerPed* me;
	std::vector<CPed*> peds;
	std::vector<int> avoidedSkins;
	std::vector<CVehicle*> cars;

	bool IsInFirstPerson()
	{
		return me->m_dwAction == 12;
	}

	void ApplyImmunity(bool includeVehicle = false)
	{
		if (includeVehicle && me->m_bInVehicle)
		{
			me->m_pVehicle->m_nFlags.bBulletProof = true;
			me->m_pVehicle->m_nFlags.bCollisionProof = true;
			me->m_pVehicle->m_nFlags.bExplosionProof = true;
			me->m_pVehicle->m_nFlags.bFireProof = true;
			me->m_pVehicle->m_nFlags.bMeleeProof = true;
		}

		me->m_nFlags.bBulletProof = true;
		me->m_nFlags.bCollisionProof = true;
		me->m_nFlags.bExplosionProof = true;
		me->m_nFlags.bFireProof = true;
		me->m_nFlags.bMeleeProof = true;
	}

	void RemoveImmunity(bool includeVehicle = false)
	{
		if (includeVehicle && me->m_bInVehicle)
		{
			me->m_pVehicle->m_nFlags.bBulletProof = false;
			me->m_pVehicle->m_nFlags.bCollisionProof = false;
			me->m_pVehicle->m_nFlags.bExplosionProof = false;
			me->m_pVehicle->m_nFlags.bFireProof = false;
			me->m_pVehicle->m_nFlags.bMeleeProof = false;
		}

		me->m_nFlags.bBulletProof = false;
		me->m_nFlags.bCollisionProof = false;
		me->m_nFlags.bExplosionProof = false;
		me->m_nFlags.bFireProof = false;
		me->m_nFlags.bMeleeProof = false;
	}

	Vector2D GetAnglesBetweenPeds(CPed* source, CPed* destination)
	{
		RwV3d head_pos;
		destination->m_pedIK.GetComponentPosition(&head_pos, 2);

		CVector sourcePos, destinationPos = CVector(head_pos.x, head_pos.y, head_pos.z);
		Vector2D adjustedScreenPos = { 0, 0 };

		float az, ax, fz, fx;

		if (!IsInFirstPerson())
		{
			sourcePos = CVector(source->GetPosition().x, source->GetPosition().y, source->GetPosition().z + 1);
			
			float screenMultiplier = tan(CDraw::ms_fFOV * FOV_ADJUSTMENT);
			fz = (float)M_PI - atan2(1.0f, screenMultiplier * (1.0f / CDraw::ms_fAspectRatio)) - 0.3675f;
			fx = 0.0025f - atan2(1.0f, screenMultiplier * headingOffset);
		}
		else
		{
			RwV3d localHeadPos;
			me->m_pedIK.GetComponentPosition(&localHeadPos, 2);
			sourcePos = CVector(source->GetPosition().x, source->GetPosition().y, localHeadPos.z);
			fx = fz = (float)M_PI / 2.0f;
			fx -= 0.001f;
		}

		CVector deltaPosition = sourcePos - destinationPos;
		float dist = sqrt(deltaPosition.x * deltaPosition.x + deltaPosition.y * deltaPosition.y);

		az = atan2f(dist, deltaPosition.z);
		ax = atan2f(deltaPosition.y, -deltaPosition.x) + (float)M_PI / 2.0f;

		adjustedScreenPos.y = (az - fz);
		adjustedScreenPos.x = -(ax - fx);

		return adjustedScreenPos;
	}

	float Get3DDistance(CVector first, CVector second)
	{
		return sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y) + (first.z - second.z) * (first.z - second.z));
	}

	int GetNearestCar()
	{
		float min_dist = (float)INT_MAX;
		int index = -1;
		for (size_t i = 0; i < cars.size(); i++)
		{
			float temp_dist = Get3DDistance(me->GetPosition(), cars[i]->GetPosition());
			if (temp_dist < min_dist)
			{
				min_dist = temp_dist;
				index = i;
			}
		}
		return index;
	}

	bool IsDead(CPed* ped)
	{
#ifdef SP
		return ped->m_fHealth < 1.5f ? true : false;
#else
		if (ped->m_dwAction == DEAD || ped->m_dwAction == DEAD2) return true;
		if (ped->m_nPedFlags.bIsPedDieAnimPlaying) return true;
		return false;
#endif
	}

	bool CheckSkin(int skin)
	{
		auto result = find(avoidedSkins.begin(), avoidedSkins.end(), skin);
		if (result == avoidedSkins.end()) return true;
		return false;
	}

	int getNearestPed(bool includeVehicle)
	{
		float min_dist = (float)INT_MAX;
		int index = -1;
		for (size_t i = 0; i < peds.size(); i++)
		{
			if (!IsDead(peds.at(i))
				&& (!SniperMode ? CWorld::GetIsLineOfSightClear(me->GetPosition(), peds.at(i)->GetPosition(), true, false, false, true, true, false, false) : true) //try not to aim at peds behind walls
				&& CheckSkin(peds.at(i)->m_nModelIndex))
			{
				if (!includeVehicle)
				{
					if (peds.at(i)->m_bInVehicle)
					{
						continue;
					}
				}
				float temp_dist = Get3DDistance(me->GetPosition(), peds.at(i)->GetPosition());
				if (temp_dist < min_dist && temp_dist > MIN_DISTANCE && temp_dist < MAX_DISTANCE)
				{
					min_dist = temp_dist;
					index = i;
				}
			}
		}
		return index;
	}

	void Undrunk()
	{
		me->m_nDrunkCountdown = 1;
		me->m_nFadeDrunkenness = 127;
		me->m_nDrunkenness = 0;
		me->m_bDrunkVisualsWearOff = true;
	}

	void AimToNearestPed(bool includeVehicle = true)
	{
		int nearest;
		nearest = getNearestPed(includeVehicle);
		if (nearest != -1)
		{
			Vector2D angles = GetAnglesBetweenPeds(me, peds[nearest]);
			*(float*)MemoryAddresses::x_axis = angles.x;
			*(float*)MemoryAddresses::y_axis = angles.y;
		}
	}

	void FlyThroughStuff() {
		me->m_nFlags.bUseCollision = false; //no collision, also effective against bullets

		float x = 0.0f, y = 0.0f, vx = 0.0f, vy = 0.0f, vz = 0.0f;
		float px = 0.0f, py = 0.0f, pz = 0.0f;

		if (GetAsyncKeyState('W'))//W - forward in the direction of the mouse heading
		{
			x += sin(-me->m_fRotationCur);
			y += cos(-me->m_fRotationCur);

			px += x;
			py += y;

			vx = x + 0.1f;
			vy = y + 0.1f;
		}

		if (GetAsyncKeyState('S'))//S - go down
		{
			pz -= 0.2f;
			vz = -0.2f;
		}

		if (GetAsyncKeyState(0x20))//space - go up
		{
			pz += 0.2f;
			vz = 0.2f;
		}

		if (GetAsyncKeyState('Q'))//Q - slow down
		{
			float div = 5.0f;

			vx /= div;
			vy /= div;
			vz /= div;

			px /= div;
			py /= div;
			pz /= div;
		}

		if (GetAsyncKeyState('E'))//E - speed up
		{
			float mult = 4.0f;

			vx *= mult;
			vy *= mult;
			vz *= mult;

			px *= mult;
			py *= mult;
			pz *= mult;
		}

		me->m_placement.pos.x += px;
		me->m_placement.pos.y += py;
		me->m_placement.pos.z += pz;
		if (vz == 0.0) vz = 0.011f;

		me->m_vecMoveSpeed = CVector(vx, vy, vz);
	}
}
