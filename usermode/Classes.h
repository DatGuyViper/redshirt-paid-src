#pragma once
#include "sdk.h"
#include <thread>
#include <unordered_map>
#include "D3DX/d3dx9math.h"

uintptr_t va_text = 0;

#define PI 3.14159265358979323846f

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

namespace Features
{
	inline int rFov = 70;
	inline bool rWireframe;
	inline bool rFPS;
	inline float rHumanization;
	inline bool rGunConfigs;
	inline bool rChams;
	inline int rAimkey = VK_RBUTTON;
	inline int rTrigkey = VK_RBUTTON;
	inline bool rVisibleCheck;
	inline bool rDownedCheck;
	inline bool rTargetingOnly;
	inline bool rBotCheck;
	inline bool rAimline;
	inline bool rLockTarget;
	inline float rSmooth = 10;
	inline float rTriggerbotDelay = 0;
	inline bool rAimbot;
	inline bool rPrediction;
	inline bool rFovCircle;
	inline bool rTriggerbot;
	inline float rMaxDistance = 300.f;
	inline bool LevelActorCaching = 1;
	inline bool rFullbox;
	inline bool rSnaplines;
	inline bool rDistance;
	inline bool rKills;
	inline bool rLevel;
	inline bool rWeaponname;
	inline bool rPlatformname;
	inline bool rRanked;
	inline bool rUsername;
	inline bool rCornerbox;
	inline bool rSkeleton;
	inline bool rViewAngle;
	inline bool rFovRadar;
	inline int rFovSize = 200;
	inline int rADSFovSize = 200;
	inline int rFovDrawSize;




	inline ImVec4 AimlineColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 FovColor = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 AccentColor = ImVec4(1.f, 1.f, 1.f, 0.6f);

	inline ImVec4 FovFillColor = ImVec4(1.f, 1.f, 1.f, 0.f);

	inline ImVec4 BoxColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 FillColorVis = ImVec4(1.f, 1.f, 1.f, 0.2f);
	inline ImVec4 DistanceColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 UsernameColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 PlatformColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 KillsColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 LevelsColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 WeaponColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 SkeletonColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 SnaplineColorVis = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 RadarColorVis = ImVec4(1.f, 0.f, 0.f, 1.f);
	inline ImVec4 FovArrowsColorVis = ImVec4(1.f, 0.f, 0.f, 1.f);

	inline ImVec4 BoxColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 FillColorHidden = ImVec4(1.f, 1.f, 1.f, 0.2f);
	inline ImVec4 DistanceColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 UsernameColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 PlatformColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 KillsColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 LevelsColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 WeaponColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 SkeletonColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 SnaplineColorHidden = ImVec4(1.f, 1.f, 1.f, 1.f);
	inline ImVec4 RadarColorHidden = ImVec4(0.f, 1.f, 0.f, 1.f);
	inline ImVec4 FovArrowsColorHidden = ImVec4(0.f, 1.f, 0.f, 1.f);
}

namespace Offsets {
	uint64_t
		UWorld = 0x11ECD148,
		GNames = 0x120A7A40,
		GameState = 0x160,
		PlayerArray = 0x2A8,
		GameInstance = 0x1D8,
		LocalPlayers = 0x38,
		PlayerController = 0x30,
		LocalPawn = 0x338,
		PlayerState = 0x2B0,
		RootComponent = 0x198,
		PersistentLevel = 0x30,
		AActors = 0xA0, 
		ActorCount = 0xA8,
		ReviveFromDBNOTime = 0x4C38,
		Mesh = 0x310,
		TeamIndex = 0x11d1,
		Platform = 0x3f0,
		PawnPrivate = 0x308,
		RelativeLocation = 0x120,
		PrimaryPickupItemEntry = 0x350,
		ItemDefinition = 0x18,
		Rarity = 0x9A,
		ItemName = 0x40,
		Levels = 0x178,
		WeaponData = 0x510,
		AmmoCount = 0xef4,
		bIsTargeting = 0x581,
		ComponentVelocity = 0x168,
		TargetedFortPawn = 0x1898,
		CurrentWeapon = 0x9d8,
		CurrentWeaponList = 0x9e8,
		BoneArray = 0x570,
		BoneCache = 0x5B8,
		LastSubmitTime = 0x2E0,
		LastRenderTime = 0x2E8,
		ComponentToWorld = 0x1C0,
		CameraManager = 0x348,
		LastFiredLoc = 0x57e8, 
		LastFiredDir = 0x5800, 
		KillScore = 0x11e4, 
		SeasonLevelUIDisplay = 0x11e8; 
}
#define FortPTR uintptr_t

