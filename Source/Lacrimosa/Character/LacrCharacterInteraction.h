// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "LacrCharacterBase.h"
#include "LacrCharacterInteraction.generated.h"


class UGameHUD;
class USphereComponent;
class UDialogueData;

UCLASS()
class LACRIMOSA_API ALacrCharacterInteraction : public ALacrCharacterBase
{
	GENERATED_BODY()

public:
	ALacrCharacterInteraction();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetNewDialogue(AActor* InDialogueInstigator, UDialogueData* DialogueData);

	virtual void InputInteract(const FInputActionValue& Value) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Interaction */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDetectionRadius = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 100.f;

	TArray<TObjectPtr<AActor>> OverlappingInteractables;
	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;
	
	
	/** Dialogue */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSubclassOf<UUserWidget> GameHUDClass;
	UPROPERTY()
	TObjectPtr<UGameHUD> GameHUD;

	bool bHasActiveDialogue = false;
	bool bIsLastText = false;
	bool bDisableInteractableAfterDialogue = false;
	int32 DialogueIndex = 0;
	
	FTimerHandle DialogueAutoProgressTimerHandle;

	UPROPERTY()
	TObjectPtr<AActor> DialogueInstigator;
	UPROPERTY()
	TObjectPtr<UDialogueData> CurrentDialogueData;

private:
	UFUNCTION()
	void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateInteractables();

	UFUNCTION()
	void OnTextFinishedWriting();
	void ProgressActiveDialogue();
	void FinishDialogue();
};
