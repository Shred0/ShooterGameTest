// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "AssetRegistryModule.h"
#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"
#include "Player/Abilities/ShooterAbilitySystem.h"

// Sets default values
UShooterAbilitySystem::UShooterAbilitySystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;
	PrimaryComponentTick.bCanEverTick = false;
	//bReplicates = true;
}
//UShooterAbilitySystem* UShooterAbilitySystem::MakeFor(AShooterCharacter* Owner)
void UShooterAbilitySystem::SetFor(AActor* PlayerState, AShooterCharacter* Avatar)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Setting Ability System");

	ActorOwner = PlayerState;
	ShooterAvatar = Avatar;
	//World = GEngine->GetWorldFromContextObject(Avatar, EGetWorldErrorMode::LogAndReturnNull);
	World = GetWorld();

	//adding class references for all abilities
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt) {
		UClass* ClassAbility = *ClassIt;
		//ClassAbility->GetClass();
		const UEnum* enumID = FindObject<UEnum>(ANY_PACKAGE, TEXT("EShooterAbilityID"), true);
		if (ClassAbility->IsChildOf(UShooterAbility::StaticClass()) && !ClassIt->HasAnyClassFlags(CLASS_Abstract) && enumID)
		{
			FString name = ClassAbility->GetName();
			UE_LOG(LogTemp, Log, TEXT("Class Name: %s"), *name);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Fetching C++ class " + name);
			//EShooterAbilityID AbilityID = static_cast<EShooterAbilityID>(enumID->GetValueByName(FName(TEXT(""+ClassAbility->GetName()))));
			//int64 index = enumID->GetValueByName(FName(TEXT("" + ClassAbility->GetName())));
			//int64 index = enumID->GetIndexByNameString(name);
			//EShooterAbilityID AbilityID = static_cast<EShooterAbilityID>((uint8)enumID->GetIndexByNameString(name));
			/*if (enumID->GetValueByNameString(FString("None")) == INDEX_NONE) {
				UE_LOG(LogTemp, Warning, TEXT("SAD"));
			}*/
			//EShooterAbilityID AbilityID = static_cast<EShooterAbilityID>((uint8)enumID->GetIndexByValue(enumID->GetValueByNameString(name)));
			/*if (AbilityID == EShooterAbilityID::ShooterAbility) {
				UE_LOG(LogTemp, Warning, TEXT("Class Name = ID"));
			}*/
			//enumID->GetValueByIndex(enumID->GetIndexByNameString(name));
			//UE_LOG(LogTemp, Warning, TEXT("Class ID: %s"), ""+index);
			//for (EShooterAbilityID AbilityID : TEnumRange<EShooterAbilityID>()){
			for (int32 idx = 0; idx < enumID->NumEnums(); ++idx) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Checking Ability class " + enumID->GetDisplayNameTextByIndex((uint8)idx).ToString());
				if (enumID->GetDisplayNameTextByIndex((uint8)idx).ToString().Equals(ClassAbility->GetName())) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Adding Ability class " + name);
					UE_LOG(LogTemp, Log, TEXT("Class ID: %s"), *enumID->GetDisplayNameTextByIndex((uint8)idx).ToString());
					EShooterAbilityID AbilityID = static_cast<EShooterAbilityID>(idx);
					AbilityClassMap.Add(AbilityID, ClassAbility);
					break;
				}
			}
		}
	}
}

