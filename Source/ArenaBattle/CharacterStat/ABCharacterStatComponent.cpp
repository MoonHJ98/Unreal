// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	CurrentLevel = 1;
	AttackRadius = 50.f;
	// bWantsInitializeComponentf�� true�� �ؾ��� InitializeComponent()�� ȣ���

	bWantsInitializeComponent = true;

}

void UABCharacterStatComponent::InitializeComponent()
{
	// bWantsInitializeComponentf�� true�� �ؾ��� InitializeComponent()�� ȣ���
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);

	/*
	* ���ο� �������� �����ǰ� NPC���� �ɶ�
	* �Ϲ����� spawn�� �ϸ�
	* BeginPlay���� ���� �� StageGimmick���� ABOpponentCharacter->SetLevel(CurrentStageNum); ȣ��
	* �׷� ������ ���ʹ� ������ �Ǳ� �� hp�� maxhp�� �����ϰ� �ڿ� �������� maxhp�� �����ϰԵ�
	* �׷��Ƿ� AABStageGimmick::OnOpponentSpawn���� SpawnActor�� �ƴ� SpawnActorDeffered�� �ؾ���
	*/
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0.f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
		OnHpZero.Broadcast();

	return CurrentHp;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	// �޸𸮿��� ����
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, BaseStat.MaxHp);

	// UI ����
	OnHpChanged.Broadcast(CurrentHp);
}


