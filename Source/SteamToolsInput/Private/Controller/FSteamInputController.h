#pragma once
#include "IInputDevice.h"
#include "SteamToolsInput.h"
#include "SteamTypes.h"
#include "steam/isteamcontroller.h"

enum class ActionType : uint8;
enum class EKeyType : uint8;
struct FSteamKey;

class FSteamInputController : public IInputDevice
{
public:
	FSteamInputController(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler);
	virtual ~FSteamInputController() override;
	virtual void SendControllerEvents() override;
	virtual void Tick(float DeltaTime) override {}
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override
	{
		MessageHandler = InMessageHandler;
	}
	virtual bool IsGamepadAttached() const override
	{
		return (FSteamToolsInputModule::Get().SteamInputInitialized && bControllerInitialized);
	}
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		return false;
	}
	void SetVibration(int32 ControllerID, const FForceFeedbackValues& Values);

	static void ActivateActionSet(int32 ControllerID, const InputActionSetHandle_t ActionSetHandle);

	ActionType GetActionHandleType(ControllerActionHandle_t ActionHandle) const;
private:
	bool bControllerInitialized = false;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	double InitialButtonRepeatDelay = 0.2;
	double ButtonRepeatDelay = 0.1;

	TMap<ControllerDigitalActionHandle_t, FName> DigitalActions;
	TMap<ControllerAnalogActionHandle_t, TTuple<FName, EKeyType>> AnalogActions;

	struct FControllerState
	{
		/** Analog status for all actions from previous frame, on a -1.0 to 1.0 range */
		TMap<FName, ControllerAnalogActionData_t> AnalogStatusMap;

		/** Button status for all actions from previous frame (pressed down or not) */
		TMap<FName, bool> DigitalStatusMap;

		/** List of times that if a button is still pressed counts as a "repeated press" */
		TMap<FName, double> DigitalRepeatTimeMap;

		/** Values for force feedback on this controller.  We only consider the LEFT_LARGE channel for SteamControllers */
		FForceFeedbackValues VibeValues;

		int32 MouseX;
		int32 MouseY;

		FControllerState() : MouseX(0), MouseY(0) {}

	};

	static InputActionSetHandle_t ActiveActionSet[STEAM_INPUT_MAX_COUNT];
	FControllerState ControllerStates[STEAM_INPUT_MAX_COUNT];
};
