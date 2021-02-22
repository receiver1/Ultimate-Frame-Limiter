#pragma once

#include <windows.h>
#include <iostream>

using CMDPROC = void(__cdecl* )(const char*);

class COffsets
{
public:
	unsigned long CInput{ 0 }, RegisterChatCommand{ 0 };
	unsigned long CChat{ 0 }, AddChatMessage{ 0 };
	unsigned long CGame{ 0 }, SetFrameLimiter{ 0 };

	COffsets(unsigned char versionByte)
	{
		switch (versionByte)
		{
			case 0xD8: // R1
			{
				CInput = 0x21A0E8;
				RegisterChatCommand = 0x65AD0;

				CChat = 0x21A0E4;
				AddChatMessage = 0x645A0;

				CGame = 0x21A10C;
				SetFrameLimiter = 0x9C190;
			} break;
			case 0xA8: // R2
			{
				CInput = 0x21A0F0;
				RegisterChatCommand = 0x65BA0;

				CChat = 0x21A0EC;
				AddChatMessage = 0x64670; 

				CGame = 0x21A114;
				SetFrameLimiter = 0x9C240;
			} break;
			case 0x78: // R3
			{
				CInput = 0x26E8CC;
				RegisterChatCommand = 0x69000;

				CChat = 0x26E8C8;
				AddChatMessage = 0x679F0;

				CGame = 0x26E8F4;
				SetFrameLimiter = 0xA04A0;
			} break;
		}
	}
};

class CSAMP
{
	class COffsets* pOffsets{ nullptr };
	struct CInput* pInput{ nullptr };
	struct CChat* pChat{ nullptr };
	struct CGame* pGame{ nullptr };
public:
	unsigned long baseAddress;
	unsigned char versionByte;
	unsigned long getOffset(unsigned long offset)
	{
		return baseAddress + offset;
	}

	bool isAvailable()
	{
		baseAddress = reinterpret_cast<unsigned long>(GetModuleHandle("samp.dll"));
		if (!baseAddress) return false;

		versionByte = *reinterpret_cast<unsigned char*>(getOffset(0x1036));
		if (!versionByte) return false;

		pOffsets = new COffsets(versionByte);
		if (!pOffsets) return false;

		pInput = *reinterpret_cast<struct CInput**>(getOffset(pOffsets->CInput));
		if (!pInput) return false;

		pChat = *reinterpret_cast<struct CChat**>(getOffset(pOffsets->CChat));
		if (!pChat) return false;

		pGame = *reinterpret_cast<struct CGame**>(getOffset(pOffsets->CGame));
		if (!pGame) return false;

		return true;
	}

	void registerChatCommand(const char* szCommand, CMDPROC pFunction)
	{
		reinterpret_cast<void(__thiscall*)(CInput*, const char*, CMDPROC)>
			(getOffset(pOffsets->RegisterChatCommand))(pInput, szCommand, pFunction);
	}

	void addChatMessage(unsigned long ulColor, const char* szText)
	{
		reinterpret_cast<void(__thiscall*)(CChat*, unsigned long, const char*)>
			(getOffset(pOffsets->AddChatMessage))(pChat, ulColor, szText);
	}
	
	void setFrameLimiter(int iValue)
	{
		reinterpret_cast<void(__thiscall*)(CGame*, int)>
			(getOffset(pOffsets->SetFrameLimiter))(pGame, iValue);
	}
} samp;
