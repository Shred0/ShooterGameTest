 // Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Misc/DateTime.h"
#include "Player/Abilities/ShooterAbilityRewindTime.h"

FTimeMovementTrace::FTimeMovementTrace()
{
}
FTimeMovementTrace::FTimeMovementTrace(FDateTime _Time, FVector _Location, FRotator _Rotation)
{
	Time = _Time;
	Location = _Location;
	Rotation = _Rotation;
}
FTimeMovementTrace::~FTimeMovementTrace()
{
}

UShooterAbilityRewindTime::UShooterAbilityRewindTime():Super()
{
	///defaults
	//AbilitySystem = nullptr;
	AbilityID = EShooterAbilityID::ShooterAbilityRewindTime;
	AbilityName = FName(TEXT("Rewind Time"));
	AbilityCooldown = 5.f;

	//tick effect
	HasTickEffect = true;
	bTickReplicate = false;

	//passive management
	HasPassiveEffect = true;
	bPassiveReplicate = false;
	HasPassiveCooldown = false;
	PassiveAbilityCooldown = 0.f;

	//HUD
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDShooterAbilityRewindTime_Icon"));
	HUDAsset = HUDAssetOb.Object;
	AbilityIcon = UCanvas::MakeIcon(HUDAsset, 0, 0, 90, 90);

	//sound
	static ConstructorHelpers::FObjectFinder<USoundCue> AbilitySoundOb(TEXT("/Game/Sounds/Abilities/SCue_Ability_RewindTime"));
	AbilitySound = AbilitySoundOb.Object;

	//custom properties
	TimeToRewind = 3.f; //3 seconds
	IsRewindingTime = false;
}

int UShooterAbilityRewindTime::Effect()
{
	AShooterCharacter* Avatar = GetAbilitySystem()->GetShooterAvatar();
	if (!IsValid(Avatar)) {
		return -1;
	}

	UShooterCharacterMovement* CharacterMovement = Cast<UShooterCharacterMovement>(Avatar->GetMovementComponent());
	if (!IsValid(CharacterMovement)) {
		return -1;
	}

	FTimerManager* TimerManager = &GetWorld()->GetTimerManager();
	if (!TimerManager) {
		return -1;
	}

	//block inout from player
	//AActor::DisableInput(Cast<APlayerController>(Avatar->GetController()));

	//FHitResult HitResult;

	//bool CanRepeat = true;
	//int i = MovementTrace.Num() - 1;
	//RewindTimeIndex = MovementTrace.Num() - 1;
	//_sleep(30000);

	/*TArray<FTimeMovementTrace> MovementTraceT = MovementTrace;
	UWorld* WorldT = World;
	FTimerHandle Timer;*/

	//ACCES VIOLATION FOR TIMER
	/*FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&MovementTraceT, &Avatar, &i, &TimerManager, &Timer]()
	{
		if (!(i >= 0)) {
			FHitResult HitResult;
			Avatar->GetMovementComponent()->SafeMoveUpdatedComponent(MovementTraceT[i].Location, MovementTraceT[i].Rotation, true, HitResult, ETeleportType::ResetPhysics);
			i--;
		}

		//exit condition do while-like
		if (i >= 0) TimerManager->ClearTimer(Timer);
	});*/

	//World->GetTimerManager().SetTimer(Timer, [&]() {TimerDelegate.ExecuteIfBound(); }, 0.05f, CanRepeat);
	//TimerManager->SetTimer(Timer, [&TimerDelegate]() {TimerDelegate.ExecuteIfBound(); }, 0.05f,);
	IsRewindingTime = true;	

	//avatar FXs
	Avatar->DisableInput(Cast<APlayerController>(Avatar->GetController()));
	//Avatar->GetMesh()->SetVisibility(false, true);
	SetActorVisibility(Avatar, false);

	//FXs
	PlaySound(AbilitySound, Avatar->GetActorLocation());

	UParticleSystem* RewindTimeFromParticleFX = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Muzzle/P_Launcher_MF.P_Launcher_MF"));
	PlayParticle(RewindTimeFromParticleFX, Avatar->GetActorLocation(), Avatar->GetActorRotation());

	//rewinding!
	//TimerManager->SetTimer(RewindTimeTimer, [&]() {this->RewindTime(this);}, 0.0035f, true);
	StartTickEffect();

	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("X:%f Y:%f Z:%f"),
		MovementTrace.Last().Location.X, MovementTrace.Last().Location.Y, MovementTrace.Last().Location.Z));*/

	/*for (int i = MovementTrace.Num() - 1; i >= 0; i--) {
		CharacterMovement->SafeMoveUpdatedComponent(MovementTrace[i].Location, MovementTrace[i].Rotation, true, HitResult, ETeleportType::ResetPhysics);
		//_sleep(30000);
	}*/

	//enable inout from player

	return 0;
}

