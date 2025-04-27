#pragma once

#include "CoreMinimal.h"
#include "IInputDeviceModule.h"

class FSteamToolsInputModule : public IInputDeviceModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    bool SteamInputInitialized = false;

    TSharedPtr<class FSteamInputController> GetInputDevice() {return Controller;}
    virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

    /// @brief Easy access get function for the module
    /// @return Pointer to the module loaded through LoadModuleChecked
    static inline class FSteamToolsInputModule& Get()
    {
        return FModuleManager::LoadModuleChecked<class FSteamToolsInputModule>("SteamToolsInput");
    }

private:
    static void RegisterSettings();
    static void UnregisterSettings();
    /// @brief Handle to the Steam API dll, while this is valid we know the steam api is loaded
    TSharedPtr<class FSteamClientInstanceHandler> APIHandle = nullptr;

    TSharedPtr<class FSteamInputController> Controller = nullptr;
};
