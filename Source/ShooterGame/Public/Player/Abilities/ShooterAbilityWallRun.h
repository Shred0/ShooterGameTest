// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "ShooterAbilityWallRun.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityWallRun : public UShooterAbility
{
	GENERATED_BODY()

public:

	UShooterAbilityWallRun();
	
protected:

private:

	virtual int Effect() override;
	virtual int TickEffect(float DeltaTime) override;
	virtual void AfterEffect() override;
};
