// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SteamGlyphUtils.h"

#include "ImageUtils.h"
#include "SteamInput.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

TMap<ESteamInputActionOrigin, TObjectPtr<UTexture2D>> USteamGlyphUtils::GlyphTextureCache{};
bool USteamGlyphUtils::TexturesCached = false;

const UTexture2D* USteamGlyphUtils::GetGlyphForActionOrigin(ESteamInputActionOrigin ActionOrigin)
{
	const USteamInputSettings* Settings = GetDefault<USteamInputSettings>();
	if (!TexturesCached)
	{
		for (const auto [InputAction, Texture] : Settings->TextureOverwrites)
		{
			GlyphTextureCache.Add(InputAction, Texture.Get());
		}
	}

	if (GlyphTextureCache.Contains(ActionOrigin))
	{
		return *GlyphTextureCache.Find(ActionOrigin);
	}

	return GlyphTextureCache.Add(ActionOrigin, FImageUtils::ImportFileAsTexture2D(SteamInput()->GetGlyphPNGForActionOrigin(static_cast<EInputActionOrigin>(ActionOrigin), static_cast<ESteamInputGlyphSize>(Settings->DefaultGlyphSize), 0)));
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
	default:
		Amount = SteamInput()->GetAnalogActionOrigins(ControllerID, ActionSet, ActionBinding.GetAnalogActionHandle(), Out);
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