int UShooterAbilityRewindTime::TickEffect(float DeltaTime)
{
	/*if (GetWorld()->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Tick Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Tick Ability in Client");
	}*/
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Rewinding!")));
	if (MovementTrace.Num() > 0) {
		AShooterCharacter* Avatar = GetAbilitySystem()->GetShooterAvatar();
		UPawnMovementComponent* MovementComponent = Avatar->GetMovementComponent();
		FTimeMovementTrace Trace = MovementTrace.GetTail()->GetValue();
		//FVector ActorLocation = Avatar->GetActorLocation();

		/*float valX = ActorLocation.X - Trace.Location.X;
		float valY = ActorLocation.Y - Trace.Location.Y;
		float valZ = ActorLocation.Z - Trace.Location.Z;
		//FVector val = ActorLocation - ability->MovementTrace.Last().Location;
		Avatar->AddMovementInput(FVector::XAxisVector, valX, true);
		Avatar->AddMovementInput(FVector::YAxisVector, valY, true);
		Avatar->AddMovementInput(FVector::ZAxisVector, valZ, true);*/

		/*FRepMovement Movement = FRepMovement();
		Movement.Location = Trace.Location;
		Movement.Rotation = Trace.Rotation;
		Avatar->GetPawnPhysicsVolume()->SetReplicatedMovement(Movement);*/

		/*FVector Direction = Trace.Location - Avatar->GetActorLocation();
		Avatar->AddMovementInput(Direction, (Avatar->GetActorLocation() - Trace.Location).Size(), true);*/

		/*FHitResult HitResult;
		MovementComponent->SafeMoveUpdatedComponent(Trace.Location, Trace.Rotation, true, HitResult, ETeleportType::ResetPhysics);*/

		Avatar->TeleportTo(Trace.Location, Trace.Rotation);
		Avatar->GetController()->ClientSetRotation(Trace.Rotation, true);

		MovementTrace.RemoveNode(MovementTrace.GetTail());
	}

	//exit condition do while-like
	if (MovementTrace.Num() == 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Reset timer");
		//ability->GetWorld()->GetTimerManager().ClearTimer(ability->RewindTimeTimer);
		StopTickEffect();

		AShooterCharacter* Avatar = GetAbilitySystem()->GetShooterAvatar();

		//Avatar->GetMesh()->SetVisibility(true, true);
		SetActorVisibility(Avatar, true);

		UParticleSystem* RewindTimeToParticleFX = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Impact/P_Launcher_IH.P_Launcher_IH"));
		PlayParticle(RewindTimeToParticleFX, Avatar->GetActorLocation(), Avatar->GetActorRotation());

		Avatar->EnableInput(Cast<APlayerController>(Avatar->GetController()));

		IsRewindingTime = false;
	}

	return 0;
}

bool UShooterAbilityRewindTime::PassiveEffectCondition()
{
	return (!GetIsPlaying() || !IsRewindingTime);
}

