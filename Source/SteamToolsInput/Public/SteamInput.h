// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamInput.generated.h"

typedef uint64 InputHandle_t;
typedef uint64 InputActionSetHandle_t;

/// @brief Wrapper to allow the use of InputHandle_t inside unreal, for c++ this gets freely converted
USTRUCT(BlueprintType)
struct FInputHandle
{
	GENERATED_BODY()

	FInputHandle() = default;
	FInputHandle(const InputHandle_t Handle) : BP_Num(Handle), ControllerID(Handle) {}
	
	operator InputHandle_t() const {return ControllerID;}

	UPROPERTY(BlueprintReadOnly)
	int BP_Num;
	InputHandle_t ControllerID = 0;
};

/// @brief Wrapper to allow the use of InputActionSetHandle_t inside unreal, for c++ this gets freely converted
USTRUCT(BlueprintType)
struct FInputActionSetHandle
{
	GENERATED_BODY()

	FInputActionSetHandle() = default;
	FInputActionSetHandle(const InputActionSetHandle_t Handle) : BP_Num(Handle), ActionSetHandle(Handle) {}

	operator InputActionSetHandle_t() const {return ActionSetHandle;}

	UPROPERTY(BlueprintReadOnly)
	int BP_Num;
	InputActionSetHandle_t ActionSetHandle = 0;
};

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
};
