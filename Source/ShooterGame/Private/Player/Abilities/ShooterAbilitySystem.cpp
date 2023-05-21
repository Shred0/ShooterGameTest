// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
//#include "AssetRegistryModule.h"
#include "ShooterAbility.h"
//#include "Player/Abilities/ShooterAbility.h"
#include "Algo/Reverse.h"
#include "Player/Abilities/ShooterAbilitySystem.h"

// Sets default values
UShooterAbilitySystem::UShooterAbilitySystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;
	PrimaryComponentTick.bCanEverTick = true;
	//bReplicates = true

	//ReplicateSubobjects(,);

	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssetOb(TEXT("/Game/UI/HUD/HUDAddedAsset"));
	HUDAsset = HUDAssetOb.Object;

	//static ConstructorHelpers::FObjectFinder<UFont> FontOb(TEXT("/Game/UI/HUD/Roboto18")); //too small
	static ConstructorHelpers::FObjectFinder<UFont> FontOb(TEXT("/Game/UI/HUD/Roboto51"));
	TextFont = FontOb.Object;

	ShadowedFont.bEnableShadow = true;

	TextColor = FColor(110, 124, 131, 255);
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

// Called every frame
/*void UShooterAbilitySystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/
void UShooterAbilitySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//time between frames
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Delta Time: "+ FString::Printf(TEXT("%.4f"), DeltaTime));

	TArray<UShooterAbility*> abilities = GetEquippedAbilities();

	//managing passive effects and energy for each equipped ability
	for (UShooterAbility* ability : abilities) {
		//passive effect
		if (ability->GetHasPassiveEffect() /*&& ability->PassiveEffectCondition()*/ && !ability->GetIsPassiveInCooldown()) {
			if (ShooterAvatar->GetLocalRole() < ROLE_Authority && ability->DoesPassiveReplicate()) {
				ServerReplicatePassiveAbility(ability, DeltaTime);
			}
			ability->PlayPassiveEffect(DeltaTime);
		}

		//energy
		if (ability->UsesEnergy()) {
			//drain abilities
			if (ability->GetIsPlaying()) {
				//using energy from ability's pool based on elapsed time
				float energy = (ability->GetDrainRateInTime() / 1) * DeltaTime; //refill rate is in seconds, like delta time
				ability->UseEnergy(energy);
				//stop ability when energy reaches zero
				if (ability->GetEnergy() == 0) {
					ability->StopEffect();
				}
			}

			//refill abilities
			if (!ability->GetIsPlaying() && ability->AutoRefills() && ability->AutoRefillCondition()) {
				if (ability->GetEnergy() < ability->GetMaxEnergy()) {
					//adding energy on ability's pool based on elapsed time
					float energy = (ability->GetRefillRateInTime() / 1) * DeltaTime; //refill rate is in seconds, like delta time
					ability->AddEnergy(energy);
				}
			}
		}
	}
}

void UShooterAbilitySystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UShooterAbilitySystem, AbilityMap);
}

//UShooterAbilitySystem* UShooterAbilitySystem::MakeFor(AShooterCharacter* Owner)
void UShooterAbilitySystem::SetFor(AActor* PlayerState, AShooterCharacter* Avatar)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Setting Ability System");

	ActorOwner = PlayerState;
	ShooterAvatar = Avatar;
	//World = GEngine->GetWorldFromContextObject(Avatar, EGetWorldErrorMode::LogAndReturnNull);

	IsBound = false;

	//adding class references for all abilities based off IDs' names
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt) {
		UClass* ClassAbility = *ClassIt;
		//ClassAbility->GetClass();
		const UEnum* enumID = FindObject<UEnum>(ANY_PACKAGE, TEXT("EShooterAbilityID"), true);
		if (ClassAbility->IsChildOf(UShooterAbility::StaticClass()) && !ClassIt->HasAnyClassFlags(CLASS_Abstract) && enumID)
		{
			//retrieving class' name
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

			//if class name coresponds to an ID name, makes and equips the related ability
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
	if (GetOwnerRole() < ROLE_Authority) {
		//ServerAddAbility(ID);
	}
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
				if (GetWorld()->IsServer()) {
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
				//SetComponentTickEnabled(true)
			}
		}
	}

	return (equipped) ? true : false;

	//return true;
}

