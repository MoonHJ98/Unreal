// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "ABPotionItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABPotionItemData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABPotionItemData();

	// 생성될 에셋의 아이디를 직접 지정
	// ABItemBox에서 박스에서 나올 아이템을 Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);으로 찾기 때문에
	// 에셋 아이디를 직접 지정해줘야함
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// GetFName : 에셋 이름
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Hp)
	float HealAmount;
};
