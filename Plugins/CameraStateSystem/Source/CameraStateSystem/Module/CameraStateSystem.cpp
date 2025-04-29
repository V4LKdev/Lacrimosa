// Copyright Epic Games, Inc. All Rights Reserved.

#include "CameraStateSystem.h"

#define LOCTEXT_NAMESPACE "FCameraStateSystemModule"

DEFINE_LOG_CATEGORY(LogCameraStateSystem);

void FCameraStateSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogCameraStateSystem, Log, TEXT("CameraStateSystem module started"));
}

void FCameraStateSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCameraStateSystemModule, CameraStateSystem)