#define DECLARE_MEMBER(type, name, offset) \
    type name() { return read<type>(reinterpret_cast<uintptr_t>(this) + offset); }

#define APPLY_MEMBER(type, name, offset) \
    void name(type val) { write<type>(reinterpret_cast<uintptr_t>(this) + offset, val); }

DWORD_PTR Uworld_Cam;

class UObject {
public:
	FortPTR GetAddress() const {
		return reinterpret_cast<FortPTR>(this);
	}

	DECLARE_MEMBER(int, GetObjectID, 0x18)
};

class USceneComponent : public UObject {
public:
	DECLARE_MEMBER(Vector3, RelativeLocation, Offsets::RelativeLocation)
	DECLARE_MEMBER(Vector3, GetComponentVelocity, Offsets::ComponentVelocity)
	APPLY_MEMBER(Vector3, SetActorLocation, Offsets::RelativeLocation)
};

class AActor : public UObject {
public:
	DECLARE_MEMBER(USceneComponent*, RootComponent, Offsets::RootComponent)

	Vector3 GetActorLocation(bool cached = true) {
		static std::unordered_map<int, Vector3> CachedLocations;
		if (cached) {
			auto it = CachedLocations.find(GetObjectID());
			if (it != CachedLocations.end()) {
				return it->second;
			}
		}
		Vector3 location = RootComponent()->RelativeLocation();
		CachedLocations[GetObjectID()] = location;
		return location;
	}
};

class USkeletalMeshComponent : public AActor {
public:
	Vector3 GetSocketLocation(int bone_id) {
		int cacheIdx = read<int>(GetAddress() + Offsets::BoneCache);
		auto boneTransform = read<FTransform>(read<uintptr_t>(GetAddress() + 0x10 * cacheIdx + Offsets::BoneArray) + 0x60 * bone_id);
		FTransform componentToWorld = read<FTransform>(GetAddress() + 0x1c0);
		D3DMATRIX matrix = MatrixMultiplication(boneTransform.ToMatrixWithScale(), componentToWorld.ToMatrixWithScale());
		return Vector3(matrix._41, matrix._42, matrix._43);
	}

	bool WasRecentlyRendered(float tolerance) {
		float lastSubmitTime = read<float>(GetAddress() + Offsets::LastSubmitTime);
		float lastRenderTime = read<float>(GetAddress() + Offsets::LastRenderTime);
		return lastRenderTime + tolerance >= lastSubmitTime;
	}
};

struct CameraInfo {
	Vector3 location;
	Vector3 rotation;
	float fov;
};

CameraInfo camera;
CameraInfo Copy_CameraInfo;
FortPTR Copy_PlayerController_Camera;

