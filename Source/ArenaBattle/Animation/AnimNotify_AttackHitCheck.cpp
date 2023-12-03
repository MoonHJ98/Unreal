// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"

// 캐릭터 자체를 참조하는게 아니라 인터페이스를 참조하도록
//#include "Character/ABCharacterBase.h"

#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// 메쉬가 있다면 해당 매쉬의 소유자를 가져와 판정하도록 시키기
		// Character를 Include해서 사용하는 것 보다 인터페이스를 만들어 범용적으로 사용할 수 있게 만들기
		
		// 메쉬의 Owner인 Actor가 IABAnimationAttackInterface를 구현했는지 확인
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
