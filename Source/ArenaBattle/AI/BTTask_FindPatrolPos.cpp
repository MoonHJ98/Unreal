// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPatrolPos.h"
#include "ABAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto result = Super::ExecuteTask(OwnerComp, NodeMemory);

	//AIController�� ��� ���� Ŭ������ ������ �ִ� �ν��Ͻ�
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// �׺���̼� �ý��� ��������
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// ABAIController::RunAI���� ������ ������ ��(NPC��������) ��������
	auto Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	
	FNavLocation NextPatrolPos;
	auto AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr ==  AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	// ���� ���� ������ ������ �� �ִ� �ݰ泻�� ��ǥ ��������
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, AIPawn->GetAIPatrolRadius(), NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
