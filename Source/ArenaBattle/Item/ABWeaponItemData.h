// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABWeaponItemData : public UABItemData
{
	GENERATED_BODY()
	
/**
 * 프로젝트 세팅 -> 에셋 매니저에서 ABItemData를 부모로 하는 모든 에셋데이터를 불러오도록 했음
 * 이걸 이용해서 아이템상자에서 뜨는 아이템을 랜덤으로 설정함
 */
public:
	UABWeaponItemData();
	// 생성될 에셋의 아이디를 직접 지정
	// ABItemBox에서 박스에서 나올 아이템을 Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);으로 찾기 때문에
	// 에셋 아이디를 직접 지정해줘야함
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// GetFName : 에셋 이름
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	// TSoftObjectPtr : 소프트 레퍼런싱
	/*
	* 게임이 시작될 때 필요한 캐릭터 같은 것은 바로 로딩되어도 되지만
	   박스에 있는 아이템 같은 경우는 그럴 필요없음

	*/
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;
};
