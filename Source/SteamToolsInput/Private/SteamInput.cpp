// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamInput.h"

#include "SteamToolsInput.h"
#include "Controller/FSteamInputController.h"

#define RETURN_IF_STEAM_INACTIVE(value) \
if (!FSteamToolsInputModule::Get().SteamInputInitialized) \
		return value;

void USteamInput::ActivateActionSet(const int32 ControllerID, const FInputActionSetHandle ActionSet)
{
	RETURN_IF_STEAM_INACTIVE()

	FSteamInputController::ActivateActionSet(ControllerID, ActionSet);
}

FInputHandle USteamInput::GetControllerHandle(const uint8 PlayerIndex)
{
	RETURN_IF_STEAM_INACTIVE(-1)

	InputHandle_t Controllers[STEAM_INPUT_MAX_COUNT];
	const int ControllerCount = SteamInput()->GetConnectedControllers(Controllers);
	
	return PlayerIndex < ControllerCount ? Controllers[PlayerIndex] : -1;
}

FInputActionSetHandle USteamInput::GetActionSetHandle(const FName ActionSetName)
{
	RETURN_IF_STEAM_INACTIVE(-1)
	
	return SteamInput()->GetActionSetHandle(TCHAR_TO_UTF8(*ActionSetName.ToString()));
}