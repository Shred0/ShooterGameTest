// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Abilities/ShooterAbility.h"
#include "ShooterAbilityWallJump.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityWallJump : public UShooterAbility
{
	GENERATED_BODY()

public:

	UShooterAbilityWallJump();

protected:

	float MaxWallDistance;

private:

	virtual int Effect() override;
};
