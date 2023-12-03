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

	//AIController를 상속 받은 클래스를 가지고 있는 인스턴스
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 네비게이션 시스템 가져오기
	auto NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// ABAIController::RunAI에서 설정한 블랙보드 값(NPC스폰지점) 가져오기
	auto Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	
	FNavLocation NextPatrolPos;
	auto AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr ==  AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	// 스폰 지점 주위로 움직일 수 있는 반경내에 좌표 가져오기
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, AIPawn->GetAIPatrolRadius(), NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
