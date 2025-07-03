// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

using UnrealBuildTool;

public class Lacrimosa : ModuleRules
{
	public Lacrimosa(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SlateCore",
			"Slate",
			"UMG",
			"CommonUI",
			"Paper2D",
			"PaperZD", 
			"AIModule",
			"CameraStateSystem",
			"NavigationSystem"
		});

	}
}