CameraInfo GetCameraInfo() {
	if (Copy_PlayerController_Camera) {
		auto locationPointer = read<uintptr_t>(Uworld_Cam + 0x110);
		auto rotationPointer = read<uintptr_t>(Uworld_Cam + 0x120);

		struct RotationInfo {
			double pitch;
			char pad_0008[24];
			double yaw;
			char pad_0028[424];
			double roll;
		};

		RotationInfo rotInfo = read<RotationInfo>(rotationPointer);

		camera.location = read<Vector3>(locationPointer);
		camera.rotation.x = asin(rotInfo.roll) * (180.0 / PI);
		camera.rotation.y = atan2(-rotInfo.pitch, rotInfo.yaw) * (180.0 / PI);
		camera.fov = read<float>(Copy_PlayerController_Camera + 0x394) * 90.f;

		return camera;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	return camera;
}

struct TArray {
	FortPTR Array = 0;
	uint32_t Count = 0;
	uint32_t MaxCount = 0;

	FortPTR Get(uint32_t Index) const {
		if (Index >= Count) {
			return 0;
		}
		return read<FortPTR>(Array + (Index * sizeof(FortPTR)));
	}

	uint32_t size() const {
		return Count;
	}

	FortPTR operator[](uint32_t Index) const {
		return Get(Index);
	}

	bool isValid() const {
		return Array && Count <= MaxCount && MaxCount <= 1000000;
	}

	FortPTR getAddress() const {
		return Array;
	}
};

class ULevel : public UObject {
public:
	DECLARE_MEMBER(uint32_t, ActorCount, Offsets::ActorCount)
	DECLARE_MEMBER(FortPTR, AActors, Offsets::AActors)
};

class AFortPlayerState : public AActor {
public:
	DECLARE_MEMBER(int, TeamIndex, Offsets::TeamIndex)
	DECLARE_MEMBER(AActor*, PawnPrivate, Offsets::PawnPrivate)
	DECLARE_MEMBER(DWORD_PTR, Platform, Offsets::Platform)
};

class UFortItemDefinition : public AActor {
public:
	DECLARE_MEMBER(int, RarityInt, Offsets::Rarity)

	std::string GetItemName() const {
		uint64_t itemNameAddr = read<uint64_t>(GetAddress() + Offsets::ItemName);
		if (!itemNameAddr) return "";

		uint64_t dataAddr = read<uint64_t>(itemNameAddr + 0x28);
		int length = read<int>(itemNameAddr + 0x30);
		if (length <= 0 || length >= 50) return "";

		std::wstring wideBuffer(length, 0);
		handler::read_physical(reinterpret_cast<void*>(dataAddr), &wideBuffer[0], length * sizeof(wchar_t));
		return std::string(wideBuffer.begin(), wideBuffer.end());
	}
};

class AFortWeapon : public AActor {
public:
	float GetProjectileSpeed() const {
		return read<float>(GetAddress() + 0x1d08);
	}

	float GetGravityScale() const {
		return read<float>(GetAddress() + 0x1d0c);
	}

	DECLARE_MEMBER(UFortItemDefinition*, WeaponData, Offsets::WeaponData)
	DECLARE_MEMBER(int, AmmoCount, Offsets::AmmoCount)
	DECLARE_MEMBER(bool, IsTargeting, Offsets::bIsTargeting)
};

class AFortPlayerPawn : public AActor {
public:
	DECLARE_MEMBER(float, ReviveFromDBNOTime, Offsets::ReviveFromDBNOTime)
	DECLARE_MEMBER(AFortPlayerState*, PlayerState, Offsets::PlayerState)
	DECLARE_MEMBER(USkeletalMeshComponent*, Mesh, Offsets::Mesh)
	DECLARE_MEMBER(AFortWeapon*, CurrentWeapon, Offsets::CurrentWeapon)
};

class APlayerController : public AActor {
public:
	D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0.f, 0.f, 0.f)) const {
		float radPitch = rot.x * PI / 180.f;
		float radYaw = rot.y * PI / 180.f;
		float radRoll = rot.z * PI / 180.f;

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix = {};
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	Vector2 ProjectWorldLocationToScreen(Vector3 worldLocation) const {
		CameraInfo viewInfo = GetCameraInfo();
		D3DMATRIX tempMatrix = Matrix(viewInfo.rotation);
		Vector3 axisX(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		Vector3 axisY(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		Vector3 axisZ(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		Vector3 delta = worldLocation - viewInfo.location;
		Vector3 transformed(delta.Dot(axisY), delta.Dot(axisZ), delta.Dot(axisX));

		if (transformed.z < 1.f) transformed.z = 1.f;

		return Vector2(
			(screenWidth / 2.0f) + transformed.x * ((screenWidth / 2.0f) / tanf(viewInfo.fov * PI / 360.f)) / transformed.z,
			(screenHeight / 2.0f) - transformed.y * ((screenWidth / 2.0f) / tanf(viewInfo.fov * PI / 360.f)) / transformed.z
		);
	}

	DECLARE_MEMBER(FortPTR, AcknowledgedPawn, Offsets::LocalPawn)
	DECLARE_MEMBER(AFortPlayerPawn*, TargetedFortPawn, Offsets::TargetedFortPawn)
};

class ULocalPlayer : public AFortPlayerPawn {
public:
	DECLARE_MEMBER(APlayerController*, PlayerController, Offsets::PlayerController)
};

class UGameInstance : public AActor {
public:
	DECLARE_MEMBER(DWORD_PTR, LocalPlayers, Offsets::LocalPlayers)
};

class AGameStateBase : public UObject {
public:
	DECLARE_MEMBER(TArray, PlayerArray, Offsets::PlayerArray)
};

class UWorld : public AActor {
public:
	DECLARE_MEMBER(UGameInstance*, OwningGameInstance, Offsets::GameInstance)
	DECLARE_MEMBER(ULevel*, PersistentLevel, Offsets::PersistentLevel)
	DECLARE_MEMBER(AGameStateBase*, GameState, Offsets::GameState)
};

namespace Cached
{
	float ProjectileSpeed;
	float ProjectileGravity;
	APlayerController* PlayerController;
	AFortPlayerState* PlayerState;
	AFortPlayerState* LocalPlayerState;
	AFortPlayerPawn* LocalPawn;
	USceneComponent* LocalRootComponent;
	ULocalPlayer* LocalPlayer;
	USkeletalMeshComponent* Mesh;
	ULevel* PersistentLevel;
	UWorld* World;
	AGameStateBase* GameState;
	AFortWeapon* CurrentWeapon;
	uintptr_t CameraManager;

	float closestDistance = FLT_MAX;
	AFortPlayerPawn* closestPawn = NULL;


	Vector3 localactorpos;
}

double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

namespace lazy {
	std::string ReadWideString(uint64_t address, int length) {
		if (length <= 0 || length > 50) return "";

		std::wstring buffer(length, L'\0');
		handler::read_physical((void*)address, (PVOID)buffer.data(), length * sizeof(wchar_t));
		return std::string(buffer.begin(), buffer.end());
	}

	std::string GetWeaponNameFromData(uint64_t weaponData) {
		uint64_t itemNameAddr = read<uint64_t>(weaponData + Offsets::ItemName);
		if (!itemNameAddr) return "";

		uint64_t fData = read<uint64_t>(itemNameAddr + 0x28);
		int length = read<int>(itemNameAddr + 0x30);
		return ReadWideString(fData, length);
	}

	std::string GetWeaponName(uint64_t playerAddress) {
		uint64_t currentWeapon = read<uint64_t>(playerAddress + Offsets::CurrentWeapon);
		uint64_t weaponData = read<uint64_t>(currentWeapon + Offsets::WeaponData);
		return GetWeaponNameFromData(weaponData);
	}

	std::string GetPlatformName(uintptr_t playerState) {
		uintptr_t platformAddr = read<uintptr_t>(playerState + Offsets::Platform);
		wchar_t platformBuffer[64];
		handler::read_physical((void*)platformAddr, platformBuffer, sizeof(platformBuffer));

		std::string platformStr(platformBuffer, platformBuffer + wcslen(platformBuffer));

		if (platformStr == "XBL") return "Xbox";
		if (platformStr == "PSN" || platformStr == "PS5") return "PlayStation";
		if (platformStr == "XSX") return "Xbox";
		if (platformStr == "SWT") return "Nintendo";
		if (platformStr == "WIN") return "PC";
		return "AI";
	}

	std::string GetPlayerName(uintptr_t playerState) {
		uintptr_t nameAddr = read<uintptr_t>(playerState + 0xA68);
		int length = read<int>(nameAddr + 0x10);
		uint64_t fTextAddr = read<uint64_t>(nameAddr + 0x8);
		return ReadWideString(fTextAddr, length);
	}

	std::string GetRank(int tier) {
		switch (tier) {
		case 0:  return "Unranked";
		case 1:  return "Bronze 2";
		case 2:  return "Bronze 3";
		case 3:  return "Silver 1";
		case 4:  return "Silver 2";
		case 5:  return "Silver 3";
		case 6:  return "Gold 1";
		case 7:  return "Gold 2";
		case 8:  return "Gold 3";
		case 9:  return "Platinum 1";
		case 10: return "Platinum 2";
		case 11: return "Platinum 3";
		case 12: return "Diamond 1";
		case 13: return "Diamond 2";
		case 14: return "Diamond 3";
		case 15: return "Elite";
		case 16: return "Champion";
		case 17: return "Unreal";
		default: return "Unranked";
		}
	}

	ImVec4 GetRankColor(int tier) {
		switch (tier) {
		case 0:  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
		case 1:
		case 2:  return ImVec4(0.902f, 0.580f, 0.227f, 1.0f); // Bronze
		case 3:
		case 4:
		case 5:  return ImVec4(0.843f, 0.843f, 0.843f, 1.0f); // Silver
		case 6:
		case 7:
		case 8:  return ImVec4(1.0f, 0.871f, 0.0f, 1.0f); // Gold
		case 9:
		case 10:
		case 11: return ImVec4(0.0f, 0.7f, 0.7f, 1.0f);  // Platinum
		case 12:
		case 13:
		case 14: return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f); // Diamond
		case 15: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);   // Elite
		case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);   // Champion
		case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);   // Unreal
		default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
		}
	}
}

