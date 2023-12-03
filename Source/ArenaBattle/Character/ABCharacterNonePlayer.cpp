// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonePlayer.h"
#include "Engine/AssetManager.h"
#include "AI/ABAIController.h"
#include "CharacterStat/ABCharacterStatComponent.h"

AABCharacterNonePlayer::AABCharacterNonePlayer()
{
	GetMesh()->SetHiddenInGame(true);
	// Default AIController º¯°æ
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AABCharacterNonePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(NPCMeshes.Num() > 0);
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterNonePlayer::NPCMeshLoadCompleted));
}

void AABCharacterNonePlayer::SetDead()
{
	Super::SetDead();

	auto ABAIController = Cast<AABAIController>(GetController());
	if (ABAIController)
		ABAIController->StopAI();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}),
		DeadEventDelayTime, false);
}

void AABCharacterNonePlayer::NPCMeshLoadCompleted()
{
	if (NPCMeshHandle.IsValid())
	{
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}
	NPCMeshHandle->ReleaseHandle();
}

float AABCharacterNonePlayer::GetAIPatrolRadius()
{
	return 800.f;
}

float AABCharacterNonePlayer::GetAIDetectRange()
{
	return 400.f;
}

float AABCharacterNonePlayer::GetAIAttackRange()
{
	return Stat->GetTotalStat().AttackRange + Stat->GetAttackRadius() * 2.f;
}

float AABCharacterNonePlayer::GetAITurnSpeed()
{
	return 2.f;
}

void AABCharacterNonePlayer::SetAIAttackDeletage(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AABCharacterNonePlayer::AttackByAI()
{
	ProcessComboCommand();
}

void AABCharacterNonePlayer::NotifyComboActionEnd()
{
	Super::NotifyComboActionEnd();

	OnAttackFinished.ExecuteIfBound();
}
