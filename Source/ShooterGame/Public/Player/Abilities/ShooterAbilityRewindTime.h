// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "Containers/List.h"
#include "ShooterAbilityRewindTime.generated.h"

/**
 * Represents the appearance of an SChatWidget
 */
USTRUCT()
struct SHOOTERGAME_API FTimeMovementTrace
{
	GENERATED_USTRUCT_BODY()

public:
	FTimeMovementTrace();
	FTimeMovementTrace(FDateTime Time, FVector Location, FRotator Rotation);
	virtual ~FTimeMovementTrace();

	FDateTime Time;

	FVector Location;

	FRotator Rotation;

	//FVector LinearVelocity;
	//FVector AngularVelocity;
};

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityRewindTime : public UShooterAbility
{
	GENERATED_BODY()
	
public:

	UShooterAbilityRewindTime();

protected:

	//seconds to rewind
	UPROPERTY(Transient, EditdefaultsOnly, Category = "Ability|Attribute")
	float TimeToRewind;

	//movement trace passively stored
	//FDateTime for Timestamp of movement
	//FVector for direction for AddMovementInput of CharacterMovement component
	TDoubleLinkedList<FTimeMovementTrace> MovementTrace;

	/** sound played when time rewinded */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* AbilitySound;

	bool IsRewindingTime;

private:

	//UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect() override;

	virtual bool PassiveEffectCondition() override;
	virtual void PassiveEffect(float DeltaTime) override;

	FTimerHandle RewindTimeTimer;
	static void RewindTime(UShooterAbilityRewindTime* ability);
};