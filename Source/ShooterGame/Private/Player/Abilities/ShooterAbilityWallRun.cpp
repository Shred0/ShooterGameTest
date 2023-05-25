// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/Abilities/ShooterAbilityWallRun.h"

UShooterAbilityWallRun::UShooterAbilityWallRun():Super()
{
	//all defaults
	AbilityID = EShooterAbilityID::ShooterAbilityWallRun;
	AbilityName = FName(TEXT("Wall Run"));
	//AbilityCooldown = 0.f;
	//AbilityDuration = 0.f;

	//tick effect
	HasTickEffect = true;
	bTickReplicate = true;

	//energy management
	bUsesEnergy = true;
	MaxEnergy = 100.0f;
	Energy = 100.f;

	bAutoRefills = true;
	RefillRateinTime = 50.f;
	DrainRateinTime = 10.f;

	//passive management
	/*HasPassiveEffect = false;
	bPassiveReplicate = false;
	HasPassiveCooldown = false;
	PassiveAbilityCooldown = 0.f;*/

	//HUD

	//custom properties
}

int UShooterAbilityWallRun::Effect()
{
	return 0;
}

int UShooterAbilityWallRun::TickEffect(float DeltaTime)
{
	return 0;
}

void UShooterAbilityWallRun::AfterEffect()
{
}
