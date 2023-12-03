#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AbCharacterStat.generated.h"


USTRUCT(BlueprintType)
struct FABCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterStat() : MaxHp(0.f), Attack(0.f), AttackRange(0.f), AttackSpeed(0.f), MovementSpeed(0.f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;

	FABCharacterStat operator+(const FABCharacterStat& Other) const
	{
		// const float* const : ��� ������
		// ����Ű�� ���� ������ ��ü�� ������� �ʴ´�.
		/*
		* �����ϴ� ����� *�� ��ġ�Դϴ�. *�� ����Ǹ� ���̾� ������ �������� ����� �ȴ�.
		�׷��� ������ int *const ptr1�� �ּҰ��� ���ó���� �Ǿ �ٸ� �ּҸ� ����ų ���� ���� ���̴�.

		�ٽ� const int *ptr1 �� �����ͺ����� ���ȭ�� �� �Ŷ�
		������ *ptr1 ������ ������ �Ұ����ϴ�.
		*/
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FABCharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FABCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; ++i)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
};