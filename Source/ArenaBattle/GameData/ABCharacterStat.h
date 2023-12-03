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
		// const float* const : 상수 포인터
		// 가리키는 값과 포인터 자체가 변경되지 않는다.
		/*
		* 구분하는 방법은 *의 위치입니다. *이 선언되면 뒤이어 나오는 변수들은 상수가 된다.
		그렇기 때문에 int *const ptr1은 주소값이 상수처리가 되어서 다른 주소를 가리킬 수가 없는 것이다.

		다시 const int *ptr1 은 포인터변수가 상수화가 된 거라
		기존의 *ptr1 값에서 변경이 불가능하다.
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