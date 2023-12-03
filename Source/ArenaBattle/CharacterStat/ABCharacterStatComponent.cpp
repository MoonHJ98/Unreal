// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	CurrentLevel = 1;
	AttackRadius = 50.f;
	// bWantsInitializeComponentf를 true로 해야지 InitializeComponent()가 호출됨

	bWantsInitializeComponent = true;

}

void UABCharacterStatComponent::InitializeComponent()
{
	// bWantsInitializeComponentf를 true로 해야지 InitializeComponent()가 호출됨
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);

	/*
	* 새로운 스테이지 생성되고 NPC생성 될때
	* 일반적인 spawn을 하면
	* BeginPlay먼저 실행 후 StageGimmick에서 ABOpponentCharacter->SetLevel(CurrentStageNum); 호출
	* 그럼 스폰된 액터는 레벨업 되기 전 hp를 maxhp로 설정하고난 뒤에 레벨업된 maxhp를 설정하게됨
	* 그러므로 AABStageGimmick::OnOpponentSpawn에서 SpawnActor가 아닌 SpawnActorDeffered를 해야함
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
	// 메모리에서 변경
	CurrentHp = FMath::Clamp<float>(NewHp, 0.f, BaseStat.MaxHp);

	// UI 변경
	OnHpChanged.Broadcast(CurrentHp);
}


