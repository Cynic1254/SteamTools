#include "FSteamInputController.h"

#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

InputActionSetHandle_t FSteamInputController::ActiveActionSet[STEAM_INPUT_MAX_COUNT] = {};

FSteamInputController::FSteamInputController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) :
MessageHandler(InMessageHandler)
{
	GConfig->GetDouble(TEXT("/Script/Engine.InputSettings"), TEXT("InitialButtonRepeatDelay"), InitialButtonRepeatDelay, GInputIni);
	GConfig->GetDouble(TEXT("/Script/Engine.InputSettings"), TEXT("ButtonRepeatDelay"), ButtonRepeatDelay, GInputIni);
	
	if (FSteamToolsInputModule::Get().SteamInputInitialized && SteamInput() != nullptr)
	{
		const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
		
		for (auto Key : Settings->Keys)
		{
			for (auto Controller : ControllerStates)
			{
				if (Key.Value == EKeyType::Button)
				{
					ControllerDigitalActionHandle_t ActionHandle = SteamInput()->GetDigitalActionHandle(TCHAR_TO_UTF8(*Key.Key.ToString()));

					DigitalActions.Add(ActionHandle, Key.Key);
					
					Controller.DigitalStatusMap.Add(Key.Key, false);
					Controller.DigitalRepeatTimeMap.Add(Key.Key, 0.0);
				}
				else
				{
					ControllerAnalogActionHandle_t ActionHandle = SteamInput()->GetAnalogActionHandle(TCHAR_TO_UTF8(*Key.Key.ToString()));

					AnalogActions.Add(ActionHandle, Key);
					
					ControllerAnalogActionData_t ActionData;
					ActionData.x = 0.0f;
					ActionData.y = 0.0f;
					Controller.AnalogStatusMap.Add(Key.Key, ActionData);
				}
			}
		}

		bControllerInitialized = true;
	}
}

FSteamInputController::~FSteamInputController()
{
	bControllerInitialized = false;
}

void FSteamInputController::SendControllerEvents()
{
	if (!bControllerInitialized)
	{
		return;
	}

	const double CurrentTime = FPlatformTime::Seconds();
	InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
	const int ControllerCount = SteamInput()->GetConnectedControllers(Controllers);
	
	for (int i = 0; i < ControllerCount; ++i)
	{
		const InputHandle_t Controller = Controllers[i];
		FControllerState& State = ControllerStates[i];
		
		SteamInput()->ActivateActionSet(Controller, ActiveActionSet[i]);

		static FName SystemName(TEXT("SteamController"));
		static FString ControllerName(TEXT("SteamController"));
		FInputDeviceScope InputScope(this, SystemName, i, ControllerName);

		FPlatformUserId UserId = FPlatformMisc::GetPlatformUserForUserIndex(i);
		FInputDeviceId DeviceId = INPUTDEVICEID_NONE;
		IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(i, UserId, DeviceId);
		
		for (const auto DigitalAction : DigitalActions)
		{
			const InputDigitalActionData_t ActionData = SteamInput()->GetDigitalActionData(Controller, DigitalAction.Key);
			
			if (State.DigitalStatusMap.FindOrAdd(DigitalAction.Value) == false && ActionData.bState)
			{
				MessageHandler->OnControllerButtonPressed(DigitalAction.Value, UserId, DeviceId, false);
				State.DigitalRepeatTimeMap.FindOrAdd(DigitalAction.Value) = FPlatformTime::Seconds() + ButtonRepeatDelay;
			}
			else if (State.DigitalStatusMap.FindOrAdd(DigitalAction.Value) == true && !ActionData.bState)
			{
				MessageHandler->OnControllerButtonReleased(DigitalAction.Value, UserId, DeviceId, false);
			}
			else if (State.DigitalStatusMap.FindOrAdd(DigitalAction.Value) == true && ActionData.bState && State.DigitalRepeatTimeMap.FindOrAdd(DigitalAction.Value) <= CurrentTime)
			{
				State.DigitalRepeatTimeMap.FindOrAdd(DigitalAction.Value) += ButtonRepeatDelay;
				MessageHandler->OnControllerButtonPressed(DigitalAction.Value, UserId, DeviceId, true);
			}

			State.DigitalStatusMap.FindOrAdd(DigitalAction.Value) = ActionData.bState;
		}
		
		for (const auto AnalogAction : AnalogActions)
		{
			const InputAnalogActionData_t ActionData = SteamInput()->GetAnalogActionData(Controller, AnalogAction.Key);
			const auto Key = AnalogAction.Value;
			
			if (Key.Value == EKeyType::MouseInput)
			{
				State.MouseX += ActionData.x;
				State.MouseY += ActionData.y;
				
				MessageHandler->OnRawMouseMove(ActionData.x, ActionData.y);
			}
			else if (Key.Value == EKeyType::Analog)
			{
				
				if (State.AnalogStatusMap.FindOrAdd(Key.Key).x != ActionData.x)
				{
					MessageHandler->OnControllerAnalog(Key.Key, UserId, DeviceId, ActionData.x);
				}
			} else
			{
				if (State.AnalogStatusMap.FindOrAdd(Key.Key).x != ActionData.x)
				{
					MessageHandler->OnControllerAnalog(USteamInputSettings::GetXAxisName(Key.Key), UserId, DeviceId, ActionData.x);
				}
					
				if (State.AnalogStatusMap.FindOrAdd(Key.Key).y != ActionData.y)
				{
					MessageHandler->OnControllerAnalog(USteamInputSettings::GetYAxisName(Key.Key), UserId, DeviceId, ActionData.y);
				}
			}
			
			State.AnalogStatusMap.FindOrAdd(Key.Key) = ActionData;
		}
	}
}

void FSteamInputController::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	if (ChannelType != FForceFeedbackChannelType::LEFT_LARGE && ChannelType != FForceFeedbackChannelType::RIGHT_LARGE)
	{
		return;
	}

	if (ControllerId >= 0 && ControllerId < STEAM_INPUT_MAX_COUNT)
	{
		FForceFeedbackValues Values;
		if (ChannelType == FForceFeedbackChannelType::LEFT_LARGE)
		{
			Values.LeftLarge = Value;
		}
		else
		{
			Values.RightLarge = Value;
		}

		SetVibration(ControllerId, Values);
	}
}

void FSteamInputController::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{
	if (ControllerId >= 0 && ControllerId < STEAM_INPUT_MAX_COUNT)
	{
		SetVibration(ControllerId, values);
	}
}

void FSteamInputController::SetVibration(int32 ControllerID, const FForceFeedbackValues& Values)
{
	InputHandle_t Controller = SteamInput()->GetControllerForGamepadIndex(ControllerID);
	if (!Controller || !IsGamepadAttached())
	{
		return;
	}

	if (Values.LeftLarge > 0.0f)
	{
		SteamInput()->Legacy_TriggerHapticPulse(Controller, k_ESteamControllerPad_Left, Values.LeftLarge * 4000.0f);
	}

	if (Values.RightLarge > 0.0f)
	{
		SteamInput()->Legacy_TriggerHapticPulse(Controller, k_ESteamControllerPad_Right, Values.RightLarge * 4000.0f);
	}
}

void FSteamInputController::ActivateActionSet(const int32 ControllerID, const InputActionSetHandle_t ActionSetHandle)
{
	if (ControllerID < STEAM_INPUT_MAX_COUNT)
	{
		ActiveActionSet[ControllerID] = ActionSetHandle;
	}
}
