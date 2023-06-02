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

	//duration
	bHasDuration = true;
	AbilityDuration = 3.f;

	//tick effect
	HasTickEffect = true;
	bTickReplicate = true;

	//energy management
	/*bUsesEnergy = true;
	MaxEnergy = 100.0f;
	Energy = 100.f;

	bAutoRefills = true;
	RefillRateinTime = 50.f;
	DrainRateinTime = 10.f;*/

	//passive management
	/*HasPassiveEffect = false;
	bPassiveReplicate = false;
	HasPassiveCooldown = false;
	PassiveAbilityCooldown = 0.f;*/

	//HUD
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDShooterAbilityWallRun_Icon"));
	HUDAsset = HUDAssetOb.Object;
	AbilityIcon = UCanvas::MakeIcon(HUDAsset, 0, 0, 90, 90);

	//custom properties
	MaxWallDistance = 0;
}

int UShooterAbilityWallRun::Effect()
{
	bWasAttatchedRight = false;
	bWasAttatchedLeft = false;

	PrevEndPoint = FVector::ZeroVector;
	PrevDirection = FVector::ZeroVector;

	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();
	if (!IsValid(Avatar)) return -1;

	Avatar->GetCharacterMovement()->SetPlaneConstraintEnabled(true);

	if (!MaxWallDistance)
		MaxWallDistance = Avatar->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f;

	StartTickEffect();

	return 0;
}

