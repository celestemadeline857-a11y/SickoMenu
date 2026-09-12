#include "pch-il2cpp.h"
#include "game.h"
#include "SignatureScan.hpp"
#include "state.hpp"
#include "utility.h"

#include <cmath>

namespace Game {
	AmongUsClient** pAmongUsClient = nullptr;
	GameData** pGameData = nullptr;
	List_1_PlayerControl_** pAllPlayerControls = nullptr;
	PlayerControl** pLocalPlayer = nullptr;
	ShipStatus** pShipStatus = nullptr;
	LobbyBehaviour** pLobbyBehaviour = nullptr;
	DestroyableSingleton<app::RoleManager*> RoleManager { "Assembly-CSharp, RoleManager" };
	DestroyableSingleton<app::HudManager*> HudManager { "Assembly-CSharp, HudManager" };
	DestroyableSingleton<app::AccountManager*> AccountManager { "Assembly-CSharp, AccountManager" };

	namespace {
		constexpr float MinKillDistance = 1.0f;
		constexpr float MaxDistance = 5.0f;

		Vent* FindClosestExposeVent(PlayerControl* player, float minDistance, float maxDistance)
		{
			if (!player || !pShipStatus || !*pShipStatus) return nullptr;

			Vent* closestVent = nullptr;
			float closestDistance = maxDistance;
			for (auto vent : il2cpp::Array((*pShipStatus)->fields._AllVents_k__BackingField)) {
				if (!vent) continue;
				const auto playerPosition = PlayerControl_GetTruePosition(player, nullptr);
				const auto ventTransform = Component_get_transform((Component_1*)vent, nullptr);
				if (!ventTransform) continue;
				const auto ventPosition3 = Transform_get_position(ventTransform, nullptr);
				const Vector2 ventPosition{ ventPosition3.x, ventPosition3.y };
				const float distance = std::hypot(playerPosition.x - ventPosition.x, playerPosition.y - ventPosition.y);
				if (distance < minDistance || distance > maxDistance || distance >= closestDistance) continue;
				closestVent = vent;
				closestDistance = distance;
			}
			return closestVent;
		}

		void ExposePlayers(PlayerControl* source, PlayerControl* excludedPlayer, float minDistance, float maxDistance)
		{
			if (!State.AutoExposeImpostors || !IsInGame() || !pShipStatus || !*pShipStatus) return;

			Vent* vent = FindClosestExposeVent(source, minDistance, maxDistance);
			if (!vent) return;
			for (auto player : GetAllPlayerControl()) {
				auto data = GetPlayerData(player);
				if (!player || player == source || player == excludedPlayer || !data || data->fields.IsDead || PlayerIsImpostor(data) || !player->fields.MyPhysics) continue;

				if (IsHost())
					PlayerPhysics_RpcBootFromVent(player->fields.MyPhysics, vent->fields.Id, nullptr);
				else
					SendBootVentNonHost(player, vent->fields.Id);
			}
		}
	}

	void AutoExposeOnMurder(PlayerControl* murderer, PlayerControl* target, MurderResultFlags__Enum resultFlags)
	{
		if (!State.AutoExposeImpostors || (static_cast<int32_t>(resultFlags) & static_cast<int32_t>(MurderResultFlags__Enum::Succeeded)) == 0 ||
			!murderer || !target || murderer->fields.shapeshiftTargetPlayerId != -1) return;
		ExposePlayers(murderer, target, MinKillDistance, MaxDistance);
	}

	void AutoExposeOnShapeshift(PlayerControl* shapeshifter, PlayerControl* target)
	{
		if (!State.AutoExposeImpostors || !shapeshifter || !target || shapeshifter == target) return;
		ExposePlayers(shapeshifter, nullptr, MinKillDistance, MaxDistance);
	}

	void AutoExposeOnPhantom(PlayerControl* phantom)
	{
		if (!State.AutoExposeImpostors || !phantom || phantom->fields.shapeshiftTargetPlayerId != -1) return;
		ExposePlayers(phantom, nullptr, 0.0f, MaxDistance);
	}

	//STEAMUSERSTATS_SETACHIEVEMENT* SteamUserStats_SetAchievement = nullptr;
	//STEAMUSERSTATS_STORESTATS* SteamUserStats_StoreStats = nullptr;

	void scanGameFunctions()
	{
		//SteamUserStats_SetAchievement = SignatureScan<STEAMUSERSTATS_SETACHIEVEMENT*>("E8 ? ? ? ? 6A 00 E8 ? ? ? ? 83 C4 0C 5D C3 A1 ? ? ? ? F6 80 ? ? ? ? ? 74 0F 83 78 74 00 75 09 50 E8 ? ? ? ? 83 C4 04 6A 00 FF 35 ? ? ? ? E8 ? ? ? ? 83 C4 08 5D C3 CC", GetModuleHandleA("GameAssembly.dll")).ResolveCall();
		//SteamUserStats_StoreStats = SignatureScan<STEAMUSERSTATS_STORESTATS*>("E8 ? ? ? ? 83 C4 0C 5D C3 A1 ? ? ? ? F6 80 ? ? ? ? ? 74 0F 83 78 74 00 75 09 50 E8 ? ? ? ? 83 C4 04 6A 00 FF 35 ? ? ? ? E8 ? ? ? ? 83 C4 08 5D C3 CC", GetModuleHandleA("GameAssembly.dll")).ResolveCall();
	}
}