void UShooterAbilitySystem::ServerAddAbility_Implementation(EShooterAbilityID ID)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Adding Ability in Server");
	AddAbility(ID);
}
bool UShooterAbilitySystem::ServerAddAbility_Validate(EShooterAbilityID ID)
{
	return true;
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

void UShooterAbilitySystem::ServerReplicatePassiveAbility_Implementation(UShooterAbility* ability, float DeltaTime)
{
	if (ability && ability->GetHasPassiveEffect() && ability->DoesPassiveReplicate() && !ability->GetIsPassiveInCooldown()) {
		ability->PlayPassiveEffect(DeltaTime);
	}
}

void UShooterAbilitySystem::StopAbility(EShooterAbilityID ID)
{
	if (IsAbilityEquipped(ID)) {
		UShooterAbility* ability = *AbilityMap.Find(ID);

		if (ability && ability->GetIsPlaying()) {
			ability->StopEffect();
		}
	}
}

TArray<EShooterAbilityID> UShooterAbilitySystem::GetAllAbilityIDs()
{
	TArray<EShooterAbilityID> abilityList;
	UEnum* ids = StaticEnum<EShooterAbilityID>();
	int nAbilities = ids->NumEnums();

	for (int32 i = 0; i < nAbilities; i++) {
		abilityList.Add(static_cast<EShooterAbilityID>(i));
	}

	return abilityList;
}

TArray<class UClass*> UShooterAbilitySystem::GetAllAbilityClasses()
{
	TArray<class UClass*> abilityList;
	AbilityClassMap.GenerateValueArray(abilityList);

	return abilityList;
}

TArray<EShooterAbilityID> UShooterAbilitySystem::GetEquippedAbilityIDs()
{
	TArray<EShooterAbilityID> abilityList;
	AbilityMap.GenerateKeyArray(abilityList);

	return abilityList;
}

TArray<class UShooterAbility*> UShooterAbilitySystem::GetEquippedAbilities()
{
	TArray<class UShooterAbility*> abilityList;
	AbilityMap.GenerateValueArray(abilityList);

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

		for (int32 idx = 1; idx < enumID->NumEnums() - 1; idx++)
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
	EShooterAbilityID ID = static_cast<EShooterAbilityID>(InputID);

	if (ID == EShooterAbilityID::Confirm || ID == EShooterAbilityID::Cancel) {
		return;
	}

	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Trying server Ability Input release");
		AbilityServerInputReleased(ID);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Local Ability Input release");
	StopAbility(ID);
}
void UShooterAbilitySystem::AbilityServerInputReleased_Implementation(EShooterAbilityID ID)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Server Ability Input release");

	if (ID == EShooterAbilityID::Confirm || ID == EShooterAbilityID::Cancel) {
		return;
	}

	StopAbility(ID);
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

UTexture2D* UShooterAbilitySystem::GetHUDAsset()
{
	return HUDAsset;
}

void UShooterAbilitySystem::DrawAbilityHUD(UCanvas* &Canvas, FVector2D StartPos, float Scale = 1.f, float Offset = 0.f, bool DrawFromBottom = false, bool DrawFromRight = false, bool IsVerticalArray = false)
{
	if (!ShooterAvatar) return;

	float IconPosX = StartPos.X;
	float IconPosY = StartPos.Y;

	TArray<UShooterAbility*> abilities = GetEquippedAbilities();
		
	if ((!IsVerticalArray && DrawFromRight) || (IsVerticalArray && DrawFromBottom)) {
		Algo::Reverse(abilities);
	}

	//drowing ability HUD for each equipped ability
	for (UShooterAbility* ability : abilities) {
		FCanvasIcon* AbilityIcon = &ability->AbilityIcon;

		//setting up text
		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), TextFont, TextColor);
		TextItem.EnableShadow(FLinearColor::Black);
		float TextScale = 0.4f;
		float SizeX, SizeY;
		FString Text = ability->GetName();
		Canvas->StrLen(TextFont, Text, SizeX, SizeY);
		TextItem.Text = FText::FromString(Text);
		TextItem.Scale = FVector2D(TextScale * Scale, TextScale * Scale);
		TextItem.FontRenderInfo = ShadowedFont;
		TextItem.SetColor(TextColor);

		//adjusting position
		if (DrawFromBottom) {
			IconPosY -= (AbilityIcon->VL * Scale) + (SizeY * TextItem.Scale.Y);
		}

		if (DrawFromRight) {
			IconPosX -= (AbilityIcon->UL * Scale);
		}

		//drawing ability icon
		Canvas->SetDrawColor(255, 255, 255, 255);
		Canvas->DrawIcon(*AbilityIcon, IconPosX, IconPosY, Scale);

		//drawing ability name
		Canvas->DrawItem(TextItem, (IconPosX + (AbilityIcon->UL * Scale) / 2) - ((SizeX * TextScale * Scale) / 2),
			IconPosY + (AbilityIcon->VL * Scale + (Offset * Scale) / 2));

		//handling ability energy
		if (ability->UsesEnergy() && ability->GetEnergy() < ability->GetMaxEnergy()) {
			//drawing energy percentage
			float EnergyPerc = (ability->GetMaxEnergy() / 100) * ability->GetEnergy();
			Text = FString::Printf(TEXT("%.2f"), EnergyPerc) + "%";
			Canvas->StrLen(TextFont, Text, SizeX, SizeY);
			TextItem.Text = FText::FromString(Text);

			//reloading bar effect on cooldown
			FLinearColor Color = FLinearColor(.5f, .5f, .5f, .5f);
			FCanvasTileItem tileItem = FCanvasTileItem(
				FVector2D(IconPosX, IconPosY),
				FVector2D((AbilityIcon->UL * Scale), (AbilityIcon->VL * Scale) * ((100 - EnergyPerc) / 100)),
				Color);
			tileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
			Canvas->DrawItem(tileItem);

			//energy percentage on center
			Canvas->DrawItem(TextItem, (IconPosX + (AbilityIcon->UL * Scale) / 2) - ((SizeX * TextScale * Scale) / 2),
				IconPosY + (AbilityIcon->VL * Scale) / 2 - (SizeY * TextScale * Scale) / 2);
		}

		//handling ability cooldown
		if (ability->GetIsInCooldown()) {
			//drawing ability cooldown
			float TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(ability->GetCooldownTimer());
			/*FString StringTimeRemaining = FString::Printf(TEXT("%.2f"), TimeRemaining);
			FText TextTimeRemaining = FText::FromString(StringTimeRemaining);
			FText LocTextTimeRemaining = LOCTEXT("AbilityCooldownTeleport", "{0}s");
			Text = FText::Format(LocTextTimeRemaining, TextTimeRemaining).ToString();*/
			Text = FString::Printf(TEXT("%.2f"), TimeRemaining) + "s";
			Canvas->StrLen(TextFont, Text, SizeX, SizeY);
			TextItem.Text = FText::FromString(Text);

			//icon disabled effect on cooldown
			/*Canvas->DrawItem(TextItem, (AbilityIconPosX + (AbilityIconTeleport.UL * ScaleUI) / 2) - ((SizeX * TextScale * ScaleUI) / 2),
				AbilityIconPosY - (SizeY * TextScale * ScaleUI + AbilityIconOffsetY / 2));*/

			float TimeRate = GetWorld()->GetTimerManager().GetTimerRate(ability->GetCooldownTimer());
			float PercRemainingCooldown = (1 / TimeRate * TimeRemaining);
			FLinearColor Color = FLinearColor(.5f, .5f, .5f, PercRemainingCooldown);
			FCanvasTileItem tileItem = FCanvasTileItem(
				FVector2D(IconPosX, IconPosY),
				FVector2D((AbilityIcon->UL * Scale), (AbilityIcon->VL * Scale)),
				Color);
			//tileItem.Rotation = FRotator(0.f, 0.f, 0.f);
			//tileItem.PivotPoint = Center;
			//tileItem.SetColor(Color);
			//Canvas->SetDrawColor(255, 255, 255, (255 / TimeRate * TimeRemaining));
			tileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
			Canvas->DrawItem(tileItem);

			//drowing icon overlay
			//Canvas->SetDrawColor(255, 255, 255, 215);
			//Canvas->DrawIcon(AbilityIconCooldownLayer, IconPosX, IconPosY, Scale);

			//timer on center
			Canvas->DrawItem(TextItem, (IconPosX + (AbilityIcon->UL * Scale) / 2) - ((SizeX * TextScale * Scale) / 2),
				IconPosY + (AbilityIcon->VL * Scale) / 2 - (SizeY * TextScale * Scale) / 2);
		}

		//setting starting position for next icon
		if (IsVerticalArray) {
			if (DrawFromRight) {
				IconPosX += (AbilityIcon->UL * Scale);
			}

			if (DrawFromBottom) {
				IconPosY -= (Offset * Scale) + (SizeY * TextItem.Scale.Y);
			} else {
				IconPosY += (AbilityIcon->VL * Scale) + (Offset * Scale);
			}
		}else{
			if (DrawFromRight) {
				IconPosX -= (Offset * Scale);
			} else {
				IconPosX += (AbilityIcon->UL * Scale) + (Offset * Scale);
			}

			if (DrawFromBottom) {
				IconPosY += (AbilityIcon->VL * Scale) + (SizeY * TextItem.Scale.Y);
			}
		}
	}
}

