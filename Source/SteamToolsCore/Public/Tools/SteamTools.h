// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamTools.generated.h"

/**
 * 
 */
UCLASS()
class STEAMTOOLSCORE_API USteamTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Get the version of the build, this uses steam's build number to ensure automatic build version updates
	/// @return Build version in the format of: <ProjectVersion>.<Steam build id> or <ProjectVersion>.developer for non-steam builds
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString GetBuildVersion();
};
