// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"

// ĳ���� ��ü�� �����ϴ°� �ƴ϶� �������̽��� �����ϵ���
//#include "Character/ABCharacterBase.h"

#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// �޽��� �ִٸ� �ش� �Ž��� �����ڸ� ������ �����ϵ��� ��Ű��
		// Character�� Include�ؼ� ����ϴ� �� ���� �������̽��� ����� ���������� ����� �� �ְ� �����
		
		// �޽��� Owner�� Actor�� IABAnimationAttackInterface�� �����ߴ��� Ȯ��
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
