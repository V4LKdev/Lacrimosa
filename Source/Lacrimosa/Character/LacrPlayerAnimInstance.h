// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperZD/Public/PaperZDAnimInstance.h"
#include "LacrPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LACRIMOSA_API ULacrPlayerAnimInstance : public UPaperZDAnimInstance
{
	GENERATED_BODY()
	friend class ALacrCharacterBase;
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector2D GetCharacterDirection() const;

protected:
	virtual void OnInit_Implementation() override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALacrCharacterBase> OwningCharacter;
};