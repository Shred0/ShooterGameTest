// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "AssetRegistryModule.h"
#include "ShooterAbility.h"
#include "Player/Abilities/ShooterAbility.h"
#include "Player/Abilities/ShooterAbilitySystem.h"

// Sets default values
AShooterAbilitySystem::AShooterAbilitySystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}
AShooterAbilitySystem::AShooterAbilitySystem(AShooterCharacter* Owner)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->ShooterOwner = Owner;
}

// Called every frame
void AShooterAbilitySystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AShooterCharacter * AShooterAbilitySystem::GetShooterOwner()
{
	return ShooterOwner;
}

bool AShooterAbilitySystem::AddAbility(UShooterAbilityID ID)
{
	//creating a reference to the ability i want to use
	//int32 intID = static_cast<int32>(ID);
	UShooterAbility* abilityReference;
	{
		const FString abilityName = TEXT("" + UEnum::GetValueAsString(ID));
		const TCHAR* abilityNameChar = *abilityName;
		abilityReference = FindObject<UShooterAbility>(this->GetOutermost(), abilityNameChar, true);
	}

	//adding ability reference to ability list
	//bool successfullyAdded = false;
	if (abilityReference) {
		AbilityMap.Add(ID, abilityReference);
		//successfullyAdded = true;
	}

	//return (successfullyAdded) ? true : false;
	return (AbilityMap.Find(ID)) ? true : false;

	//return true;
}

bool AShooterAbilitySystem::PlayAbility(UShooterAbilityID ID)
{
	bool successfullyPlayed = false;

	UShooterAbility* ability = *AbilityMap.Find(ID);

	if (ability) {
		successfullyPlayed = ability->PlayEffect();
	}

	return successfullyPlayed;
}

TArray<UShooterAbilityID> AShooterAbilitySystem::GetAllAbilities()
{
	TArray<UShooterAbilityID> abilityList;
	UEnum* ids = StaticEnum<UShooterAbilityID>();
	int nAbilities = ids->NumEnums();

	for (int32 i = 0; i < nAbilities; i++) {
		abilityList.Add(static_cast<UShooterAbilityID>(i));
	}

	return abilityList;
}

TArray<UShooterAbilityID> AShooterAbilitySystem::GetEquippedAbilities()
{
	TArray<UShooterAbilityID> abilityList;
	AbilityMap.GenerateKeyArray(abilityList);

	return abilityList;
}

bool AShooterAbilitySystem::IsAbilityEquipped(UShooterAbilityID ID)
{
	return AbilityMap.Contains(ID);
}

// Called when the game starts or when spawned
void AShooterAbilitySystem::BeginPlay()
{
	Super::BeginPlay();
	
	/*FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	const UClass* Class = UShooterAbility::StaticClass();
	AssetRegistryModule.Get().GetAssetsByClass(Class->GetFName(), AssetData);*/
}

