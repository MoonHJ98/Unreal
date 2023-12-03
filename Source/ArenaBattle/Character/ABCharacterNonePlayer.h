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

NPCMeshes�� ABCharacterNonPlayer���� ����Ǿ� �ִٸ� NPCMeshes�� ���� �����ϰڴٴ� ��
 */

// UCLASS(config=ArenaBattle) : DefaultArenaBattle�̶�� �ҷ��鿩�� ����ϰڴ�
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

	// �ش� config���Ϸκ��� �����͸� �ҷ����ڴ�
	// ������Ʈ�� �ε��ɶ� �ڵ����� NPCMeshes�� ���� ä����
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	// �񵿱�� �ε��ϱ����� ����
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
