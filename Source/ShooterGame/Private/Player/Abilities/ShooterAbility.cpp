// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "ShooterAbilitySystem.h"
#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"

UShooterAbility::UShooterAbility()
{
	//the ability must have a working tick component
	//in order to manage its effects (energy, duration and tick effect)

	//ReplicateSubobjects(,);
	SetIsReplicatedByDefault(true);
	bTickReplicate = true;
	//bRegistered = true;
	SetIsReplicated(true);
	PrimaryComponentTick.Target = this;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
	SetComponentTickEnabled(true);
	//RegisterComponent();
	//PrimaryComponentTick.RegisterTickFunction(GetComponentLevel());

	//all defaults
	/*AbilityID = EShooterAbilityID::ShooterAbility;
	AbilityName = FName(TEXT("None"));
	IsInCooldown = false;
	AbilityCooldown = 0.f;
	IsPlaying = false;
	IsEffectActive = false;

	//duration
	bHasDuration = true;
	AbilityDuration = 0.f;

	//tick effect
	HasTickEffect = false;
	bTickReplicate = false;

	//energy management
	bUsesEnergy = false;
	MaxEnergy = 100.0f;
	Energy = 0.f;

	bAutoRefills = true;
	RefillRateinTime = 25.f;
	DrainRateinTime = 15.f;

	//passive management
	HasPassiveEffect = false;
	bPassiveReplicate = false;
	HasPassiveCooldown = false;
	PassiveAbilityCooldown = 0.f;*/

	//HUD

	//custom properties
}

// Called when the game starts or when spawned
void UShooterAbility::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UShooterAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Ability is Ticking");
	if (GetWorld()->IsServer()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Ability Ticks in Server");
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Ability Ticks in Client");
	}

	//passive effect management
	if (GetHasPassiveEffect() /*&& ability->PassiveEffectCondition()*/ && !GetIsPassiveInCooldown()) {
		PlayPassiveEffect(DeltaTime);
	}

	//energy management
	if (UsesEnergy()) {
		//drain abilities
		if (GetIsPlaying()) {
			//using energy from ability's pool based on elapsed time
			float energy = (GetDrainRateInTime() / 1) * DeltaTime; //refill rate is in seconds, like delta time
			UseEnergy(energy);
			//stop ability when energy reaches zero
			if (GetEnergy() == 0) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Ability Energy Depleted");
				StopTickEffect();
				StopEffect();
			}
		}

		//refill abilities
		if (!GetIsPlaying() && AutoRefills() && AutoRefillCondition()) {
			if (GetEnergy() < GetMaxEnergy()) {
				//adding energy on ability's pool based on elapsed time
				float energy = (GetRefillRateInTime() / 1) * DeltaTime; //refill rate is in seconds, like delta time
				AddEnergy(energy);
			}
		}
	}

	//tick effect management
	if (HasTickEffect) {
		if (IsEffectActive && (!bHasDuration || (bHasDuration && AbilityTimeLeft > 0.f))) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Ability should tick");
			PlayTickEffect(DeltaTime);
		}
	}
	
	//duration management
	if (HasDuration() && GetIsPlaying()) {
		AbilityTimeLeft -= DeltaTime;
		if (GetWorld()->IsClient()) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Purple, FString::Printf(TEXT("Ability Time Left: %f"), AbilityTimeLeft));
		//stop effect if ability duration depletes
		if (AbilityTimeLeft <= 0.f) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Ability Duration Depleted");
			StopTickEffect();
			StopEffect();
		}
	}
}

void UShooterAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UShooterAbility* UShooterAbility::MakeFor(UShooterAbilitySystem* SAS, EShooterAbilityID ID)
{
	UShooterAbility* abilityReference = nullptr;
	if (SAS->IsAbilityValid(ID)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ID is valid");
		//check if ability ID is valid
		UClass* abilityClass = *SAS->AbilityClassMap.Find(ID);
		if (abilityClass) {
			abilityReference = NewObject<UShooterAbility>(SAS, abilityClass);
			//abilityReference->SetIsReplicated(true);
			abilityReference->RegisterComponent();
			//abilityReference->PrimaryComponentTick.bCanEverTick = true;
			abilityReference->AbilitySystem = SAS;
			//abilityReference->World = SAS->World;
			abilityReference->PrimaryComponentTick.RegisterTickFunction(SAS->GetComponentLevel());
		}
	}

	return abilityReference;
}

