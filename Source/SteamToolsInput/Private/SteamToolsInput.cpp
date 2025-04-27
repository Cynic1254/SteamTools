#include "SteamToolsInput.h"

#include "SteamSharedModule.h"
#include "Controller/FSteamInputController.h"
#include "Settings/SteamInputSettings.h"
#include "steam/isteaminput.h"

#define LOCTEXT_NAMESPACE "FSteamToolsInputModule"

void FSteamToolsInputModule::StartupModule()
{
    IInputDeviceModule::StartupModule();

    RegisterSettings();
    
    APIHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
    if (APIHandle && SteamInput())
    {
        SteamInput()->Init(false);
        SteamInputInitialized = true;
    }

    EKeys::AddMenuCategoryDisplayInfo("SteamBindings", LOCTEXT("SteamBindings", "Steam Input Bindings"), TEXT("GraphEditor.PadEvent_16x"));

    USteamInputSettings::RegenerateKeys();
}

void FSteamToolsInputModule::ShutdownModule()
{
    IInputDeviceModule::ShutdownModule();
    
    if (SteamInput() != nullptr)
    {
        SteamInput()->Shutdown();
    }
    
    APIHandle.Reset();
    SteamInputInitialized = false;

    UnregisterSettings();
}

TSharedPtr<class IInputDevice> FSteamToolsInputModule::CreateInputDevice(
    const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
    if (Controller.IsValid())
    {
        return Controller;
    }
    
    if (SteamInputInitialized)
    {
        Controller = MakeShared<FSteamInputController>(InMessageHandler);
        return Controller;
    }

    return nullptr;
}

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

void FSteamToolsInputModule::RegisterSettings()
{
#if WITH_EDITOR
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "SteamInput", LOCTEXT("SteamInputName", "Steam Input Settings"), LOCTEXT("SteamInputDescription", "Steam Input Settings"), GetMutableDefault<USteamInputSettings>());
    }
#endif
}

void FSteamToolsInputModule::UnregisterSettings()
{
#if WITH_EDITOR
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "SteamInput");
    }
#endif
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamToolsInputModule, SteamToolsInput)