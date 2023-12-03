// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ABAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ABAI.h"


AABAIController::AABAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/ArenaBattle/AI/BB_ABCharacter.BB_ABCharacter'"));
	if (BBAssetRef.Object)
		BBAsset = BBAssetRef.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/ArenaBattle/AI/BT_ABCharacter.BT_ABCharacter'"));
	if (BTAssetRef.Object)
		BTAsset = BTAssetRef.Object;
}

void AABAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	// 만들어 놓은 블랙보드 에셋으로 블랙보드 컴포넌트 초기화
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		// 블랙보드에 NPC스폰 지점 지정
		BlackboardPtr->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		// 행동트리 구동
		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AABAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}
