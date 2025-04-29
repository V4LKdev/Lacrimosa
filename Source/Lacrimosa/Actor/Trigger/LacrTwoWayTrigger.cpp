// Copyright (c) 2025 Nicolas Martin. All Rights Reserved.


#include "LacrTwoWayTrigger.h"

#include "Components/ArrowComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameModeBase.h"

ALacrTwoWayTrigger::ALacrTwoWayTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowPositive = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowPositive"));
	ArrowPositive->SetupAttachment(RootComponent);
	ArrowPositive->ArrowColor = FColor::Green;
	ArrowPositive->ArrowSize = 2.0f;
	ArrowPositive->ArrowLength = 50.0f;
	ArrowPositive->SetRelativeRotation(FRotator(0, 90, 0));
	ArrowPositive->SetUsingAbsoluteScale(true);

	ArrowNegative = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowNegative"));
	ArrowNegative->SetupAttachment(RootComponent);
	ArrowNegative->ArrowColor = FColor::Red;
	ArrowNegative->ArrowSize = 2.0f;
	ArrowNegative->ArrowLength = 50.0f;
	ArrowNegative->SetRelativeRotation(FRotator(0, -90, 0));
	ArrowNegative->SetUsingAbsoluteScale(true);

	GetCollisionComponent()->SetCollisionProfileName("Trigger");
	GetCollisionComponent()->SetLineThickness(1.f);
}

void ALacrTwoWayTrigger::BeginPlay()
{
	Super::BeginPlay();
	check(GetCollisionComponent());

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

void ALacrTwoWayTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherComp || ActorEntryQueue.Contains(OtherActor) || Cast<USphereComponent>(OtherComp))
	{
		return;
	}

	// Generically check if the actor is the player pawn
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
	if (!GameMode || OtherActor->GetClass() != GameMode->DefaultPawnClass)
	{
		return;
	}

	const FVector ActorLocationWorld = OtherActor->GetActorLocation();
	const FVector ActorLocationLocal = GetCollisionComponent()->GetComponentTransform().InverseTransformPosition(ActorLocationWorld);
	const ETriggerSide EntrySide = (ActorLocationLocal.Y > 0) ? Positive : Negative;

	UE_LOG(LogTemp, Log, TEXT("Actor %s entered on side %s"), *GetNameSafe(OtherActor), *UEnum::GetValueAsString(EntrySide));

	// Store entry side for the actor
	ActorEntryQueue.Add(OtherActor, EntrySide);
}

void ALacrTwoWayTrigger::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!ActorEntryQueue.Contains(OtherActor))
		return;

	// Get the actor's current location in local space
	const FVector ActorLocationWorld = OtherActor->GetActorLocation();
	const FVector ActorLocationLocal = GetCollisionComponent()->GetComponentTransform().InverseTransformPosition(ActorLocationWorld);
	const ETriggerSide ExitSide = (ActorLocationLocal.Y > 0) ? Positive : Negative;

	UE_LOG(LogTemp, Log, TEXT("Actor %s exited on side %s"), *GetNameSafe(OtherActor), *UEnum::GetValueAsString(ExitSide));
	
	const ETriggerSide EntrySide = ActorEntryQueue[OtherActor];
	ActorEntryQueue.Remove(OtherActor);
	
	if (ExitSide != EntrySide)
	{
		Internal_TriggerSide(ExitSide); // Player exited opposite side
	}
}

void ALacrTwoWayTrigger::Internal_TriggerSide(ETriggerSide TriggerSide)
{
	switch (TriggerSide)
	{
	case Positive:
		{
			OnPositiveExit();
				
			break;
		}
	case Negative:
		{
			OnNegativeExit();
				
			break;
		}
	default:
		break;
	}
}
