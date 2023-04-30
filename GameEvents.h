#pragma once

void OnPedCreated(CPed* ped)
{
	if (ped != GameUtils::me)
	{
		auto result = find(GameUtils::peds.begin(), GameUtils::peds.end(), ped);
		if (result == GameUtils::peds.end())
		{
			GameUtils::peds.push_back(ped);
		}
	}
}

void OnPedDestroyed(CPed* ped)
{
	if (GameUtils::peds.size())
		for (size_t i = 0; i < GameUtils::peds.size(); i++)
		{
			if (GameUtils::peds.at(i) == ped)
			{
				GameUtils::peds.erase(GameUtils::peds.begin() + i);
				break;
			}
		}
}

void OnVehicleCreated(CVehicle* vehicle)
{
	GameUtils::cars.push_back(vehicle);
}

void OnVehicleDestroyed(CVehicle* vehicle)
{
	if (GameUtils::cars.size())
	{
		for (size_t i = 0; i < GameUtils::cars.size(); i++)
		{
			if (GameUtils::cars[i] == vehicle)
			{
				GameUtils::cars.erase(GameUtils::cars.begin() + i);
				break;
			}
		}
	}
}