/*UShooterAbility::~UShooterAbility()
{
}*/

UShooterAbilitySystem* UShooterAbility::GetAbilitySystem()
{
	return AbilitySystem;
}

EShooterAbilityID UShooterAbility::GetID()
{
	return AbilityID;
}

FString UShooterAbility::GetName()
{
	return (AbilityName.IsValid()) ? AbilityName.ToString() : "Invalid Name";
}

bool UShooterAbility::GetIsInCooldown()
{
	return IsInCooldown;
}
void UShooterAbility::SetIsInCooldown(bool bInCoolDown)
{
	IsInCooldown = bInCoolDown;
}

bool UShooterAbility::GetIsPassiveInCooldown()
{
	return IsPassiveInCooldown;
}
void UShooterAbility::SetIsPassiveInCooldown(bool bInCoolDown)
{
	IsPassiveInCooldown = bInCoolDown;
}

float UShooterAbility::GetCooldown()
{
	return AbilityCooldown;
}

bool UShooterAbility::GetHasPassiveCooldown()
{
	return HasPassiveCooldown;
}

float UShooterAbility::GetPassiveCooldown()
{
	return PassiveAbilityCooldown;
}

FTimerHandle UShooterAbility::GetCooldownTimer()
{
	return AbilityCooldownTimer;
}

FTimerHandle UShooterAbility::GetPassiveCooldownTimer()
{
	return PassiveAbilityCooldownTimer;
}

bool UShooterAbility::GetIsPlaying()
{
	return IsPlaying;
}

bool UShooterAbility::HasDuration()
{
	return bHasDuration;
}

float UShooterAbility::GetDuration()
{
	return AbilityDuration;
}

bool UShooterAbility::GetIsEffectActive()
{
	return IsEffectActive;
}

bool UShooterAbility::GetHasPassiveEffect()
{
	return HasPassiveEffect;
}

bool UShooterAbility::DoesPassiveReplicate()
{
	return bPassiveReplicate;
}

bool UShooterAbility::UsesEnergy()
{
	return bUsesEnergy;
}

float UShooterAbility::GetMaxEnergy()
{
	return MaxEnergy;
}

float UShooterAbility::GetEnergy()
{
	return Energy;
}

void UShooterAbility::SetEnergy(float Energy)
{
	//energy can't be lower than zero and higher than max
	this->Energy = FMath::Clamp(Energy, 0.f, MaxEnergy);
}

void UShooterAbility::AddEnergy(float Energy)
{
	//cant add more energy than the remaining amount of energy to reach max
	this->Energy += FMath::Clamp(Energy, 0.f, MaxEnergy - this->Energy);
}

void UShooterAbility::UseEnergy(float Energy)
{
	//can't use more energy than the current amount
	this->Energy -= FMath::Clamp(Energy, 0.f, this->Energy);
}

bool UShooterAbility::AutoRefills()
{
	return bAutoRefills;
}

float UShooterAbility::GetRefillRateInTime()
{
	return RefillRateinTime;
}

float UShooterAbility::GetDrainRateInTime()
{
	return DrainRateinTime;
}

bool UShooterAbility::AutoRefillCondition()
{
	return true;
}

void UShooterAbility::CooldownStart()
{
	if (GetWorld()) {
		GetWorld()->GetTimerManager().SetTimer(AbilityCooldownTimer, this, &UShooterAbility::CooldownReset, AbilityCooldown, false);
		IsInCooldown = true;
	}
}
void UShooterAbility::CooldownReset()
{
	IsInCooldown = false;
}

void UShooterAbility::PassiveCooldownStart()
{
	if (GetWorld()) {
		GetWorld()->GetTimerManager().SetTimer(PassiveAbilityCooldownTimer, this, &UShooterAbility::PassiveCooldownReset, PassiveAbilityCooldown, false);
		IsPassiveInCooldown = true;
	}
}
void UShooterAbility::PassiveCooldownReset()
{
	IsPassiveInCooldown = false;
}

