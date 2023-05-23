// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "ShooterAbilityJetpack.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityJetpack : public UShooterAbility
{
	GENERATED_BODY()
	
public:

	UShooterAbilityJetpack();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//jetpack force
	UPROPERTY(Transient, EditdefaultsOnly, Category = "Ability|Attribute")
	float JetpackForce;

	//jetpack max velocity
	UPROPERTY(Transient, EditdefaultsOnly, Category = "Ability|Attribute")
	float JetpackMaxVelocity;

	//avatar air control before ability activation
	float PrevAirControl;

	/** sound played when jetpack thrusting */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* AbilitySound;
	UAudioComponent* AbilitySoundAC;

private:

	//UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect() override;

	virtual int TickEffect(float DeltaTime) override;

	virtual void AfterEffect() override;
};
