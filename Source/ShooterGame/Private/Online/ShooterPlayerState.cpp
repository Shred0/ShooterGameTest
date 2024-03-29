// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterPlayerState.h"
#include "Net/OnlineEngineInterface.h"

AShooterPlayerState::AShooterPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;

	AbilitySystem = CreateDefaultSubobject<UShooterAbilitySystem>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
}

void AShooterPlayerState::Reset()
{
	Super::Reset();
	
	//PlayerStates persist across seamless travel.  Keep the same teams as previous match.
	//SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void AShooterPlayerState::RegisterPlayerWithSession(bool bWasFromInvite)
{
	if (UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), NAME_GameSession))
	{
		Super::RegisterPlayerWithSession(bWasFromInvite);
	}
}

void AShooterPlayerState::UnregisterPlayerWithSession()
{
	if (!IsFromPreviousLevel() && UOnlineEngineInterface::Get()->DoesSessionExist(GetWorld(), NAME_GameSession))
	{
		Super::UnregisterPlayerWithSession();
	}
}

void AShooterPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void AShooterPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdateTeamColors();
}

void AShooterPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void AShooterPlayerState::AddBulletsFired(int32 NumBullets)
{
	NumBulletsFired += NumBullets;
}

void AShooterPlayerState::AddRocketsFired(int32 NumRockets)
{
	NumRocketsFired += NumRockets;
}

void AShooterPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void AShooterPlayerState::SetMatchId(const FString& CurrentMatchId)
{
	MatchId = CurrentMatchId;
}

void AShooterPlayerState::CopyProperties(APlayerState* PlayerState)
{	
	Super::CopyProperties(PlayerState);

	AShooterPlayerState* ShooterPlayer = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayer)
	{
		ShooterPlayer->TeamNumber = TeamNumber;
	}	
}

void AShooterPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OwnerController->GetCharacter());
		if (ShooterCharacter != NULL)
		{
			ShooterCharacter->UpdateTeamColorsAllMIDs();
		}
	}
}

int32 AShooterPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 AShooterPlayerState::GetKills() const
{
	return NumKills;
}

int32 AShooterPlayerState::GetDeaths() const
{
	return NumDeaths;
}

int32 AShooterPlayerState::GetNumBulletsFired() const
{
	return NumBulletsFired;
}

int32 AShooterPlayerState::GetNumRocketsFired() const
{
	return NumRocketsFired;
}

UShooterAbilitySystem * AShooterPlayerState::GetAbilitySystem() const
{
	return AbilitySystem;
}

bool AShooterPlayerState::IsQuitter() const
{
	return bQuitter;
}

FString AShooterPlayerState::GetMatchId() const
{
	return MatchId;
}

void AShooterPlayerState::ScoreKill(AShooterPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void AShooterPlayerState::ScoreDeath(AShooterPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void AShooterPlayerState::ScorePoints(int32 Points)
{
	AShooterGameState* const MyGameState = GetWorld()->GetGameState<AShooterGameState>();
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	SetScore(GetScore() + Points);
}

void AShooterPlayerState::InformAboutKill_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->GetUniqueId().IsValid())
	{	
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{		
			AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player might not have an ID if it was created with CreateDebugPlayer.
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				FUniqueNetIdRepl LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() &&  *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->GetUniqueId())
				{			
					TestPC->OnKill();
				}
			}
		}
	}
}

void AShooterPlayerState::BroadcastDeath_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);				
		}
	}	
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, TeamNumber);
	DOREPLIFETIME(AShooterPlayerState, NumKills);
	DOREPLIFETIME(AShooterPlayerState, NumDeaths);
	DOREPLIFETIME(AShooterPlayerState, MatchId);
}

FString AShooterPlayerState::GetShortPlayerName() const
{
	if( GetPlayerName().Len() > MAX_PLAYER_NAME_LENGTH )
	{
		return GetPlayerName().Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}
	return GetPlayerName();
}