int UShooterAbilityWallRun::TickEffect(float DeltaTime)
{
	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();

	//i can only run on walls in mid-air
	if (!Avatar->GetMovementComponent()->IsFalling()) {
		StopEffect();
		return -1;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Avatar);

	DrawDebugPoint(GetWorld(), Avatar->GetActorLocation(), 8.0f, FColor::Green, false, 40.0f, 0);
	//i have to stick to a wall from right or left
	FVector rightCheckEnd = (PrevDirection != FVector::ZeroVector && bWasAttatchedRight) ? 
		(PrevDirection * MaxWallDistance) + Avatar->GetActorLocation() : 
		(Avatar->GetActorRightVector() * MaxWallDistance) + Avatar->GetActorLocation();
	FVector leftCheckEnd = (PrevDirection != FVector::ZeroVector && bWasAttatchedLeft) ?
		(PrevDirection * MaxWallDistance) + Avatar->GetActorLocation() :
		(Avatar->GetActorRightVector() * -MaxWallDistance) + Avatar->GetActorLocation();

	//checking right
	FHitResult HitRightRes;
	GetWorld()->LineTraceSingleByChannel(HitRightRes, Avatar->GetActorLocation(), rightCheckEnd, ECollisionChannel::ECC_WorldStatic, CollisionParams);
	//checking left
	FHitResult HitLeftRes;
	GetWorld()->LineTraceSingleByChannel(HitLeftRes, Avatar->GetActorLocation(), leftCheckEnd, ECollisionChannel::ECC_WorldStatic, CollisionParams);

	//direction i should use to check for next attatchment point when turning around a corner
	//valorized when seeking for a valid attatchment point after wall's end/curve
	FVector suggestedDirection = FVector::ZeroVector;

	//if can't find a near wall but i was previously attached to a wall, wall might be ended or curved
	//if so, i try to turn around the corner of the wall
	if (!HitRightRes.bBlockingHit && !HitLeftRes.bBlockingHit && PrevEndPoint != FVector::ZeroVector) {

		if (bWasAttatchedRight) {
			FVector rightCheckEndT = PrevEndPoint;

			//checking right
			GetWorld()->LineTraceSingleByChannel(HitRightRes, rightCheckEnd, rightCheckEndT, ECollisionChannel::ECC_WorldStatic, CollisionParams);
			if (HitRightRes.bBlockingHit) {
				DrawDebugLine(GetWorld(), rightCheckEnd, rightCheckEndT, FColor::Cyan, false, 40.0f, 0, 3.f);
				DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), rightCheckEndT, FColor::Orange, false, 40.0f, 0, 2.5f);
				suggestedDirection = (Avatar->GetActorLocation() - HitRightRes.ImpactPoint).GetSafeNormal();
				DrawDebugDirectionalArrow(GetWorld(), HitRightRes.ImpactPoint, suggestedDirection + HitRightRes.ImpactPoint, 8.f, FColor::Purple, false, 25.f, 0, 4.f);
			}
		}
		else {
			FVector leftCheckEndT = PrevEndPoint;

			//checking left
			GetWorld()->LineTraceSingleByChannel(HitLeftRes, leftCheckEnd, leftCheckEndT, ECollisionChannel::ECC_WorldStatic, CollisionParams);
			if (HitLeftRes.bBlockingHit) {
				DrawDebugLine(GetWorld(), leftCheckEnd, leftCheckEndT, FColor::Cyan, false, 40.0f, 0, 3.f);
				DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), leftCheckEndT, FColor::Orange, false, 40.0f, 0, 2.5f);
				suggestedDirection = (Avatar->GetActorLocation() - HitLeftRes.ImpactPoint).GetSafeNormal();
				DrawDebugDirectionalArrow(GetWorld(), HitLeftRes.ImpactPoint, suggestedDirection + HitLeftRes.ImpactPoint, 5.f, FColor::Purple, false, 25.f, 0, 3.f);
			}
		}
	}

	// if can't find a wall to attach to, detatch from wall without stopping effect in order to look for other walls
	if (!HitRightRes.bBlockingHit && !HitLeftRes.bBlockingHit) {
		//detatch from wall
		Avatar->GetCharacterMovement()->GravityScale = 1.f;
		Avatar->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 0.f));
		return -1;
	}

	//if both direction find a near wall, i use the nearest wall
	if ((HitRightRes.bBlockingHit && HitLeftRes.bBlockingHit)) {
		if (HitRightRes.Distance < HitLeftRes.Distance) {
			HitLeftRes.bBlockingHit = false;
		}
		else if (HitRightRes.Distance > HitLeftRes.Distance)
		{
			HitRightRes.bBlockingHit = false;
		}
	}

	//attatch to wall right to avatar
	if (HitRightRes.bBlockingHit) {
		DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), rightCheckEnd, FColor::Red, false, 40.0f, 0, 2.5f);
		//UKismetMathLibrary::rotation;
		//HitRightRes.Normal.Rotation().Add(0.f, 90.f, 0.f);
		//Avatar->GetActorRotation(). .Yaw = HitRightRes.Normal.Rotation().Yaw
		/*FRotator newRotation = Avatar->GetActorRotation();
		newRotation.Yaw = HitRightRes.Normal.Rotation().Yaw + 90;
		Avatar->SetActorRotation(newRotation);*/ //Controller->ClientSetRotation(HitRightRes.Normal.Rotation().Add(0.f, 90.f, 0.f), false); //
		//Avatar->GetRunningSpeedModifier();
		FRotator newRotation = ((suggestedDirection != FVector::ZeroVector) ? suggestedDirection : HitRightRes.Normal).Rotation();
		newRotation.Yaw += 90;
		FVector newAvatarVelocity = (newRotation.Vector() + HitRightRes.Normal.Rotation().Vector() * -1) * Avatar->GetCharacterMovement()->GetMaxSpeed();
		//DrawDebugDirectionalArrow(GetWorld(), Avatar->GetActorLocation(), newAvatarVelocity + Avatar->GetActorLocation(), 5.f, FColor::Purple, false, 25.f, 0, 3.f);
		newAvatarVelocity.Z = 0.f;
		Avatar->GetCharacterMovement()->Velocity = FVector(newAvatarVelocity);
		Avatar->GetCharacterMovement()->GravityScale = 0.f;
		Avatar->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));

		bWasAttatchedRight = true;
		PrevEndPoint = rightCheckEnd;
		//PrevDirection = (HitRightRes.ImpactPoint - Avatar->GetActorLocation()).GetSafeNormal();
		PrevDirection = HitRightRes.Normal * -1;
	}

	//attatch to wall left to avatar
	if (HitLeftRes.bBlockingHit) {
		DrawDebugLine(GetWorld(), Avatar->GetActorLocation(), leftCheckEnd, FColor::Red, false, 40.0f, 0, 2.5f);
		//UKismetMathLibrary::rotation;
		//HitRightRes.Normal.Rotation().Add(0.f, 90.f, 0.f);
		//Avatar->GetActorRotation(). .Yaw = HitRightRes.Normal.Rotation().Yaw
		/*FRotator newRotation = Avatar->GetActorRotation();
		newRotation.Yaw = HitLeftRes.Normal.Rotation().Yaw - 90;
		Avatar->SetActorRotation(newRotation);*/ //Controller->ClientSetRotation(HitLeftRes.Normal.Rotation().Add(0.f, -90.f, 0.f), false); //
		//Avatar->GetRunningSpeedModifier();
		FRotator newRotation = ((suggestedDirection != FVector::ZeroVector) ? suggestedDirection : HitLeftRes.Normal).Rotation();
		newRotation.Yaw -= 90;
		FVector newAvatarVelocity = (newRotation.Vector() + HitLeftRes.Normal.Rotation().Vector() * -1) * (Avatar->GetCharacterMovement()->GetMaxSpeed());
		//DrawDebugDirectionalArrow(GetWorld(), Avatar->GetActorLocation(), newAvatarVelocity + Avatar->GetActorLocation(), 5.f, FColor::Purple, false, 25.f, 0, 3.f);
		newAvatarVelocity.Z = 0.f;
		Avatar->GetCharacterMovement()->Velocity = FVector(newAvatarVelocity);
		Avatar->GetCharacterMovement()->GravityScale = 0.f;
		Avatar->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));

		bWasAttatchedLeft = true;
		PrevEndPoint = leftCheckEnd;
		//PrevDirection = (HitLeftRes.ImpactPoint - Avatar->GetActorLocation()).GetSafeNormal();
		PrevDirection = HitLeftRes.Normal * -1;
	}

	return 0;
}

void UShooterAbilityWallRun::AfterEffect()
{
	StopTickEffect();

	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();
	//detatch from wall
	Avatar->GetCharacterMovement()->GravityScale = 1.f;
	Avatar->GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 0.f));
	//GetAbilitySystem()->GetShooterAvatar()->GetCharacterMovement()->SetPlaneConstraintEnabled(false);

	//Avatar->StopJumping();
	//Avatar->LaunchCharacter(, true, true);
}