namespace draw {

	void DrawFilledRect(int x, int y, int w, int h, ImU32 color, float alpha) {
		ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ImColor(color).Value.x, ImColor(color).Value.y, ImColor(color).Value.z, alpha));
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), fillColor);
	}

	void addtofovradar(Vector3 WorldLocation, float fDistance, bool visible, ImVec2 drawPos, ImColor color) {
		Vector3 vAngle = camera.rotation;
		float fYaw = vAngle.y * PI / 180.0f;
		float dx = WorldLocation.x - camera.location.x;
		float dy = WorldLocation.y - camera.location.y;
		float fsin_yaw = sinf(fYaw);
		float fminus_cos_yaw = -cosf(fYaw);

		float x = -(dy * fminus_cos_yaw + dx * fsin_yaw);
		float y = dx * fminus_cos_yaw - dy * fsin_yaw;

		float fovRadius = Features::rFovDrawSize;
		float angle = atan2f(y, x);
		float triangleSize = 12.0f;
		float widthFactor = 8.0f;
		float outlineThickness = 2.0f;

		ImVec2 triangleCenter = ImVec2(drawPos.x + cosf(angle) * (fovRadius + triangleSize),
			drawPos.y + sinf(angle) * (fovRadius + triangleSize));

		ImVec2 point1 = ImVec2(triangleCenter.x + cosf(angle) * triangleSize,
			triangleCenter.y + sinf(angle) * triangleSize);
		ImVec2 point2 = ImVec2(triangleCenter.x + cosf(angle + widthFactor) * triangleSize,
			triangleCenter.y + sinf(angle + widthFactor) * triangleSize);
		ImVec2 point3 = ImVec2(triangleCenter.x + cosf(angle - widthFactor) * triangleSize,
			triangleCenter.y + sinf(angle - widthFactor) * triangleSize);

		ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(visible ? color : ImColor(color.Value.x, color.Value.y, color.Value.z, 0.5f));
		ImU32 outlineColor = ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 1.0));

		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		drawList->AddTriangleFilled(point1, point2, point3, fillColor);
		drawList->AddTriangle(point1, point2, point3, outlineColor, outlineThickness);
	}

	void AddLine(Vector2 start, Vector2 end, ImU32 color, float thickness, ImU32 shadowColor, float shadowOffset) {
		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		drawList->AddLine(ImVec2(start.x + shadowOffset, start.y + shadowOffset), ImVec2(end.x + shadowOffset, end.y + shadowOffset), shadowColor, thickness);
		drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color, thickness);
	}

	void DrawCornerBox(int X, int Y, int W, int H, const ImColor color, int thickness) {
		float lineW = W / 3.0f;
		float lineH = H / 3.0f;
		ImU32 shadowColor = ImColor(0, 0, 0, 255);
		float shadowOffset = 1.0f;

		AddLine(Vector2{ (float)X, (float)Y }, Vector2{ (float)X, (float)(Y + lineH) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)X, (float)Y }, Vector2{ (float)(X + lineW), (float)Y }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)(X + W - lineW), (float)Y }, Vector2{ (float)(X + W), (float)Y }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)(X + W), (float)Y }, Vector2{ (float)(X + W), (float)(Y + lineH) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)X, (float)(Y + H - lineH) }, Vector2{ (float)X, (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)X, (float)(Y + H) }, Vector2{ (float)(X + lineW), (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)(X + W - lineW), (float)(Y + H) }, Vector2{ (float)(X + W), (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)(X + W), (float)(Y + H - lineH) }, Vector2{ (float)(X + W), (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
	}

	void DrawSkeleton(USkeletalMeshComponent* Mesh, float distance, ImColor color) {
		if (distance > 100.0f) return;

		std::vector<Vector3> bones = {
			Mesh->GetSocketLocation(66),  // neck
			Mesh->GetSocketLocation(9),   // left shoulder
			Mesh->GetSocketLocation(10),  // left elbow
			Mesh->GetSocketLocation(11),  // left hand
			Mesh->GetSocketLocation(38),  // right shoulder
			Mesh->GetSocketLocation(39),  // right elbow
			Mesh->GetSocketLocation(40),  // right hand
			Mesh->GetSocketLocation(2),   // pelvis
			Mesh->GetSocketLocation(71),  // left hip
			Mesh->GetSocketLocation(72),  // left knee
			Mesh->GetSocketLocation(75),  // left foot
			Mesh->GetSocketLocation(78),  // right hip
			Mesh->GetSocketLocation(79),  // right knee
			Mesh->GetSocketLocation(82),  // right foot
			Mesh->GetSocketLocation(110)  // head
		};

		std::vector<Vector2> screenPositions(bones.size());
		for (size_t i = 0; i < bones.size(); ++i) {
			screenPositions[i] = Cached::PlayerController->ProjectWorldLocationToScreen(bones[i]);
		}

		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		ImU32 shadowColor = ImColor(0, 0, 0, 80);
		float lineThickness = 1.1f;
		float shadowOffset = 1.467f;

		auto DrawLine = [&](Vector2 start, Vector2 end) {
			drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color, lineThickness);
			};

		DrawLine(screenPositions[1], screenPositions[4]);  // left shoulder to right shoulder
		DrawLine(screenPositions[1], screenPositions[2]);  // left shoulder to left elbow
		DrawLine(screenPositions[2], screenPositions[3]);  // left elbow to left hand
		DrawLine(screenPositions[4], screenPositions[5]);  // right shoulder to right elbow
		DrawLine(screenPositions[5], screenPositions[6]);  // right elbow to right hand
		DrawLine(screenPositions[0], screenPositions[7]);  // neck to pelvis
		DrawLine(screenPositions[7], screenPositions[8]);  // pelvis to left hip
		DrawLine(screenPositions[8], screenPositions[9]);  // left hip to left knee
		DrawLine(screenPositions[9], screenPositions[10]); // left knee to left foot
		DrawLine(screenPositions[7], screenPositions[11]); // pelvis to right hip
		DrawLine(screenPositions[11], screenPositions[12]);// right hip to right knee
		DrawLine(screenPositions[12], screenPositions[13]);// right knee to right foot

		float baseRadius = 130.0f;
		float radius = baseRadius / distance;
		drawList->AddCircle(ImVec2(screenPositions[14].x + shadowOffset, screenPositions[14].y + shadowOffset), radius, shadowColor, 64, lineThickness);
		drawList->AddCircle(ImVec2(screenPositions[14].x, screenPositions[14].y), radius, ImGui::ColorConvertFloat4ToU32(color), 64, lineThickness);
	}

	void FullBox(int X, int Y, int W, int H, const ImColor color, int thickness) {
		ImU32 shadowColor = ImColor(0, 0, 0, 255);
		float shadowOffset = 1.0f;

		AddLine(Vector2{ (float)X, (float)Y }, Vector2{ (float)(X + W), (float)Y }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)(X + W), (float)Y }, Vector2{ (float)(X + W), (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)X, (float)(Y + H) }, Vector2{ (float)(X + W), (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
		AddLine(Vector2{ (float)X, (float)Y }, Vector2{ (float)X, (float)(Y + H) }, color, thickness, shadowColor, shadowOffset);
	}

	void DrawFill(int X, int Y, int W, int H, const ImColor color, int thickness) {
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(color));
	}
}