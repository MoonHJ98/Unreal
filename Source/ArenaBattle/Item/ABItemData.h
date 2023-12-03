// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABItemData.generated.h"

// ��� ����Ʈ�� ȣȯ�� �� �ֵ��� BlueprintType�� �߰�
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon = 0,
	Potion,
	Scroll
};

/**
 * ������Ʈ ���� -> ���� �Ŵ������� ABItemData�� �θ�� �ϴ� ��� ���µ����͸� �ҷ������� ����
 * �̰� �̿��ؼ� �����ۻ��ڿ��� �ߴ� �������� �������� ������
 */
UCLASS()
class ARENABATTLE_API UABItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// ������ ������ ���̵� ���� ����
	// ABItemBox���� �ڽ����� ���� �������� Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);���� ã�� ������
	// ���� ���̵� ���� �����������
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// GetFName : ���� �̸�
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;
};
