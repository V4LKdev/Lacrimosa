// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUI/Public/CommonUserWidget.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LACRIMOSA_API UGameHUD : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UDialogueWidget> DialogueWidget;
};
