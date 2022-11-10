#include "mem.h""
#include <thread>
namespace offsets 
{
	// client.dll
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDE6964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFBE54;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x5356910;

	// entity
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
}

__declspec(align(16)) struct Color
{
	constexpr Color(const float r, const float g, const float b, const float a = 1.f) noexcept:
		r(r), g(g), b(b), a(a) { }
	float r, g, b, a;
};

int main() 
{
	auto mem = Memory("csgo.exe");
	const auto client = mem.GetModuleAddress("client.dll");
	const auto frn = Color(0.f, 1.f, 0.f);
	const auto foe = Color(1.f, 0.f, 0.f);

	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::dwLocalPlayer);

		if (!localPlayer) continue;
		
		const auto localPlayerTeam = mem.Read<std::uintptr_t>(localPlayer + offsets::m_iTeamNum);
		const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::dwGlowObjectManager);

		for (auto i = 0; i < 32; ++i) 
		{
			const auto entity = mem.Read<std::uintptr_t>(client + offsets::dwEntityList + i * 0x10);
			if (!entity) continue;
			const auto entityTeam = mem.Read<std::uintptr_t>(entity + offsets::m_iTeamNum);
			const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::m_iGlowIndex);

			mem.Write<Color>(glowObjectManager + (glowIndex * 0x38) + 0x8, ((entityTeam == localPlayerTeam) ? frn : foe)); // glow
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true); // -> ? render when ocluded		(render invisible entities)
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true); // -> ? render when unocluded		(render visible entities)
		}
	}
	return 0;
}