// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/Abilities/ShooterAbilityWallJump.h"

UShooterAbilityWallJump::UShooterAbilityWallJump() :Super()
{
	///defaults
	//AbilitySystem = nullptr;
	AbilityID = EShooterAbilityID::ShooterAbilityWallJump;
	AbilityName = FName(TEXT("Wall Jump"));
	AbilityCooldown = 0.f;

	//HUD
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDShooterAbilityWallJump_Icon"));
	HUDAsset = HUDAssetOb.Object;
	AbilityIcon = UCanvas::MakeIcon(HUDAsset, 0, 0, 90, 90);

	//custom properties
	MaxWallDistance = 0.f;
}

int UShooterAbilityWallJump::Effect()
{
	AShooterCharacter* Avatar = GetAbilitySystem()->GetShooterAvatar();

	if (!MaxWallDistance) MaxWallDistance = Avatar->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f;

	//i can only jump on walls in mid-air
	if (!Avatar->GetMovementComponent()->IsFalling()) {
		StopEffect();
		return -1;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Avatar);

	DrawDebugPoint(GetWorld(), Avatar->GetActorLocation(), 8.f, FColor::Green, false, 40.f, 0);
	//i have to jump from a wall from right or left
	FVector rightCheckEnd = (Avatar->GetActorRightVector() * MaxWallDistance) + Avatar->GetActorLocation();
	FVector leftCheckEnd = (Avatar->GetActorRightVector() * -MaxWallDistance) + Avatar->GetActorLocation();

	//check right
	FHitResult HitRightRes;
	GetWorld()->LineTraceSingleByChannel(HitRightRes, Avatar->GetActorLocation(), rightCheckEnd, ECollisionChannel::ECC_WorldStatic, CollisionParams);
	//check left
	FHitResult HitLeftRes;
	GetWorld()->LineTraceSingleByChannel(HitLeftRes, Avatar->GetActorLocation(), leftCheckEnd, ECollisionChannel::ECC_WorldStatic, CollisionParams);

	//if i can't find a near wall i can't jump
	if (!HitRightRes.bBlockingHit && !HitLeftRes.bBlockingHit) {
		return -1;
	}

	if (HitRightRes.bBlockingHit && HitLeftRes.bBlockingHit) {
		if (HitRightRes.Distance < HitLeftRes.Distance) {
			HitLeftRes.bBlockingHit = false;
		}
		else if (HitRightRes.Distance > HitLeftRes.Distance) {
			HitRightRes.bBlockingHit = false;
		}
	}

	//jump from wall right to avatar
	if (HitRightRes.bBlockingHit) {
		DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), rightCheckEnd, FColor::Red, false, 40.f, 0, 2.5f);
		FRotator DirectionT;
		FVector newAvatarVelocity;

		//newAvatarVelocity += HitRightRes.Normal;

		DirectionT = HitRightRes.Normal.Rotation();
		DirectionT.Yaw += 75;
		DirectionT.Pitch += 45;
		newAvatarVelocity = DirectionT.Vector();
		//newAvatarVelocity.Normalize();

		/*DirectionT = HitRightRes.Normal.Rotation();
		DirectionT.Pitch += 90;
		newAvatarVelocity += DirectionT.Vector();
		newAvatarVelocity.Normalize();*/

		newAvatarVelocity *= Avatar->GetCharacterMovement()->GetMaxSpeed();
		DrawDebugDirectionalArrow(GetWorld(), Avatar->GetActorLocation(), newAvatarVelocity + Avatar->GetActorLocation(), 8.f, FColor::Purple, false, 15.f, 0, 4.f);

		Avatar->LaunchCharacter(newAvatarVelocity, true, true);
	}

	//jump from wall left to avatar
	if (HitLeftRes.bBlockingHit) {
		DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), leftCheckEnd, FColor::Red, false, 40.f, 0, 2.5f);
		FRotator DirectionT;
		FVector newAvatarVelocity;

		//newAvatarVelocity += HitLeftRes.Normal;

		DirectionT = HitLeftRes.Normal.Rotation();
		DirectionT.Yaw -= 75;
		DirectionT.Pitch += 45;
		newAvatarVelocity = DirectionT.Vector();
		//newAvatarVelocity.Normalize();

		/*DirectionT = HitLeftRes.Normal.Rotation();
		DirectionT.Pitch += 90;
		newAvatarVelocity += DirectionT.Vector();
		newAvatarVelocity.Normalize();*/

		newAvatarVelocity *= Avatar->GetCharacterMovement()->GetMaxSpeed();
		DrawDebugDirectionalArrow(GetWorld(), Avatar->GetActorLocation(), newAvatarVelocity + Avatar->GetActorLocation(), 8.f, FColor::Purple, false, 15.f, 0, 4.f);

		Avatar->LaunchCharacter(newAvatarVelocity, true, true);
	}

	StopEffect();
	return 0;
}