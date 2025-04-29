// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueData.generated.h"


USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	FDialogueLine() 
		: CharacterName(FText::GetEmpty())
		, NameColor(FLinearColor::White)
		, Text("")
		, WriteSpeed(0.05f){}

	UPROPERTY(EditAnywhere, Category = DialogueData)
	FText CharacterName;
	UPROPERTY(EditAnywhere, Category = DialogueData)
	FSlateColor NameColor;
	UPROPERTY(EditAnywhere, Category = DialogueData)
	FString Text;
	UPROPERTY(EditAnywhere, Category = DialogueData)
	float WriteSpeed;
};

UCLASS(BlueprintType)
class LACRIMOSA_API UDialogueData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bLoopDialogue = false;
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bLockPlayerMovement = false;
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bAutoProgress = false;
	UPROPERTY(EditAnywhere, Category = Settings)
	float AutoProgressTime = 1.0f;
	UPROPERTY(EditAnywhere, Category = Dialogue)
	TArray<FDialogueLine> DialogueLines;
};
