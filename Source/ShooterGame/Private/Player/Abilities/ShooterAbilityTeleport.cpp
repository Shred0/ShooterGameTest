// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/Abilities/ShooterAbilityTeleport.h"

UShooterAbilityTeleport::UShooterAbilityTeleport():Super()
{
	///defaults
	//AbilitySystem = nullptr;
	AbilityID = EShooterAbilityID::ShooterAbilityTeleport;
	AbilityName = FName(TEXT("Teleport"));
	AbilityCooldown = 1.5f;

	//HUD
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDAddedAsset"));
	HUDAsset = HUDAssetOb.Object;
	AbilityIcon = UCanvas::MakeIcon(HUDAsset, 901, 285, 90, 95);

	//sound
	static ConstructorHelpers::FObjectFinder<USoundCue> AbilitySoundOb(TEXT("/Game/Sounds/Abilities/SCue_Ability_Teleport"));
	AbilitySoundTeleport = AbilitySoundOb.Object;

	//custom properties
	TeleportDistance = 1000.f; //about 10 meters
}

int UShooterAbilityTeleport::Effect()
{
	if (GetWorld()->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Teleport Effect in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Teleport Effect in Client");
	}

	bool bTeleported = false;

	AShooterCharacter* SCOwner = AbilitySystem->GetShooterAvatar();

	if (SCOwner) {
		FVector CLocation = SCOwner->GetActorLocation();

		FRotator CRotation = SCOwner->GetActorRotation();
		FVector CDirection = CRotation.Vector();

		FVector TargetLocation = CLocation + (CDirection * TeleportDistance);

		//check target location validity

		//obtain set of traces created within the base teleport distance to predict the best target location
		FCollisionQueryParams TraceParams(FName(TEXT("TeleportTrace")), true, SCOwner);
		FCollisionShape TraceShape = FCollisionShape::MakeSphere(TeleportDistance);
		TraceParams.bTraceComplex = true;
		TraceParams.AddIgnoredActor(SCOwner);
		//TArray<FHitResult> HitResults;
		FHitResult HitResult;
		//GetWorld()->SweepMultiByObjectType(HitResults, CLocation, TargetLocation, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, TraceShape, TraceParams);

		//find best position through set of traces
		FVector BestLocation = TargetLocation;
		float BestDistance = TeleportDistance;

		//if my target position is available i can teleport there
		UE_LOG(LogTemp, Log, TEXT("Target location: X=%f, Y=%f, Z=%f"), TargetLocation.X, TargetLocation.Y, TargetLocation.Z);

		{
			// Apply the pivot offset to the desired location
			FVector Offset = SCOwner->GetRootComponent()->Bounds.Origin - CLocation;
			BestLocation = BestLocation + Offset;

			// check if able to find an acceptable destination for this actor that doesn't embed it in world geometry
			bTeleported = GetWorld()->FindTeleportSpot(SCOwner, BestLocation, CRotation);
			BestLocation = BestLocation - Offset;

			UE_LOG(LogTemp, Log, TEXT("Best location: X=%f, Y=%f, Z=%f"), BestLocation.X, BestLocation.Y, BestLocation.Z);
		}

		if (BestLocation.ContainsNaN()) {
			UE_LOG(LogActor, Log, TEXT("Attempted to teleport to NaN"));

			return 0;
		}

		//FHitResult res;
		bTeleported = SCOwner->GetMovementComponent()->SafeMoveUpdatedComponent(BestLocation, CRotation, true, HitResult, ETeleportType::TeleportPhysics);
		
		//check teleport validity
		if (bTeleported) {
			float TeleportedDistance = (SCOwner->GetActorLocation() - BestLocation).Size();
			if (TeleportedDistance > TeleportDistance) {
				SCOwner->GetMovementComponent()->SafeMoveUpdatedComponent(CLocation, CRotation, true, HitResult, ETeleportType::TeleportPhysics);
				bTeleported = false;
			}
		}

		if (!SCOwner->TeleportTo(TargetLocation, CRotation) && !bTeleported) {
			DrawDebugPoint(GetWorld(), TargetLocation, 8.0f, FColor::White, false, 40.0f, 0);
			//ActorGetDistanceToCollision(TargetLocation, ECC_WorldStatic, BestLocation); //retirns nearest point to this character from TargetLocation

			///solution 1
			//setting up navigation system to find the nearest location for shooting character outside of a collision shape
			/*UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			FNavLocation ProjectedLocation;
			if (NavSys->ProjectPointToNavigation(TargetLocation, ProjectedLocation)) {
				//play particle effect and sound
				//...

				SetActorLocation(ProjectedLocation.Location);
			}
			else {
				//SetActorLocation(TargetLocation);
			}*/

			///solution 2
			// Check for collisions in all directions in a sphere
			/*int NumDirections = 256;
			//float TurnFraction = 0.618033; //golden ratio
			//float TurnFraction = 0.1;
			float TurnFraction = (1 + FMath::Sqrt(5)) / 2; //Golden Ratio
			//float Angle;
			float Inclination;
			float Azimuth;
			FVector EndLocation;
			//FVector Direction;
			float DistanceToResult;
			for (int i = 0; i < NumDirections; i++) {
				// Calculate the direction to check in
				//Angle = (360.0f / NumDirections) * i;

				//Inclination = PI / 2 - FMath::DegreesToRadians(Angle);
				//Azimuth = FMath::DegreesToRadians(Angle);

				float t = i / (NumDirections - 1.0f);
				Inclination = FMath::Acos(1 - 2 * t);
				Azimuth = 2.0f * PI * TurnFraction * i;

				//calculate end location of the sphere trace
				EndLocation.X = FMath::Sin(Inclination) * FMath::Cos(Azimuth);
				EndLocation.Y = FMath::Sin(Inclination) * FMath::Sin(Azimuth);
				EndLocation.Z = FMath::Cos(Inclination);

				EndLocation = EndLocation * (TeleportDistance) + TargetLocation;

				//DEBUG
				//UE_LOG(LogTemp, Log, TEXT("%f, %f, %f"), EndLocation.X, EndLocation.Y, EndLocation.Z);
				//Direction = FRotator(0.0f, Angle, 0.0f).Vector();
				//DrawDebugLine(GetWorld(), TargetLocation, EndLocation, FColor::Red, false, 40.0f, 0, 2.5f);
				DrawDebugPoint(GetWorld(), EndLocation, 8.0f, FColor::Red, false, 40.0f, 0);

				//perform the sphere trace
				//GetWorld()->SweepSingleByChannel(HitResult, TargetLocation, EndLocation, FQuat::Identity, ECC_WorldDynamic, TraceShape, TraceParams);
				GetWorld()->SweepMultiByChannel(HitResults, TargetLocation, EndLocation, FQuat::Identity, ECC_Visibility, TraceShape, TraceParams);

				//find best valid distance based on original distance
				for (const FHitResult& Result : HitResults) {
					if (Result.bBlockingHit) {
						DistanceToResult = (Result.Location - TargetLocation).Size();

						///se distanza è 0 memorizzo elemento colpito

						///prendo in considerazione punto più distante impattato con elemento colpito

						if (DistanceToResult > 0 && DistanceToResult < BestDistance) {
							UE_LOG(LogTemp, Log, TEXT("%f"), DistanceToResult);
							BestLocation = Result.Location;
							BestDistance = DistanceToResult;
						}
					}
					DrawDebugLine(GetWorld(), TargetLocation, BestLocation, FColor::Blue, false, 40.0f, 0, 2.5f);
					DrawDebugPoint(GetWorld(), BestLocation, 8.0f, FColor::Blue, false, 40.0f, 0);
				}
			}*/

			///solution 3

			///controllo se in linea di visione ho ostacoli, mi fermo alla prima collisione o fino a distanza di target
			///e imposto valore ottenuto in best location

			///controllo se dal punto raggiunto, puntando verso il basso ci sono collisioni, mi fermo all'altezza del
			///target iniziale o alla prima collsione riscotrata

			//ottengo punto e direzione di vista
			FVector TEyePos;
			SCOwner->GetActorEyesViewPoint(TEyePos, CRotation);
			BestLocation.Z = TEyePos.Z;
			//controllo collisioni fino a proiezione di target
			DrawDebugPoint(GetWorld(), TEyePos, 8.0f, FColor::Red, false, 40.0f, 0);
			DrawDebugPoint(GetWorld(), BestLocation, 8.0f, FColor::Blue, false, 40.0f, 0);
			GetWorld()->LineTraceSingleByChannel(HitResult, TEyePos, BestLocation, ECC_WorldStatic, TraceParams);
			//se intercetto una collisione, sovrascrivo best location e nuova coordinata target
			if (HitResult.bBlockingHit) {
				BestLocation = HitResult.Location;
				DrawDebugPoint(GetWorld(), BestLocation, 8.0f, FColor::Green, false, 40.0f, 0);
				TargetLocation.X = BestLocation.X;
				TargetLocation.Y = BestLocation.Y;
				DrawDebugPoint(GetWorld(), TargetLocation, 8.0f, FColor::Cyan, false, 40.0f, 0);
			}
			//controllo collisioni in basso verso target
			DrawDebugLine(GetWorld(), TEyePos, BestLocation, FColor::Red, false, 40.0f, 0, 2.5f);
			GetWorld()->LineTraceSingleByChannel(HitResult, BestLocation, TargetLocation, ECC_WorldStatic, TraceParams);
			BestLocation = TargetLocation;
			//aggiorno best location
			if (HitResult.bBlockingHit) {
				BestLocation = HitResult.Location;
				DrawDebugPoint(GetWorld(), BestLocation, 8.0f, FColor::Green, false, 40.0f, 0);
			}
			TEyePos.X = BestLocation.X;
			TEyePos.Y = BestLocation.Y;
			DrawDebugLine(GetWorld(), TEyePos, BestLocation, FColor::Red, false, 40.0f, 0, 2.5f);

			//effettuo il teletrasporto
			UE_LOG(LogTemp, Log, TEXT("Best location: X=%f, Y=%f, Z=%f"), BestLocation.X, BestLocation.Y, BestLocation.Z);
			//SCOwner->SetActorLocation(BestLocation, true, nullptr, ETeleportType::TeleportPhysics);
			if (SCOwner->TeleportTo(BestLocation, CRotation)) {
				bTeleported = true;
			}
		}
		else {
			bTeleported = true;
		}

		//ability FXs
		if (bTeleported) {
			UParticleSystem* TeleportFromParticleFX = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Muzzle/P_Launcher_MF.P_Launcher_MF"));
			UGameplayStatics::SpawnEmitterAtLocation(SCOwner, TeleportFromParticleFX, CLocation, CRotation);

			//AbilitySoundTeleport = LoadObject<USoundCue>(nullptr, TEXT("/Game/Sounds/Abilities/SCue_Ability_Teleport.SCue_Ability_Teleport"));
			PlaySound(AbilitySoundTeleport, BestLocation);
			UParticleSystem* TeleportToParticleFX = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Impact/P_Launcher_IH.P_Launcher_IH"));
			//static ConstructorHelpers::FObjectFinder<UParticleSystem> TeleportParticleFXOb(TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Impact/P_Launcher_IH.P_Launcher_IH"));
			//UParticleSystem* TeleportParticleFX = TeleportParticleFXOb.Object;
			PlayParticle(TeleportToParticleFX, BestLocation, CRotation);
		}
	}

	StopEffect();
	return 0;
}
