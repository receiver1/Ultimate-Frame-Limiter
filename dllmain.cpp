
#include "samp.hpp"
#include <thread>

uint32_t originalAddress;

void CTimer__UpdateHooked()
{
	static bool isInitializated{ false };
	if (!isInitializated)
	{
		if (samp.isAvailable())
		{
			samp.registerChatCommand("ufl", [](const char* szParams) 
			{
				if (strlen(szParams) < 1)
					return samp.addChatMessage(0x1FD5EDFF, "<ufl> {FFFFFF}Usage: /ufl {1FD5ED}<0-9999>");

				char& frameLimit = *reinterpret_cast<char*>(0xBA6794);

				int iParam = atoi(szParams);
				if (iParam <= 0)
				{
					samp.addChatMessage(0x1FD5EDFF, "<ufl> {FFFFFF}Frame Limiter: {1FD5ED}Disabled");
					samp.setFrameLimiter(0);

					// Выключаем ограничитель FPS
					if (frameLimit)
						frameLimit = 0;
				}
				else
				{
					if (iParam > 9999)
						iParam = 9999;

					char szMessage[64];
					sprintf_s(szMessage, "<ufl> {FFFFFF}Frame Limiter: {1FD5ED}%d", iParam);
					samp.addChatMessage(0x1FD5EDFF, szMessage);
					samp.setFrameLimiter(iParam);

					// Включаем ограничитель FPS
					if (!frameLimit) 
						frameLimit = 1;
				}
			});

			isInitializated = true;
		}
	}
	// Вызываем оригинальный CTimer::Update (0x561B10)
	reinterpret_cast<void(__cdecl*)()>(originalAddress)();
}

class CEntry
{
	uint32_t hookAddress{ 0x53E968 },
		&relativeAddress{ *reinterpret_cast<uint32_t*>(hookAddress + 1) };
	unsigned long ulProtection;
public:
	CEntry()
	{
		VirtualProtect(LPVOID(hookAddress), 5, PAGE_READWRITE, &ulProtection);

		// Сохраняем оригинал для совершения прыжка
		originalAddress = relativeAddress + hookAddress + 5;

		// Подменяем вызов CTimer::Update на свой
		relativeAddress = reinterpret_cast<uint32_t>(&CTimer__UpdateHooked) - hookAddress - 5;

		VirtualProtect(LPVOID(hookAddress), 5, ulProtection, &ulProtection);
	}
	~CEntry()
	{
		VirtualProtect(LPVOID(hookAddress), 5, PAGE_READWRITE, &ulProtection);

		// Подменяем свой вызов на CTimer::Update
		relativeAddress = *reinterpret_cast<uint32_t*>(originalAddress) - hookAddress - 5;

		VirtualProtect(LPVOID(hookAddress), 5, ulProtection, &ulProtection);
	}
} entry;
