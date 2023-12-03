// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABItemData.generated.h"

// 블루 프린트와 호환될 수 있도록 BlueprintType를 추가
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon = 0,
	Potion,
	Scroll
};

/**
 * 프로젝트 세팅 -> 에셋 매니저에서 ABItemData를 부모로 하는 모든 에셋데이터를 불러오도록 했음
 * 이걸 이용해서 아이템상자에서 뜨는 아이템을 랜덤으로 설정함
 */
UCLASS()
class ARENABATTLE_API UABItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 생성될 에셋의 아이디를 직접 지정
	// ABItemBox에서 박스에서 나올 아이템을 Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);으로 찾기 때문에
	// 에셋 아이디를 직접 지정해줘야함
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// GetFName : 에셋 이름
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;
};
