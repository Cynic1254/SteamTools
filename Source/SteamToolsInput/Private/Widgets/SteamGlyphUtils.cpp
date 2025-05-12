// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SteamGlyphUtils.h"

#include "ImageUtils.h"
#include "SteamInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

const UTexture2D* USteamGlyphUtils::GetGlyphForActionOrigin(ESteamInputActionOrigin ActionOrigin)
{
	USteamGlyphUtils* MutableDefault = GetMutableDefault<USteamGlyphUtils>();
	const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
	
	if (!MutableDefault->TexturesCached)
	{
		for (const auto [InputAction, Texture] : Settings->TextureOverwrites)
		{
			MutableDefault->GlyphTextureCache.Add(InputAction, Texture.Get());
		}
		MutableDefault->TexturesCached = true;
	}

	if (MutableDefault->GlyphTextureCache.Contains(ActionOrigin) && UKismetSystemLibrary::IsValid(*(MutableDefault->GlyphTextureCache.Find(ActionOrigin))))
	{
		return *(MutableDefault->GlyphTextureCache.Find(ActionOrigin));
	}

	return MutableDefault->GlyphTextureCache.Add(ActionOrigin, FImageUtils::ImportFileAsTexture2D(SteamInput()->GetGlyphPNGForActionOrigin(static_cast<EInputActionOrigin>(ActionOrigin), static_cast<ESteamInputGlyphSize>(Settings->DefaultGlyphSize), 0)));
}

TArray<ESteamInputActionOrigin> USteamGlyphUtils::GetActionOrigins(FInputHandle ControllerID,
                                                                   FInputActionSetHandle ActionSet,
                                                                   FControllerActionHandle ActionBinding)
{
	const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
	EInputActionOrigin Out[STEAM_INPUT_MAX_ORIGINS]{};
	int Amount = 0;

	switch (USteamInput::GetActionHandleType(ActionBinding))
	{
	case ActionType::EDigital:
		Amount = SteamInput()->GetDigitalActionOrigins(ControllerID, ActionSet, ActionBinding.GetDigitalActionHandle(), Out);
		break;
	case ActionType::EAnalog:
		Amount = SteamInput()->GetAnalogActionOrigins(ControllerID, ActionSet, ActionBinding.GetAnalogActionHandle(), Out);
	case ActionType::EUnknown:
	default:
		break;
	}

	return TArray{reinterpret_cast<ESteamInputActionOrigin*>(Out), Amount};
}

TArray<FSteamInputActionOrigin> USteamGlyphUtils::GetActionOriginsBPCompat(FInputHandle ControllerID,
	FInputActionSetHandle ActionSet, FControllerActionHandle ActionBinding)
{
	const auto Array = GetActionOrigins(ControllerID, ActionSet, ActionBinding);

	TArray<FSteamInputActionOrigin> Return;

	for (const ESteamInputActionOrigin Value : Array)
	{
		Return.Add({Value});
	}
	
	return Return;
}
