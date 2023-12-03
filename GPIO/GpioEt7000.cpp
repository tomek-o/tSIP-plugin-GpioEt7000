//---------------------------------------------------------------------------


#pragma hdrstop

#include "GpioEt7000.h"
#include "PhoneLocal.h"
#include "Device.h"
#include "Utils.h"
#include <algorithm>
#include <stdio.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

#include "Log.h"

std::string GpioEt7000::initBuf = "";
static const unsigned char request[] = {
	0x01, 0x02	/* transaction id */,
	0x00, 0x00	/* protocol ID */,
	0x00, 0x06	/* bytes following */,
	0x01		/* unitID */,
	0x02		/* func */,
	0x00, 0x00	/* start addr */,
	0x00, 0x10	/* 16 inputs */
	};
std::string GpioEt7000::keepAliveBuf(reinterpret_cast<const char*>(request), sizeof(request));

std::string varStateName;
std::string varConnName;

GpioEt7000::GpioEt7000(Device &device):
	device(device),
	connected(false),
	connectedInitialized(false)
{
	std::string path = Utils::GetDllPath();
	std::string dllName = Utils::ExtractFileNameWithoutExtension(path);
	varStateName = dllName;
	#pragma warn -8091	// incorrectly issued by BDS2006
	std::transform(varStateName.begin(), varStateName.end(), varStateName.begin(), tolower);
	varConnName = varStateName;
	varStateName += "State";
	varConnName += "Conn";
}

int GpioEt7000::OnDeviceBufRx(std::string buf)
{
#if 0
	LOG("RX %s", cmd.c_str());
	const char *ptr = cmd.c_str();
	if (StartsWith(ptr, "SETVARIABLE ")) {
		SetVariable("TEST", "TESTVAL");
	} else if (StartsWith(ptr, "CLEARVARIABLE ")) {
		ClearVariable("TEST");
	}
#endif

	rxBuf += buf;
	if (rxBuf.size() >= 11)
	{
		if (
			rxBuf[0] == request[0] &&
			rxBuf[1] == request[1] &&
			rxBuf[2] == request[2] &&
			rxBuf[3] == request[3] &&
			rxBuf[4] == request[4] &&
			rxBuf[5] == 4 &&
			rxBuf[6] == request[6] &&
			rxBuf[7] == request[7] &&	// func code
			rxBuf[8] == 2				// byte count
			)
		{
			unsigned int diValue = (rxBuf[9] << 8) + rxBuf[10];
			char str[16];
			sprintf(str, "%d", diValue);
			SetVariable(varStateName.c_str(), str);
		}
		else
		{
			SetVariable(varConnName.c_str(), "0");
			return -1;
		}
	}
	return 0;
}

void GpioEt7000::OnPoll(void)
{
	if (connected != device.isConnected() || !connectedInitialized)
	{
		connected = device.isConnected();
		connectedInitialized = true;
		if (connected)
		{
			SetVariable(varConnName.c_str(), "1");
		}
		else
		{
			SetVariable(varConnName.c_str(), "0");
		}
	}
}
