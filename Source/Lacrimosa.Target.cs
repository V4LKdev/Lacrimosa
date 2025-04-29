// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LacrimosaTarget : TargetRules
{
	public LacrimosaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Lacrimosa" } );
	}
}
