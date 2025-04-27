#pragma once

#include "CoreMinimal.h"
#include "SteamSharedModule.h"
#include "Modules/ModuleManager.h"

class FSteamToolsCoreModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
private:
    TSharedPtr<class FSteamClientInstanceHandler> APIHandle = nullptr;
};
