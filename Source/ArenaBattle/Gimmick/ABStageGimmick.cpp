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
		// SetupAttachment���� �����̸� ����
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
	// �����ڿ��� GateTrigger->ComponentTags.Add(GateSocket);�� ���� ����Ʈ ������ �±׷� �־���
	// �±� 1���� ���ٸ� �߸��Ȱ�
	check(OverlappedComponent->ComponentTags.Num() == 1);

	// ��ũ ������ ������
	FName ComponentTag = OverlappedComponent->ComponentTags[0];

	// �±� �������� ���� 2���ڸ� ������ +x, -x, +y, -y�� �ϳ��� ���� ���� +xGate..
	FName SocketName = FName(*ComponentTag.ToString().Left(2));

	// �� �޽��� ������ �ִ��� Ȯ��
	check(Stage->DoesSocketExist(SocketName));

	// ������ ���� ��ġ�� ������
	FVector NewLocation = Stage->GetSocketLocation(SocketName);

	// �ش� ��ġ�� �ڸ��� ������ static mesh�� �ִ��� Ȯ��
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

	// ExecuteIfBound : ���ε� �Ǿ��ִ� ��������Ʈ�� �����Ŵ
	// ���̳��� ��������Ʈ���� ������
	if (StageChangeActions.Contains(InNewState))
		StageChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
}
void AABStageGimmick::SetReady()
{
	// Stage Ʈ���Ŵ� Ȱ��ȭ, Gate Ʈ���Ŵ� �� Ȱ��ȭ
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();
}
void AABStageGimmick::SetFight()
{
	// ��� Ʈ���� ��Ȱ��ȭ
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
	// ��� Ʈ���� ��Ȱ��ȭ
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
	// ����Ʈ Ʈ���� Ȱ��ȭ
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
* ���ο� �������� �����ǰ� NPC���� �ɶ�
* �Ϲ����� spawn�� �ϸ�
* BeginPlay���� ���� �� StageGimmick���� ABOpponentCharacter->SetLevel(CurrentStageNum); ȣ��
* �׷� ������ ���ʹ� ������ �Ǳ� �� hp�� maxhp�� �����ϰ� �ڿ� �������� maxhp�� �����ϰԵ�
* �׷��Ƿ� AABStageGimmick::OnOpponentSpawn���� SpawnActor�� �ƴ� SpawnActorDeferred�� �ؾ���
*/

	//AActor* OpponentActor = GetWorld()->SpawnActor(OpponentClass, &SpawnLocation, &FRotator::ZeroRotator);
	//AABCharacterNonePlayer* ABOpponentCharacter = Cast<AABCharacterNonePlayer>(OpponentActor);
	
	const FTransform SpawnTransform (GetActorLocation() + FVector::UpVector * 88.f);
	AABCharacterNonePlayer* ABOpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacterNonePlayer>(OpponentClass, SpawnTransform);


	if (ABOpponentCharacter)
	{
		// �׾��� �� OnOpponentDestroyed ȣ��ǵ��� ��������Ʈ ����

		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
		// NPC ���� ����
		ABOpponentCharacter->SetLevel(CurrentStageNum);

		// �̰� ȣ������� ���� ����
		ABOpponentCharacter->FinishSpawning(SpawnTransform);
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ���ڸ� �ϳ� �Ծ��� �� ������ ���ڸ� ��� �����ϴ� ���
	for (const auto& RewardBox : RewardBoxes)
	{
		// �������̱� ������ ��ȿ���� �ƴ��� Ȯ��
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

