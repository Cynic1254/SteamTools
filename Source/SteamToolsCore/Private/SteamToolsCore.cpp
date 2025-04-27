#include "SteamToolsCore.h"

#define LOCTEXT_NAMESPACE "FSteamToolsCoreModule"

void FSteamToolsCoreModule::StartupModule()
{
    APIHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
}

void FSteamToolsCoreModule::ShutdownModule()
{
    APIHandle.Reset();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSteamToolsCoreModule, SteamToolsCore)