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

	//max distance i from character location where i can detect nearby walls
	float MaxWallDistance;

	//variables needed to know wich wall i run on and to detect a wall change or a turn
	bool bWasAttatchedRight;
	bool bWasAttatchedLeft;
	FVector PrevEndPoint;
	FVector PrevDirection;

private:

	virtual int Effect() override;
	virtual int TickEffect(float DeltaTime) override;
	virtual void AfterEffect() override;
};
