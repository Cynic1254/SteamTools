﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/SteamInputSettings.h"

#include "SteamToolsInput.h"
#include "Controller/FSteamInputController.h"
#include "Framework/Application/NavigationConfig.h"

USteamInputSettings::USteamInputSettings()
{
	if (FSlateApplication::IsInitialized())
	{
		const TSharedRef<FNavigationConfig> Config = FSlateApplication::Get().GetNavigationConfig();
		KeyActionRules = Config->KeyActionRules;
		KeyEventRules = Config->KeyEventRules;
	}

	ReloadCache();
}

void USteamInputSettings::RegenerateKeys()
{
	USteamInputSettings* Settings = GetMutableDefault<USteamInputSettings>();
	
	EKeys::RemoveKeysWithCategory(Settings->MenuCategory);

	for (const auto Key : Settings->Keys)
	{
		Settings->GenerateKey(Key.Key, Key.Value);
	}

	Settings->ReloadCache();
}

FName USteamInputSettings::GetXAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisX")};
}

FName USteamInputSettings::GetYAxisName(const FName Name)
{
	return FName{Name.ToString() + TEXT("_AxisY")};
}

void USteamInputSettings::ApplySlateConfig()
{
	GetDefault<USteamInputSettings>()->Internal_ApplySlateConfig();
}

void USteamInputSettings::PostInitProperties()

{
	UObject::PostInitProperties();

	RegenerateKeys();
	ApplySlateConfig();
}

void USteamInputSettings::GenerateKey(const FName ActionName, const EKeyType KeyType) const
{
	switch (KeyType)
	{
	case EKeyType::Button:
		{
			const FKey Key{ActionName};
			if (EKeys::GetKeyDetails(Key) == nullptr)
			{	
				EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey, MenuCategory});
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Key %s already exists"), *Key.ToString());
			}
		}
		break;
	case EKeyType::Analog:
		{
			const FKey Key{ActionName};
			if (EKeys::GetKeyDetails(Key) == nullptr)
			{
				EKeys::AddKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Key %s already exists"), *Key.ToString());
			}
		}
		break;
	case EKeyType::MouseInput:
	case EKeyType::Joystick:
		{
			const FKey KeyX{GetXAxisName(ActionName)};
			const FKey KeyY{GetYAxisName(ActionName)};
			const FKey Key{ActionName};

			EKeys::AddKey({KeyX, KeyX.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			EKeys::AddKey({KeyY, KeyY.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis1D, MenuCategory});
			EKeys::AddPairedKey({Key, Key.GetDisplayName(), FKeyDetails::GamepadKey | FKeyDetails::Axis2D, MenuCategory}, KeyX, KeyY);
		}
		break;
	}
}

void USteamInputSettings::Internal_ApplySlateConfig() const
{
	if (FSlateApplication::IsInitialized())
	{
		const TSharedRef<FNavigationConfig> Config = FSlateApplication::Get().GetNavigationConfig();
		Config->KeyActionRules = KeyActionRules;
		Config->KeyEventRules = KeyEventRules;
	}
}

#if WITH_EDITOR
void USteamInputSettings::PreEditChange(FProperty* PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(USteamInputSettings, MenuCategory))
	{
		EKeys::RemoveKeysWithCategory(MenuCategory);
	}
}

void USteamInputSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	RegenerateKeys();
	ApplySlateConfig();
}

	
#endif

void USteamInputSettings::ReloadCache()
{
	Handles.Empty();

	if (!FSteamToolsInputModule::Get().SteamInputInitialized)
	{
		return;
	}

	for (const auto [KeyName, Type] : Keys)
	{
		switch (Type)
		{
		case EKeyType::Button:
			Handles.Add(KeyName, {SteamInput()->GetDigitalActionHandle(TCHAR_TO_UTF8(*KeyName.ToString())), ActionType::EDigital});
			break;
		case EKeyType::Analog:
		case EKeyType::Joystick:
		case EKeyType::MouseInput:
		default:
			Handles.Add(KeyName, {SteamInput()->GetAnalogActionHandle(TCHAR_TO_UTF8(*KeyName.ToString())), ActionType::EAnalog});
		}
	}
}

TArray<FName> USteamInputSettings::GetFSteamKeysOptions()
{
	UE_LOG(LogTemp, Warning, TEXT("CALLED GetFSteamKeysOptions"));
	TArray<FName> Keys;
	GetDefault<USteamInputSettings>()->Keys.GetKeys(Keys);
	return Keys;
}
