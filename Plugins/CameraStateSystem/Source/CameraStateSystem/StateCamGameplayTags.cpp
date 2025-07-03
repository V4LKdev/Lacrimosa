// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#include "StateCamGameplayTags.h"

namespace StateCamGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CameraState_Follow, "CameraState.Follow", "Follows the player along one axis");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CameraState_Static, "CameraState.Static", "Static camera that does not follow the player");
	
	UE_DEFINE_GAMEPLAY_TAG(CameraState_2D, "CameraState.2D");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CameraState_2D_Static, "CameraState.2D.Static", "Static camera that does not follow the player, but is 2D");
	
}