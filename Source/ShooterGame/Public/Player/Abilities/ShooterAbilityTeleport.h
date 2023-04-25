// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "ShooterAbilityTeleport.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityTeleport : public UShooterAbility
{
	GENERATED_BODY()
	
public:

	UShooterAbilityTeleport();

protected:

	//teleport distance
	UPROPERTY(Transient, EditdefaultsOnly, Category = "Ability|Attribute")
	float TeleportDistance;

	/** sound played when teleported */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* AbilitySoundTeleport;

private:

	//UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	virtual int Effect() override;
};
