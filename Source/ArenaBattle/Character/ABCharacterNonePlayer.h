// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonePlayer.generated.h"

/**
config -> DefaultArenaBattle.ini 
[/Script/ArenaBattle.ABCharacterNonePlayer]
+NPCMeshes=/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous
+NPCMeshes=/Game/InfinityBladeWarriors/Character/CompleteCharacters/sk_CharM_Base.sk_CharM_Base
+NPCMeshes=/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Bladed.SK_CharM_Bladed
.
.
.

NPCMeshes가 ABCharacterNonPlayer내에 선언되어 있다면 NPCMeshes의 값을 지정하겠다는 뜻
 */

// UCLASS(config=ArenaBattle) : DefaultArenaBattle이라는 불러들여서 사용하겠다
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonePlayer : public AABCharacterBase, public IABCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AABCharacterNonePlayer();

protected:
	virtual void PostInitializeComponents() override;
protected:
	void SetDead() override;
	void NPCMeshLoadCompleted();

	// 해당 config파일로부터 데이터를 불러오겠다
	// 프로젝트가 로딩될때 자동으로 NPCMeshes에 값이 채워짐
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	// 비동기로 로드하기위한 변수
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDeletage(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;

	virtual void NotifyComboActionEnd() override;
};
