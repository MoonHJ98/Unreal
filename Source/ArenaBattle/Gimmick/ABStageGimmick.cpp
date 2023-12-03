// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"
#include "Character/ABCharacterNonePlayer.h"
#include "Item/ABItemBox.h"
#include "Interface/ABGameInterface.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Stage Section
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Object)
		Stage->SetStaticMesh(StageMeshRef.Object);

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.f, 775.f, 300.f));
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);
	
	// Gate Section
	static FName GateSockets[] = { TEXT("+XGate"), TEXT("-XGate"), TEXT("+YGate"), TEXT("-YGate") };
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	for (FName GateSocket : GateSockets)
	{
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		Gate->SetStaticMesh(GateMeshRef.Object);
		// SetupAttachment에서 소켓이름 지정
		Gate->SetupAttachment(Stage, GateSocket);
		Gate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		Gate->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		Gates.Add(GateSocket, Gate);

		FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		GateTrigger->SetupAttachment(Stage, GateSocket);
		GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateSocket);

		GateTriggers.Add(GateTrigger);
	}

	// State Section
	CurrentState = EStageState::READY;

	StageChangeActions.Add(EStageState::READY, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StageChangeActions.Add(EStageState::FIGHT, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StageChangeActions.Add(EStageState::REWARD, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward)));
	StageChangeActions.Add(EStageState::NEXT, FStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));

	// Fight Section
	OpponentSpawnTime = 2.f;
	OpponentClass = AABCharacterNonePlayer::StaticClass();

	// Reward Section
	RewardBoxClass = AABItemBox::StaticClass();
	for (const FName& GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}

	// Stage Stat
	CurrentStageNum = 0;
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::FIGHT);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 생성자에서 GateTrigger->ComponentTags.Add(GateSocket);를 통해 게이트 소켓을 태그로 넣어줌
	// 태그 1개가 없다면 잘못된것
	check(OverlappedComponent->ComponentTags.Num() == 1);

	// 태크 정보를 가져옴
	FName ComponentTag = OverlappedComponent->ComponentTags[0];

	// 태그 정보에서 왼쪽 2글자만 가져옴 +x, -x, +y, -y중 하나가 나옴 원래 +xGate..
	FName SocketName = FName(*ComponentTag.ToString().Left(2));

	// 맵 메쉬에 소켓이 있는지 확인
	check(Stage->DoesSocketExist(SocketName));

	// 소켓을 통해 위치를 가져옴
	FVector NewLocation = Stage->GetSocketLocation(SocketName);

	// 해당 위치에 자린을 제외한 static mesh가 있는지 확인
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(FName("GateTrigger"), false, this);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllObjects,
		FCollisionShape::MakeSphere(775.f),
		CollisionQueryParam
	);

	if (!bResult)
	{
		FTransform NewTransform(NewLocation);
		AABStageGimmick* NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransform);
		if (NewGimmick)
		{
			NewGimmick->SetStageNum(CurrentStageNum + 1);
			NewGimmick->FinishSpawning(NewTransform);
		}
		
	}
}
void AABStageGimmick::OpenAllGates()
{
	for (auto& Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
}
void AABStageGimmick::CloseAllGates()
{
		for (auto& Gate : Gates)
	{
		Gate.Value->SetRelativeRotation(FRotator::ZeroRotator);
	}
}
void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	// ExecuteIfBound : 바인딩 되어있는 델리게이트만 실행시킴
	// 다이나믹 델리게이트에서 유용함
	if (StageChangeActions.Contains(InNewState))
		StageChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
}
void AABStageGimmick::SetReady()
{
	// Stage 트리거는 활성화, Gate 트리거는 비 활설화
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();
}
void AABStageGimmick::SetFight()
{
	// 모든 트리거 비활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();

	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
}
void AABStageGimmick::SetChooseReward()
{
	// 모든 트리거 비활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
	SpawnRewardBoxes();
}
void AABStageGimmick::SetChooseNext()
{
	// 게이트 트리거 활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	auto ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
	if (ABGameMode)
	{
		ABGameMode->OnPlayerScoreChangend(CurrentStageNum);
		if (ABGameMode->IsGameCleared())
		{
			return;
		}
	}

	SetState(EStageState::REWARD);
}

void AABStageGimmick::OnOpponentSpawn()
{
	const FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 88.f;

	/*
* 새로운 스테이지 생성되고 NPC생성 될때
* 일반적인 spawn을 하면
* BeginPlay먼저 실행 후 StageGimmick에서 ABOpponentCharacter->SetLevel(CurrentStageNum); 호출
* 그럼 스폰된 액터는 레벨업 되기 전 hp를 maxhp로 설정하고난 뒤에 레벨업된 maxhp를 설정하게됨
* 그러므로 AABStageGimmick::OnOpponentSpawn에서 SpawnActor가 아닌 SpawnActorDeferred를 해야함
*/

	//AActor* OpponentActor = GetWorld()->SpawnActor(OpponentClass, &SpawnLocation, &FRotator::ZeroRotator);
	//AABCharacterNonePlayer* ABOpponentCharacter = Cast<AABCharacterNonePlayer>(OpponentActor);
	
	const FTransform SpawnTransform (GetActorLocation() + FVector::UpVector * 88.f);
	AABCharacterNonePlayer* ABOpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacterNonePlayer>(OpponentClass, SpawnTransform);


	if (ABOpponentCharacter)
	{
		// 죽었을 떄 OnOpponentDestroyed 호출되도록 델리게이트 연결

		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
		// NPC 레벨 설정
		ABOpponentCharacter->SetLevel(CurrentStageNum);

		// 이거 호출해줘야 액터 스폰
		ABOpponentCharacter->FinishSpawning(SpawnTransform);
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 상자를 하나 먹었을 떄 나머지 상자를 모두 제거하는 기능
	for (const auto& RewardBox : RewardBoxes)
	{
		// 약참조이기 때문에 유효한지 아닌지 확인
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
				ValidItemBox->Destroy();
		}
	}

	SetState(EStageState::NEXT);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FVector WorldSpawnLocation = GetActorLocation() + RewardBoxLocation.Value + FVector(0.f, 0.f, 30.f);
		FTransform NewTransform(WorldSpawnLocation);

		AABItemBox* RewardItemBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, NewTransform);
		if (RewardItemBoxActor)
		{
			RewardItemBoxActor->Tags.Add(RewardBoxLocation.Key);
			RewardItemBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(RewardItemBoxActor);
		}
	}

	for (const auto& RewardBox : RewardBoxes)
	{
		if(RewardBox.IsValid())
			RewardBox->FinishSpawning(RewardBox.Get()->GetActorTransform());
		 
	}
}

