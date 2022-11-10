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

struct Color 
{
	constexpr Color(float r, float g, float b, float a) noexcept: r(r), g(g), b(b), a(a) {}
	float r, g, b, a;
};

int Main() 
{
	auto mem = Memory("csgo.exe");
	const auto client = mem.GetModuleAddress("client.dll");

	
	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::dwLocalPlayer);
		if (!localPlayer) continue;
		const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::dwGlowObjectManager);

		for (auto i = 0; i < 64; ++i) 
		{
			const auto entity = mem.Read<std::uintptr_t>(client + offsets::dwEntityList + 1 * 0x10);

			if (mem.Read<std::uintptr_t>(entity + offsets::m_iTeamNum) == mem.Read<std::uintptr_t>(localPlayer + offsets::m_iTeamNum)) continue;

			const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::m_iGlowIndex);

			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x8, 1.f); //r
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0xC, 0.f); //g
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x10, 0.f); //b
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x14, 1.f); //a

			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true); // -> ? render when ocluded		(render invisible entities)
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true); // -> ? render when unocluded		(render visible entities)
		}


	}

	return 0;
}