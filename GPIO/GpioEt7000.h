//---------------------------------------------------------------------------

#ifndef GpioEt7000H
#define GpioEt7000H
//---------------------------------------------------------------------------

#include <string>

class Device;

class GpioEt7000
{
public:
	static std::string initBuf;
	static std::string keepAliveBuf;

	enum { KEEPALIVE_PERIOD = 10000 };	///< note initCmd must be also updated to maintain time margin
	GpioEt7000(Device &device);
protected:
	Device &device;
	std::string rxBuf;
	bool connected;
	bool connectedInitialized;

	friend int OnDeviceBufReceive(std::string cmd, void* opaque);
	int OnDeviceBufRx(std::string cmd);

	friend void OnPeriodicPoll(void* opaque);
	void OnPoll(void);
};

#endif