// Called every frame
/*void UShooterAbilitySystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/
void UShooterAbilitySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UShooterAbilitySystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UShooterAbilitySystem, AbilityMap);
}

AActor * UShooterAbilitySystem::GetActorOwner()
{
	return ActorOwner;
}

AShooterCharacter * UShooterAbilitySystem::GetShooterAvatar()
{
	return ShooterAvatar;
}

bool UShooterAbilitySystem::AddAbility(EShooterAbilityID ID)
{
	//creating a reference to the ability i want to use
	bool equipped = false;
	UEnum* enumID = FindObject<UEnum>(ANY_PACKAGE, TEXT("EShooterAbilityID"), true);
	if (enumID) {
		FString wrn = "Validating Ability with ID " + enumID->GetDisplayNameTextByIndex((uint8)ID).ToString();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, wrn);
		if (IsAbilityValid(ID)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ID is valid");
			UE_LOG(LogTemp, Warning, TEXT("adding ability with id: %s"), *enumID->GetDisplayNameTextByIndex((uint8)ID).ToString());
			UShooterAbility* abilityReference = nullptr;
			if (IsAbilityEquipped(ID)) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Ability already equipped");
				abilityReference = *AbilityMap.Find(ID);
			}
			if (abilityReference == nullptr) {
				if (World->IsServer()) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Ability in Server");
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Ability in Client");
				}
				abilityReference = UShooterAbility::MakeFor(this, ID);
			}
			if (abilityReference) {
				equipped = true;
				UE_LOG(LogTemp, Log, TEXT("ability instantiated"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Ability equipped");
				AbilityMap.Add(ID, abilityReference);
				UE_LOG(LogTemp, Log, TEXT("ability name: %s"), *abilityReference->GetName());
				//AddOnScreenDebugMessage(0, 2.f, FColor::Magenta, abilityReference->GetName());
			}
		}
	}

	return (equipped) ? true : false;

	//return true;
}

bool UShooterAbilitySystem::PlayAbility(EShooterAbilityID ID)
{
	bool successfullyPlayed = false;

	if (IsAbilityEquipped(ID)) {
		UShooterAbility* ability = *AbilityMap.Find(ID);

		if (ability) {
			successfullyPlayed = ability->PlayEffect();
		}
	}

	return successfullyPlayed;
}

TArray<EShooterAbilityID> UShooterAbilitySystem::GetAllAbilities()
{
	TArray<EShooterAbilityID> abilityList;
	UEnum* ids = StaticEnum<EShooterAbilityID>();
	int nAbilities = ids->NumEnums();

	for (int32 i = 0; i < nAbilities; i++) {
		abilityList.Add(static_cast<EShooterAbilityID>(i));
	}

	return abilityList;
}

TArray<EShooterAbilityID> UShooterAbilitySystem::GetEquippedAbilities()
{
	TArray<EShooterAbilityID> abilityList;
	AbilityMap.GenerateKeyArray(abilityList);

	return abilityList;
}

bool UShooterAbilitySystem::IsAbilityEquipped(EShooterAbilityID ID)
{
	return AbilityMap.Contains(ID);
}

bool UShooterAbilitySystem::IsAbilityValid(EShooterAbilityID ID)
{
	return AbilityClassMap.Contains(ID);
}

void UShooterAbilitySystem::SetKeyBindings(UInputComponent* ShooterInputComponent)
{
	if (!IsBound && IsValid(ShooterInputComponent)) {
		UEnum* enumID = FindObject<UEnum>(ANY_PACKAGE, TEXT("EShooterAbilityID"), true);

		for (int32 idx = 1; idx < enumID->NumEnums() - 2; idx++)
		{
			const FString actionName = enumID->GetNameStringByIndex(idx);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Binding "+actionName);

			// Pressed event
			{
				FInputActionBinding AB(FName(*actionName), IE_Pressed);
				AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UShooterAbilitySystem::AbilityLocalInputPressed, idx);
				ShooterInputComponent->AddActionBinding(AB);
				//InputComponent->BindAction(FName(*actionName), IE_Pressed, ShooterOwner, &AShooterCharacter::OnStartJump);
			}

			// Released event
			{
				FInputActionBinding AB(FName(*actionName), IE_Released);
				AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UShooterAbilitySystem::AbilityLocalInputReleased, idx);
				ShooterInputComponent->AddActionBinding(AB);
			}
		}

		// Bind Confirm/Cancel
		/*{
			FInputActionBinding AB(FName("Confirm"), IE_Pressed);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UShooterAbilitySystem::LocalInputConfirm);
			InputComponent->AddActionBinding(AB);
		}*/

		/*{
			FInputActionBinding AB(FName("Cancel"), IE_Pressed);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UShooterAbilitySystem::LocalInputCancel);
			InputComponent->AddActionBinding(AB);
		}*/

		IsBound = true;
	}
}

void UShooterAbilitySystem::AbilityLocalInputPressed(int32 InputID)
{
	UE_LOG(LogTemp, Warning, TEXT("muchas gracias aficion"));
	EShooterAbilityID ID = static_cast<EShooterAbilityID>(InputID);

	if (ID == EShooterAbilityID::Confirm || ID == EShooterAbilityID::Cancel) {
		return;
	}

	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Trying server Ability Input");
		AbilityServerInputPressed(ID);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Local Ability Input");
	PlayAbility(ID);
}
void UShooterAbilitySystem::AbilityServerInputPressed_Implementation(EShooterAbilityID ID)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Server Ability Input");

	if (ID == EShooterAbilityID::Confirm || ID == EShooterAbilityID::Cancel) {
		return;
	}

	PlayAbility(ID);
}
bool UShooterAbilitySystem::AbilityServerInputPressed_Validate(EShooterAbilityID ID)
{
	return true;
}

void UShooterAbilitySystem::AbilityLocalInputReleased(int32 InputID)
{
}
void UShooterAbilitySystem::AbilityServerInputReleased_Implementation(EShooterAbilityID ID)
{
}
bool UShooterAbilitySystem::AbilityServerInputReleased_Validate(EShooterAbilityID ID)
{
	return true;
}

void UShooterAbilitySystem::LocalInputConfirm()
{
}

void UShooterAbilitySystem::ServerInputConfirm_Implementation()
{
}
bool UShooterAbilitySystem::ServerInputConfirm_Validate()
{
	return true;
}

void UShooterAbilitySystem::LocalInputCancel()
{
}

void UShooterAbilitySystem::ServerInputCancel_Implementation()
{
}
bool UShooterAbilitySystem::ServerInputCancel_Validate()
{
	return true;
}

// Called when the game starts or when spawned
void UShooterAbilitySystem::BeginPlay()
{
	Super::BeginPlay();
	
	/*FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	const UClass* Class = UShooterAbility::StaticClass();
	AssetRegistryModule.Get().GetAssetsByClass(Class->GetFName(), AssetData);*/
}