bool UShooterAbility::PlayEffect()
{
	if (GetWorld()->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Ability in Client");
	}

	//generic error case
	int result = -1;

	//if is not in cooldown and can drain energy
	if (!IsInCooldown && (XOR(!UsesEnergy(), DrainRateinTime <= Energy))) {
		IsPlaying = true;
		result = Effect();
		//IsPlaying = false;
		AbilityTimeLeft = AbilityDuration;
	}

	bool successfullyPlayed = (result == 0) ? true : false;

	/*if (successfullyPlayed && AbilityCooldown > 0 && !HasTickEffect) {
		CooldownStart();
	}*/

	if (!successfullyPlayed) IsPlaying = false;

	return successfullyPlayed;
}

void UShooterAbility::StopEffect()
{
	if (IsPlaying == true) {
		if (GetWorld()->IsServer()) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Ability in Server");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Ability in Client");
		}

		AfterEffect();

		IsPlaying = false;

		if (AbilityCooldown > 0 /*&& HasTickEffect*/) {
			CooldownStart();
		}
	}
}

bool UShooterAbility::PlayTickEffect(float DeltaTime)
{
	/*if (GetWorld()->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Tick Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Playing Tick Ability in Client");
	}*/

	//generic error case
	int result = -1;

	//if is not in cooldown and can drain energy
	//if (IsInCooldown) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "IsInCooldown");
	if (HasTickEffect && IsEffectActive && !IsInCooldown && (XOR(!UsesEnergy(), DrainRateinTime <= Energy))) {
		result = TickEffect(DeltaTime);
	}

	bool successfullyPlayed = (result == 0) ? true : false;

	/*if (successfullyPlayed && AbilityCooldown > 0) {
		CooldownStart();
	}*/

	return successfullyPlayed;
}

void UShooterAbility::StartTickEffect()
{
	if (IsPlaying == true) {
		if (GetWorld()->IsServer()) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Starting Tick Ability in Server");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Starting Tick Ability in Client");
		}

		IsEffectActive = true;
	}
}
void UShooterAbility::StopTickEffect()
{
	if (IsPlaying == true) {
		if (GetWorld()->IsServer()) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Tick Ability in Server");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Stopping Tick Ability in Client");
		}

		IsEffectActive = false;

		if (HasTickEffect && AbilityCooldown > 0) {
			CooldownStart();
		}
	}
}

void UShooterAbility::PlayPassiveEffect(float DeltaTime)
{
	/*if (GetWorld()->IsServer()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Tick Ability in Server");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Tick Ability in Client");
	}*/

	if (GetHasPassiveEffect() && PassiveEffectCondition() && !GetIsPassiveInCooldown()) {
		if (GetOwnerRole() < ROLE_Authority && DoesPassiveReplicate()) {
			ServerPlayPassiveEffect(DeltaTime);
		}
		PassiveEffect(DeltaTime);
		if (GetHasPassiveCooldown() && PassiveAbilityCooldown > 0) {
			PassiveCooldownStart();
		}
	}
}
void UShooterAbility::ServerPlayPassiveEffect_Implementation(float DeltaTime)
{
	PlayPassiveEffect(DeltaTime);
}

///
//FXs
///
void UShooterAbility::PlaySound(USoundCue* Sound, FVector Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Sound");
	if (GetOwnerRole() < ROLE_Authority) {
		//ServerPlaySound(Sound, Location);
		MulticastSound(Sound, Location);
	}
	UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
}
void UShooterAbility::ServerPlaySound_Implementation(USoundCue* Sound, FVector Location)
{
	MulticastSound(Sound, Location);
}
void UShooterAbility::MulticastSound_Implementation(USoundCue* Sound, FVector Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Sound");
	//only on proxies
	if (!GetOwner()->HasLocalNetOwner()) {
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
		//UGameplayStatics::SpawnSoundAttached(Sound, SCOwner->GetRootComponent());
	}
}