void UShooterAbilitySystem::PlaySound(USoundCue* Sound, FVector Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Sound");
	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		ServerPlaySound(Sound, Location);
	}
	UGameplayStatics::PlaySoundAtLocation(ShooterAvatar, Sound, Location);
}
void UShooterAbilitySystem::ServerPlaySound_Implementation(USoundCue* Sound, FVector Location)
{
	MulticastSound(Sound, Location);
}
void UShooterAbilitySystem::MulticastSound_Implementation(USoundCue* Sound, FVector Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Sound");
	//only on proxies
	if (!ShooterAvatar->IsLocallyControlled()) {
		UGameplayStatics::PlaySoundAtLocation(ShooterAvatar, Sound, Location);
		//UGameplayStatics::SpawnSoundAttached(Sound, SCOwner->GetRootComponent());
	}
}

void UShooterAbilitySystem::PlayAudioComponent(UAudioComponent* AudioComponent, float StartTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Audio Component");
	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		ServerPlayAudioComponent(AudioComponent, StartTime);
	}
	if (AudioComponent) {
		AudioComponent->Play(StartTime);
	}
}
void UShooterAbilitySystem::ServerPlayAudioComponent_Implementation(UAudioComponent* AudioComponent, float StartTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Server Playing Ability Audio Component");
	MulticastAudioComponent(AudioComponent, StartTime);
}
void UShooterAbilitySystem::MulticastAudioComponent_Implementation(UAudioComponent* AudioComponent, float StartTime)
{
	if (!ShooterAvatar->IsLocallyControlled()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Audio Component");
		if (AudioComponent) {
			AudioComponent->Play(StartTime);
		}
	}
}

