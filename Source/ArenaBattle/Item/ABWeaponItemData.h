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
 * ������Ʈ ���� -> ���� �Ŵ������� ABItemData�� �θ�� �ϴ� ��� ���µ����͸� �ҷ������� ����
 * �̰� �̿��ؼ� �����ۻ��ڿ��� �ߴ� �������� �������� ������
 */
public:
	UABWeaponItemData();
	// ������ ������ ���̵� ���� ����
	// ABItemBox���� �ڽ����� ���� �������� Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);���� ã�� ������
	// ���� ���̵� ���� �����������
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		// GetFName : ���� �̸�
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	// TSoftObjectPtr : ����Ʈ ���۷���
	/*
	* ������ ���۵� �� �ʿ��� ĳ���� ���� ���� �ٷ� �ε��Ǿ ������
	   �ڽ��� �ִ� ������ ���� ���� �׷� �ʿ����

	*/
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;
};
