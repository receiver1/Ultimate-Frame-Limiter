
#include "samp.hpp"
#include <thread>

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
	// Вызываем оригинальный CTimer::Update
	reinterpret_cast<void(__cdecl*)()>(0x561B10)();
}

class CEntry
{
public:
	CEntry()
	{
		// Подменяем вызов CTimer::Update на свой
		*reinterpret_cast<uint32_t*>(0x53E968 + 1) = 
			reinterpret_cast<uint32_t>(&CTimer__UpdateHooked) - 0x53E968 - 5;
	}
	~CEntry()
	{
		// Подменяем свой вызов на CTimer::Update
		//*reinterpret_cast<uint32_t*>(0x53E968 + 1) =
		//	*reinterpret_cast<uint32_t*>(0x561B10) - 0x53E968 - 5;
	}
} entry;
