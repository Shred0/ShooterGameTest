// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/Abilities/ShooterAbilityJetpack.h"

UShooterAbilityJetpack::UShooterAbilityJetpack():Super()
{
	//PrimaryComponentTick.bCanEverTick = true;
	AbilityID = EShooterAbilityID::ShooterAbilityJetpack;
	AbilityName = FName(TEXT("Jetpack"));

	//tick effect
	HasTickEffect = true;
	bTickReplicate = true;

	//energy management
	bUsesEnergy = true;
	//MaxEnergy = 100.0f;
	Energy = MaxEnergy;

	bAutoRefills = true;
	RefillRateinTime = 27.5f;
	DrainRateinTime = 27.5f;

	//HUD
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDShooterAbilityJetpack_Icon"));
	HUDAsset = HUDAssetOb.Object;
	AbilityIcon = UCanvas::MakeIcon(HUDAsset, 0, 0, 90, 90);

	//sound
	AbilitySound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Sounds/Abilities/SCue_ShooterAbilityJetpack_LP.SCue_ShooterAbilityJetpack_LP"));
	///ACCESS VIOLATION
	//AbilitySoundAC = UGameplayStatics::SpawnSoundAttached(Cast<USoundBase>(AbilitySound), AbilitySystem->GetOwner()->GetRootComponent());
	AbilitySoundAC = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	if (AbilitySound) {
		AbilitySoundAC->SetSound(AbilitySound);
		AbilitySoundAC->bAutoDestroy = false;
	}

	//custom properties
	JetpackForce = 350000.f; //about
	JetpackMaxVelocity = 350.f;
}

void UShooterAbilityJetpack::BeginPlay()
{
	Super::BeginPlay();
	///ACCESS VIOLATION
	//AbilitySoundAC = UGameplayStatics::SpawnSoundAttached(Cast<USoundBase>(AbilitySound), AbilitySystem->GetOwner()->GetRootComponent());
	/*if (AbilitySoundAC)
		AbilitySoundAC->AttachTo(AbilitySystem->GetShooterAvatar()->GetRootComponent());*/
	//Cast<UShooterAbilitySystem>(GetOwner());
}

int UShooterAbilityJetpack::Effect()
{
	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();
	//Avatar->GetMovementComponent()->Velocity = FVector(0,0,100000);
	//Avatar->GetCharacterMovement()->AddForce(FVector(0, 0, 10000));
	//Avatar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	//Avatar->ClientCheatFly

	if (Avatar->JumpCurrentCount != Avatar->JumpMaxCount) return -1;

	//IsEffectActive = true;
	StartTickEffect();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Avatar->GetCharacterMovement()->AirControl));

	PrevAirControl = Avatar->GetCharacterMovement()->AirControl;
	Avatar->GetCharacterMovement()->AirControl = 1.f;

	/*if (AbilitySound) {
		if (AbilitySoundAC) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Audio Component already instantiated!");
			AbilitySoundAC->Play();
			//AbilitySoundAC.sound
		}
		else {
			AbilitySoundAC = UGameplayStatics::SpawnSoundAttached(AbilitySound, Avatar->GetRootComponent());
			if (AbilitySoundAC) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Audio Component instantiated!");
				AbilitySoundAC->bAutoDestroy = false;
			}
		}
	}*/

	if (AbilitySoundAC) {
		if (!AbilitySoundAC->IsAttachedTo(AbilitySystem->GetShooterAvatar()->GetRootComponent()))
			AbilitySoundAC->AttachTo(AbilitySystem->GetShooterAvatar()->GetRootComponent());
		//AbilitySoundAC->Play();
		PlayAudioComponent(AbilitySoundAC);
	}

	return 0;
}

int UShooterAbilityJetpack::TickEffect(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Purple, "Jetpack Ability is Ticking");
	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();
	//Avatar->AddMovementInput(FVector::UpVector, 1.f, true);
	//Avatar->GetCharacterMovement()->AddForce(FVector(0, 0, 10000));

	float AvatarMass = Avatar->GetCharacterMovement()->Mass;
	float Gravity = Avatar->GetCharacterMovement()->GetGravityZ();

	float multiplier = 1.f;

	float acceleration = JetpackForce / AvatarMass;
	acceleration *= multiplier;

	float surplusAccel = FMath::Max<float>(0, acceleration + Gravity);
	float desiredSurplusAccel = surplusAccel * 1; //desiredThrottle: all trhottle
	float desiredAccel = (Gravity * -1) + desiredSurplusAccel;

	float desiredVelocity = Avatar->GetMovementComponent()->Velocity.Z + (desiredAccel * DeltaTime);
	float velocityLimitFromGravity = JetpackMaxVelocity + (Gravity * DeltaTime * -1);

	float resultingAccel = 0.f;

	if (Avatar->GetMovementComponent()->Velocity.Z > velocityLimitFromGravity) {
		resultingAccel = 0.f;
	}
	else {
		if (desiredVelocity > velocityLimitFromGravity) {
			float eccessVelocity = desiredVelocity - velocityLimitFromGravity;
			resultingAccel = FMath::Clamp<float>(desiredAccel - (eccessVelocity / DeltaTime), 0, desiredAccel);
		}
		else {
			resultingAccel = desiredAccel;
		}
	}

	Avatar->GetMovementComponent()->Velocity.Z += resultingAccel * DeltaTime;

	return 0;
}

void UShooterAbilityJetpack::AfterEffect()
{
	AShooterCharacter* Avatar = AbilitySystem->GetShooterAvatar();
	//Avatar->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	Avatar->GetCharacterMovement()->AirControl = PrevAirControl;
	if (AbilitySoundAC) {
		//AbilitySoundAC->Stop();
		StopAudioComponent(AbilitySoundAC);
	}

	//IsEffectActive = false;
	StopTickEffect();
}