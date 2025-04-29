// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LacrimosaEditorTarget : TargetRules
{
	public LacrimosaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Lacrimosa" } );
	}
}