void UShooterAbility::PlayAudioComponent(UAudioComponent* AudioComponent, float StartTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Audio Component");
	if (GetOwnerRole() < ROLE_Authority) {
		//ServerPlayAudioComponent(AudioComponent, StartTime);
		MulticastAudioComponent(AudioComponent, StartTime);
	}
	if (AudioComponent) {
		AudioComponent->Play(StartTime);
	}
}
void UShooterAbility::ServerPlayAudioComponent_Implementation(UAudioComponent* AudioComponent, float StartTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Server Playing Ability Audio Component");
	MulticastAudioComponent(AudioComponent, StartTime);
}
void UShooterAbility::MulticastAudioComponent_Implementation(UAudioComponent* AudioComponent, float StartTime)
{
	if (!GetOwner()->HasLocalNetOwner()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Audio Component");
		if (AudioComponent) {
			AudioComponent->Play(StartTime);
		}
	}
}

void UShooterAbility::StopAudioComponent(UAudioComponent* AudioComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Stopping Ability Audio Component");
	if (GetOwnerRole() < ROLE_Authority) {
		//ServerStopAudioComponent(AudioComponent);
		MulticastStopAudioComponent(AudioComponent);
	}
	if (AudioComponent) {
		AudioComponent->Stop();
	}
}
void UShooterAbility::ServerStopAudioComponent_Implementation(UAudioComponent* AudioComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Server Stopping Ability Audio Component");
	MulticastStopAudioComponent(AudioComponent);
}
void UShooterAbility::MulticastStopAudioComponent_Implementation(UAudioComponent* AudioComponent)
{
	if (!GetOwner()->HasLocalNetOwner()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Stop Ability Audio Component");
		if (AudioComponent) {
			AudioComponent->Stop();
		}
	}
}

void UShooterAbility::PlayParticle(UParticleSystem * FX, FVector Location, FRotator Rotation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Particle");
	if (GetOwnerRole() < ROLE_Authority) {
		ServerPlayParticle(FX, Location, Rotation);
		//MulticastParticle(FX, Location, Rotation);
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, FX, Location, Rotation);
}
void UShooterAbility::ServerPlayParticle_Implementation(UParticleSystem* FX, FVector Location, FRotator Rotation)
{
	MulticastParticle(FX, Location, Rotation);
}
void UShooterAbility::MulticastParticle_Implementation(UParticleSystem* FX, FVector Location, FRotator Rotation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Particle");
	//only on proxies
	if (!GetOwner()->HasLocalNetOwner()) {
		UGameplayStatics::SpawnEmitterAtLocation(this, FX, Location, Rotation);
	}
}

void UShooterAbility::SetActorVisibility(AActor* Actor, bool bVisible)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Setting Actor Visibility");
	if (!IsValid(Actor)) {
		return;
	}
	if (GetOwnerRole() < ROLE_Authority) {
		//ServerSetActorVisibility(Actor, bVisible);
		MulticastActorVisibility(Actor, bVisible);
	}
	Actor->SetActorHiddenInGame(!bVisible);
	TArray<AActor*> children = Actor->Children;
	//Actor->GetAllChildActors(children, true);
	for (AActor* a : children) {
		a->SetActorHiddenInGame(!bVisible);
	}
}
void UShooterAbility::ServerSetActorVisibility_Implementation(AActor* Actor, bool bVisible)
{
	MulticastActorVisibility(Actor, bVisible);
}
void UShooterAbility::MulticastActorVisibility_Implementation(AActor* Actor, bool bVisible)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Actor Visibility");
	//only on proxies
	if (!GetOwner()->HasLocalNetOwner()) {
		Actor->SetActorHiddenInGame(!bVisible);
		//SetActorVisibility(Actor, bVisible);
	}
}

int UShooterAbility::Effect()
{
	return 0;
}

int UShooterAbility::TickEffect(float DeltaTime)
{
	return 0;
}

void UShooterAbility::AfterEffect()
{
}

bool UShooterAbility::PassiveEffectCondition()
{
	return true;
}

void UShooterAbility::PassiveEffect(float DeltaTime)
{
}
