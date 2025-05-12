// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamTypes.h"
#include "Settings/SteamInputSettings.h"
#include "SteamInput.generated.h"

/**
 * Steam API function wrappers
 */
UCLASS()
class STEAMTOOLSINPUT_API USteamInput : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Activate the action set for the specified controller
	/// @param ControllerID Handle of the controller to get the id from
	/// @param ActionSet Handle of the action set to enable
	UFUNCTION(BlueprintCallable)
	static void ActivateActionSet(int32 ControllerID, FInputActionSetHandle ActionSet);

	/// @brief Get the input handle steam uses to identify the controller
	/// @param PlayerIndex Index of the player to get the controller for, this assumes the index is sequential and starts at 0 (player 1 is id 0, player 2 is id 1, etc.)
	/// @return Handle to the steam controller or -1 if no handle was found
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FInputHandle GetControllerHandle(uint8 PlayerIndex);

	/// @brief Get the action set handle of the action
	/// @param ActionSetName String name of the action to get
	/// @return Handle to the action set or -1 if steam isn't initialized and 0 if the handle isn't found
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FInputActionSetHandle GetActionSetHandle(FName ActionSetName);

	/// @brief Test if the provided controller is currently being used by steam as a steam controller.
	/// @param ControllerID Id of the controller to check
	/// @return True if steam is currently using this controller as a steam controller, false otherwise
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsSteamController(int32 ControllerID);

	/// @brief Invokes the Steam overlay and brings up the binding screen.
	/// @param ControllerID Id of the controller to open the panel for
	/// @return true for success; false if overlay is disabled/unavailable. If the player is using Big Picture Mode the configuration will open in the overlay. In desktop mode a popup window version of Big Picture will be created and open the configuration.
	UFUNCTION(BlueprintCallable)
	static bool ShowBindingPanel(int32 ControllerID);

	/// @brief Get the action handle from the action name
	/// @param ActionName Name of the action handle to get
	/// @return Action handle
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FControllerActionHandle GetActionHandle(FName ActionName);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FControllerActionHandle GetActionHandleSteamKey(FSteamKey ActionKey);

	/// @brief Get the type of action that the action handle is (Analog or Digital)
	/// @param Handle Action Handle to get the action type from
	/// @return Digital or Analog
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ActionType GetActionHandleType(FControllerActionHandle Handle);
};