void UShooterAbilitySystem::StopAudioComponent(UAudioComponent* AudioComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Stopping Ability Audio Component");
	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		ServerStopAudioComponent(AudioComponent);
	}
	if (AudioComponent) {
		AudioComponent->Stop();
	}
}
void UShooterAbilitySystem::ServerStopAudioComponent_Implementation(UAudioComponent* AudioComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Server Stopping Ability Audio Component");
	MulticastStopAudioComponent(AudioComponent);
}
void UShooterAbilitySystem::MulticastStopAudioComponent_Implementation(UAudioComponent* AudioComponent)
{
	if (!ShooterAvatar->IsLocallyControlled()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Stop Ability Audio Component");
		if (AudioComponent) {
			AudioComponent->Stop();
		}
	}
}

void UShooterAbilitySystem::PlayParticle(UParticleSystem * FX, FVector Location, FRotator Rotation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Playing Ability Particle");
	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		ServerPlayParticle(FX, Location, Rotation);
	}
	UGameplayStatics::SpawnEmitterAtLocation(ShooterAvatar, FX, Location, Rotation);
}

void UShooterAbilitySystem::ServerPlayParticle_Implementation(UParticleSystem* FX, FVector Location, FRotator Rotation)
{
	MulticastParticle(FX, Location, Rotation);
}

void UShooterAbilitySystem::MulticastParticle_Implementation(UParticleSystem* FX, FVector Location, FRotator Rotation)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Ability Particle");
	//only on proxies
	if (!ShooterAvatar->IsLocallyControlled()) {
		UGameplayStatics::SpawnEmitterAtLocation(ShooterAvatar, FX, Location, Rotation);
	}
}

void UShooterAbilitySystem::SetActorVisibility(AActor* Actor, bool bVisible)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Setting Actor Visibility");
	if (!IsValid(Actor)) {
		return;
	}
	if (ShooterAvatar->GetLocalRole() < ROLE_Authority) {
		ServerSetActorVisibility(Actor, bVisible);
	}
	Actor->SetActorHiddenInGame(!bVisible);
	TArray<AActor*> children = Actor->Children;
	//Actor->GetAllChildActors(children, true);
	for (AActor* a : children) {
		a->SetActorHiddenInGame(!bVisible);
	}
}

void UShooterAbilitySystem::ServerSetActorVisibility_Implementation(AActor* Actor, bool bVisible)
{
	MulticastActorVisibility(Actor, bVisible);
}

void UShooterAbilitySystem::MulticastActorVisibility_Implementation(AActor* Actor, bool bVisible)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Multicasting Actor Visibility");
	//only on proxies
	if (!Actor->HasLocalNetOwner()) {
		//Actor->SetActorHiddenInGame(!bVisible);
		SetActorVisibility(Actor, bVisible);
	}
}
