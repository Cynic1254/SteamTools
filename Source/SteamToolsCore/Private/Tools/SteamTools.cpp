// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/SteamTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "steam/isteamapps.h"

FString USteamTools::GetBuildVersion()
{
	FString BuildVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		BuildVersion,
		GGameIni
		);
	
	if (!SteamApps())
	{
		return BuildVersion + ".Developer";
	}
	
	const int BuildID = SteamApps()->GetAppBuildId();
	return BuildVersion + '.' + FString::FromInt(BuildID);
}
