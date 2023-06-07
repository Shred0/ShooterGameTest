// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "Containers/List.h"
#include "ShooterAbilityRewindTime.generated.h"

/**
 * Represents the movement and location in a specified time of player's character
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

	//needed to find and delete traces in the double linked list used to store them to rewind time
	FORCEINLINE bool operator== (FTimeMovementTrace x);
};

FORCEINLINE bool FTimeMovementTrace::operator==(FTimeMovementTrace x)
{
	return Time == x.Time && Location == x.Location && Rotation == x.Rotation;
}

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

	FDateTime TimeStartRewind;
	float TimeRewinded; //same format as delta time (seconds)
	float RewindTimeSpeedMultiplier = 6.f; //how fast i rewind time (1x same velocity as normal time flow)

private:

	//UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect() override;
	virtual int TickEffect(float DeltaTime) override;

	virtual bool PassiveEffectCondition() override;
	virtual void PassiveEffect(float DeltaTime) override;

	FTimerHandle RewindTimeTimer;
	static void RewindTime(UShooterAbilityRewindTime* ability);
};
