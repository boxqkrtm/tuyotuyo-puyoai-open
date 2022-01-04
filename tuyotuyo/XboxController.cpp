#if !_WINDLL

#include "XboxController.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
using std::queue;
using std::pair;

XboxController::XboxController()
{
	initController();
}


XboxController::~XboxController()
{
	if(isThreadOn) th.~thread();
	vigem_target_remove(client, pad);
	vigem_target_free(pad);
}

int XboxController::initController()
{
	isThreadOn = false;
	lastInputFrame = 0;
	inputFrame = 0;
	isSucceedFirstSucceed = false;
	client = vigem_alloc();
	//Establish connection to the driver:
	VIGEM_ERROR retval = vigem_connect(client);
	if (!VIGEM_SUCCESS(retval))
	{
		std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
		return -1;
	}
	pad = vigem_target_x360_alloc();
	vigem_target_add(client, pad);//plugin
	performAction();
	return 0;
}

void XboxController::inputAction(int input, int delay)
{
	if (delay <= 0) delay = 1;
	lastInputFrame += delay;
	inputQueue.enqueue(pair<int, int>({ lastInputFrame, input }));
	
}

void XboxController::performActionThread() {
	XINPUT_STATE state;
	pair<int, int> input = { -1,0 };
	while (1) {
		//has input and wait delay
		if (input.first != inputFrame && input.first != -1) {
			inputFrame++;
			//cout << "wait" << inputFrame << endl;
		}
		else
		{
			if (input.first == -1 || input.first == inputFrame) {
				//need new input
				//if (!inputQueue.empty()) {
					input = inputQueue.dequeue();
					state.Gamepad.wButtons = input.second;
					state.Gamepad.sThumbLX = 0;
					state.Gamepad.sThumbLY = 0;
					state.Gamepad.sThumbRX = 0;
					state.Gamepad.sThumbRY = 0;
					state.Gamepad.bLeftTrigger = 0;
					state.Gamepad.bRightTrigger = 0;
					vigem_target_x360_update(client, pad, *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
				//}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void XboxController::performAction()
{
	if (isThreadOn == true) return;
	isThreadOn = true;
	th = std::thread(&XboxController::performActionThread, this);
	th.detach();
}

void XboxController::isReadyToInputTrue()
{
	isSucceedFirstSucceed_mutex.lock();
	isSucceedFirstSucceed = true;
	isSucceedFirstSucceed_mutex.unlock();
}

void XboxController::isReadyToInputFalse()
{
	isSucceedFirstSucceed_mutex.lock();
	isSucceedFirstSucceed = false;
	isSucceedFirstSucceed_mutex.unlock();
}

#endif