void UShooterAbilityRewindTime::PassiveEffect(float DeltaTime)
{
	//memorizing movements in a range from now to <TimeToRewind> seconds ago
	UShooterAbilitySystem* AS = GetAbilitySystem();
	if (!IsValid(AS)) return;

	AShooterCharacter* Avatar = GetAbilitySystem()->GetShooterAvatar();
	if (Avatar) {
		//adding timestamp with location to trace record
		FTimeMovementTrace Trace = FTimeMovementTrace(FDateTime::UtcNow(), Avatar->GetActorLocation(), Avatar->GetActorRotation());
		//MovementTrace.Emplace(FTimeMovementTrace(FDateTime::UtcNow(), Avatar->GetActorLocation(), Avatar->GetActorRotation()));
		MovementTrace.AddTail(Trace);

		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("X:%f Y:%f Z:%f"),
			MovementTrace.Last().Location.X, MovementTrace.Last().Location.Y, MovementTrace.Last().Location.Z));*/

		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("X:%f Y:%f Z:%f"),
			MovementTrace.Last().Rotation.Roll, MovementTrace.Last().Rotation.Pitch, MovementTrace.Last().Rotation.Yaw));*/

		//removing traces over <TimeToRewind> seconds ago
		FTimespan timeSpan;
		while (timeSpan.GetTotalSeconds() > TimeToRewind) {
			timeSpan = MovementTrace.GetTail()->GetValue().Time - MovementTrace.GetHead()->GetValue().Time;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("%f"), timeSpan.GetTotalSeconds()));
			if (timeSpan.GetTotalSeconds() > TimeToRewind) {
				MovementTrace.RemoveNode(MovementTrace.GetHead());
			}
		}
	}
}

void UShooterAbilityRewindTime::RewindTime(UShooterAbilityRewindTime* ability)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Rewinding!")));
	if (ability->MovementTrace.Num() > 0) {
		AShooterCharacter* Avatar = ability->GetAbilitySystem()->GetShooterAvatar();
		UPawnMovementComponent* MovementComponent = Avatar->GetMovementComponent();
		FTimeMovementTrace Trace = ability->MovementTrace.GetTail()->GetValue();
		//FVector ActorLocation = Avatar->GetActorLocation();

		/*float valX = ActorLocation.X - Trace.Location.X;
		float valY = ActorLocation.Y - Trace.Location.Y;
		float valZ = ActorLocation.Z - Trace.Location.Z;
		//FVector val = ActorLocation - ability->MovementTrace.Last().Location;
		Avatar->AddMovementInput(FVector::XAxisVector, valX, true);
		Avatar->AddMovementInput(FVector::YAxisVector, valY, true);
		Avatar->AddMovementInput(FVector::ZAxisVector, valZ, true);*/

		/*FRepMovement Movement = FRepMovement();
		Movement.Location = Trace.Location;
		Movement.Rotation = Trace.Rotation;
		Avatar->GetPawnPhysicsVolume()->SetReplicatedMovement(Movement);*/

		/*FVector Direction = Trace.Location - Avatar->GetActorLocation();
		Avatar->AddMovementInput(Direction, (Avatar->GetActorLocation() - Trace.Location).Size(), true);*/

		/*FHitResult HitResult;
		MovementComponent->SafeMoveUpdatedComponent(Trace.Location, Trace.Rotation, true, HitResult, ETeleportType::ResetPhysics);*/

		Avatar->TeleportTo(Trace.Location, Trace.Rotation);
		Avatar->GetController()->ClientSetRotation(Trace.Rotation, true);

		ability->MovementTrace.RemoveNode(ability->MovementTrace.GetTail());
	}

	//exit condition do while-like
	if (ability->MovementTrace.Num() == 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Reset timer");
		ability->GetWorld()->GetTimerManager().ClearTimer(ability->RewindTimeTimer);

		AShooterCharacter* Avatar = ability->GetAbilitySystem()->GetShooterAvatar();

		//Avatar->GetMesh()->SetVisibility(true, true);
		ability->SetActorVisibility(Avatar, true);

		UParticleSystem* RewindTimeToParticleFX = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Impact/P_Launcher_IH.P_Launcher_IH"));
		ability->PlayParticle(RewindTimeToParticleFX, Avatar->GetActorLocation(), Avatar->GetActorRotation());

		Avatar->EnableInput(Cast<APlayerController>(Avatar->GetController()));

		ability->IsRewindingTime = false;
	}
}