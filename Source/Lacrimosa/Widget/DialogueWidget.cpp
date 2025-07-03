// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "DialogueWidget.h"

#include "Kismet/KismetMathLibrary.h"

void UDialogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const float CurrentOpacity = GetRenderOpacity();
	const float TargetOpacity = bShouldBeVisible ? 1.0f : 0.0f;
	
	if (CurrentOpacity != TargetOpacity)
	{
		const float InterpOpacity = UKismetMathLibrary::FInterpTo_Constant(
			CurrentOpacity,
			TargetOpacity,
			InDeltaTime,
			5.0f
			);

		SetRenderOpacity(InterpOpacity);
		if (CurrentOpacity <= 0.f || CurrentOpacity >= 1.f)
		{
			SetRenderOpacity(TargetOpacity);
		}
	}
}

void UDialogueWidget::FadeVisibility(const bool bNewVisibility)
{
	bShouldBeVisible = bNewVisibility;
	if (bShouldBeVisible)
	{
		SetDialogueText();
	}
}

void UDialogueWidget::DisplayText(const FDialogueLine& InDialogueLine)
{
	SetCharacterName(InDialogueLine.CharacterName, InDialogueLine.NameColor);

	TextToWrite = InDialogueLine.Text;
	CurrentText = TEXT("");
	SetDialogueText();
	TextIndex = 0;

	const float ClampedTypeSpeed = FMath::Max(InDialogueLine.WriteSpeed, 0.01f);
	
	// Set timer to call WriteText every TypeSpeed seconds
	GetWorld()->GetTimerManager().SetTimer(
		TextDisplayHandle,
		this,
		&ThisClass::WriteText,
		ClampedTypeSpeed,
		true
	);
}

void UDialogueWidget::WriteText()
{
	const auto& CharArray = TextToWrite.GetCharArray();

	CurrentText.AppendChar(CharArray[TextIndex]);

	SetDialogueText(FText::FromString(CurrentText));
	TextIndex++;

	if (TextIndex >= CharArray.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(TextDisplayHandle);
		OnWritingFinished.Broadcast();
	}
}
