// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Lacrimosa/Data/DialogueData.h"
#include "DialogueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWritingFinished);

UCLASS(Abstract)
class LACRIMOSA_API UDialogueWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void FadeVisibility(bool bNewVisibility);
	UFUNCTION(BlueprintCallable)
	void DisplayText(const FDialogueLine& InDialogueLine);

	UPROPERTY(BlueprintAssignable)
	FOnWritingFinished OnWritingFinished;
	
protected:
	void WriteText();

	UFUNCTION(BlueprintImplementableEvent)
	void SetDialogueText(const FText& InText = FText::GetEmpty());
	UFUNCTION(BlueprintImplementableEvent)
	void SetCharacterName(const FText& InName = FText::GetEmpty(), const FSlateColor& InColor = FSlateColor(FLinearColor::White));
	
	bool bShouldBeVisible = false;
	FString TextToWrite;
	FString CurrentText;
	int32 TextIndex = 0;
	FTimerHandle TextDisplayHandle;
};
