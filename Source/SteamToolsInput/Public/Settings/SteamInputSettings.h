// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SteamTypes.h"
#include "SteamInputSettings.generated.h"

/// @brief The different input types steam supports, technically unreal also supports a 3D vector but steam doesn't so we don't give the option
UENUM()
enum class EKeyType: uint8
{
	Button,
	Analog,
	Joystick,
	MouseInput
};

/**
 * Config for the steam input system
 */
UCLASS(config = Input, DefaultConfig)
class STEAMTOOLSINPUT_API USteamInputSettings : public UObject
{
	GENERATED_BODY()

public:

	USteamInputSettings();
	
	/// @brief Map of all the steam keys that are available, each key has an enum to identify the type of key (boolean, analog, 2D analog and mouse)
	UPROPERTY(Config, EditAnywhere, Category = "Actions")
	TMap<FName, EKeyType> Keys;

	/// @brief The name of the Category to put the keys under
	UPROPERTY(Config, EditAnywhere, Category = "Actions")
	FName MenuCategory = "SteamBindings";

	/// @brief Regenerate the keys
	/// @warning This deletes all the keys under the MenuCategory group so make sure you regenerate any keys that should be there
	static void RegenerateKeys();
	/// @brief Simple helper function to provide a simple way to generate keynames for the x key for 2D analog style keys
	/// @param Name Name of the key
	/// @return <Name>_AxisX
	static FName GetXAxisName(FName Name);
	/// @brief Simple helper function to provide a simple way to generate keynames for the y key for 2D analog style keys
	/// @param Name Name of the key
	/// @return <Name>_AxisY
	static FName GetYAxisName(FName Name);

	/// @brief Since slate doesn't use the enhanced input system and we work with custom keys, allow the user to bind the keys slate will listen to for navigation
	UPROPERTY(Config, EditAnywhere, Category = "Slate Input")
	TMap<FKey, EUINavigation> KeyEventRules;

	/// @brief Since slate doesn't use the enhanced input system and we work with custom keys, allow the user to bind the keys slate will listen to for navigation
	UPROPERTY(Config, EditAnywhere, Category = "Slate Input")
	TMap<FKey, EUINavigationAction> KeyActionRules;

	UPROPERTY(Config, EditAnywhere, Category = "Glyphs")
	ESteamGlyphSize DefaultGlyphSize = ESteamGlyphSize::ELarge;
	
	UPROPERTY(Config, EditAnywhere, Category = "Glyphs")
	TMap<ESteamInputActionOrigin, TSoftObjectPtr<UTexture2D>> TextureOverwrites;
	
	/// @brief Update the slate key binds
	static void ApplySlateConfig();
	
	TMap<FName, FControllerActionHandle> Handles;
	
	UFUNCTION()
	static TArray<FName> GetFSteamKeysOptions();
private:
	virtual void PostInitProperties() override;
	void GenerateKey(FName ActionName, EKeyType KeyType) const;

	void Internal_ApplySlateConfig() const;
	
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void ReloadCache();

};

USTRUCT(BlueprintType)
struct FSteamKey
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Steam", meta = (GetOptions="SteamToolsInput.SteamInputSettings.GetFSteamKeysOptions"))
	FName Key = "default";

	FSteamKey() = default;
	FSteamKey(const FName Key) : Key(Key) {}
	operator FName() const {return Key;}
};
