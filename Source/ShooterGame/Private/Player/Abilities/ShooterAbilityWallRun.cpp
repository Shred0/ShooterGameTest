// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Kismet/KismetMathLibrary.h"
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
	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();

	//i can only run on walls in mid-air
	if (!Avatar->GetMovementComponent()->IsFalling()) {
		return -1;
	}

	//i have to stick to a wall from right or left
	float maxDistance = 30;
	FVector rightCheckEnd = (Avatar->GetActorRightVector() * maxDistance) + Avatar->GetActorLocation();
	FVector leftCheckEnd = (Avatar->GetActorRightVector() * -maxDistance) + Avatar->GetActorLocation();
	//checking right
	FHitResult HitRightRes;
	GetWorld()->LineTraceSingleByChannel(HitRightRes, Avatar->GetActorLocation(), rightCheckEnd, ECollisionChannel::ECC_WorldStatic);
	//checking left
	FHitResult HitLeftRes;
	GetWorld()->LineTraceSingleByChannel(HitLeftRes, Avatar->GetActorLocation(), leftCheckEnd, ECollisionChannel::ECC_WorldStatic);

	if (!HitRightRes.bBlockingHit && !HitLeftRes.bBlockingHit) {
		return -1;
	}

	if (HitRightRes.bBlockingHit) {
		//UKismetMathLibrary::rotation;
		HitRightRes.Normal.Rotation();
		//Avatar->GetActorRotation(). .Yaw = HitRightRes.Normal.Rotation().Yaw
		//Avatar->SetActorRotation(HitRightRes.Normal.Rotation().Yaw);
	}

	return 0;
}

int UShooterAbilityWallRun::TickEffect(float DeltaTime)
{
	return 0;
}

void UShooterAbilityWallRun::AfterEffect()
{
}
