// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamTypes.h"
#include "SteamGlyphUtils.generated.h"


/**
 * 
 */
UCLASS()
class STEAMTOOLSINPUT_API USteamGlyphUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Get the texture that best represents the ActionOrigin, uses TextureOverwrites from the settings first but will fall back to Steam-default textures
	/// @param ActionOrigin Action to get glyph texture for
	/// @return UTexture2D of the glyph that is linked to the button
	static const UTexture2D* GetGlyphForActionOrigin(ESteamInputActionOrigin ActionOrigin);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UTexture2D* GetGlyphForActionOriginBPCompat(FSteamInputActionOrigin ActionOrigin) {return GetGlyphForActionOrigin(ActionOrigin.ActionOrigin);}

	/// @brief Get all action origins that are currently bound to the action
	/// @param ControllerID Controller Handle
	/// @param ActionSet Action Set Handle
	/// @param ActionBinding Action Binding Handle
	/// @return List of all action origins bound to the Action
	static TArray<ESteamInputActionOrigin> GetActionOrigins(FInputHandle ControllerID, FInputActionSetHandle ActionSet,
	                                                        FControllerActionHandle ActionBinding);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FSteamInputActionOrigin> GetActionOriginsBPCompat(FInputHandle ControllerID, FInputActionSetHandle ActionSet,
															FControllerActionHandle ActionBinding);
private:
	UPROPERTY()
	bool TexturesCached = false;
	UPROPERTY()
	TMap<ESteamInputActionOrigin, TObjectPtr<UTexture2D>> GlyphTextureCache;
};
