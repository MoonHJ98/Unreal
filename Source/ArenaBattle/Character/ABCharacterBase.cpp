// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "Physics/AbCollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABHpBarWidget.h"
#include "Item/ABItems.h"

DEFINE_LOG_CATEGORY(LogABCharacter);

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Pawn
	// Pawn�� ȸ���� �����ϱ� ���� ����
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	// �𵨸� ������ ���� ���⿡ �°� �����ġ�� ��������
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// �޽��� �ִϸ��̼� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// AnimBlueprint Ŭ���� ������ ���°� �ƴ϶� AnimInstance�� ����Ұ��̱� ������ _C
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// �����Ϳ��� �����س��� DataAsset�� CharacterControlManager�� ����
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object)
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);

	// ����Ʈ �޺����� ��Ÿ�� ���� (�������Ʈ���� �����ϴ°�)
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	// ����Ʈ �޺����� ���� ������ ����
	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Stat Component
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	// Widget Component
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	// ��ġ ����
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	// ������ �ִϸ��̼� �������Ʈ ó�� Ŭ���� ������ ����ؼ� BeginPlay�� ����Ǹ�
	// �׶� Ŭ���� �����κ��� �ν��Ͻ��� ������
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		// Ŭ���� ����
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		// 2D�� ����
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.f, 15.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	/*
	* ���� ������Ʈ�� PostInitializeComponents �ܰԿ��� ��� ������ ���������
	* ���� ������Ʈ�� BeginPlay���Ŀ� �����Ǳ� ������ �� 2���� �������� ������ Ÿ�̹��� �ʿ�
	* �̶� ��밡���� ���� WidgetComponent::InitWidget(�ν��Ͻ� ����) -> UUserWidget::NativeConstruct(UI �Ӽ� �ʱ�ȭ)
	* �𸮾� 5.1.1������ UUserWidget���� �ʱ�ȭ �ϱ� ���� ������ ���� ������Ʈ������ �����;��ϴµ� UUserWidget�� �����ϰ��ִ� WidgetComponent�κ��� ���������� ������ �� ����
	* �׷��� WidgetComponent�� UUserWidget�� ���� Ȯ���ؼ� ���͸� ������ �� �ֵ��� ABWidgetComponent�� ABUserWidget�� �������
	*/

	// Item Actions
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));


	// Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	// ù ������ �ƴ϶�� Ÿ�̸� ��ȿ���� üũ
	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		// Ÿ�̸Ӱ� ��ȿ�� ���� Ŭ���� ���Դٸ� ComboCheck callback
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	CurrentCombo = 1;

	// ���ݽ� �̵� ���ϵ���
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// �ִϸ��̼�
	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	// ��Ÿ��(���� �ִ�) ���� ���� �� ComboActionEnd ȣ�� �ǵ��� ��������Ʈ ����
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	// Ÿ�̸� �ʱ�ȭ
	ComboTimerHandle.Invalidate();

	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(class UAnimMontage* TargetMontage, bool isProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	NotifyComboActionEnd();
}

void AABCharacterBase::NotifyComboActionEnd()
{

}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0)
	{
		// ���� �� Ÿ�̸� ��ȿ�ϰ� ����
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}

}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();

	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	// SCENE_QUERY_STAT(Attack) : Attack�̶� �±׷� ������ �۾��� ���ؼ� ���� �� �� �ֵ��� �ϴ� ��
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = Stat->GetTotalStat().AttackRange;
	const float AttackRadius = Stat->GetAttackRadius();
	const float AttackDamage = Stat->GetTotalStat().Attack;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		// �������� ���� ���� ����
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}
#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRange, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.f);
#endif
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// EventInstigator : ������
	// DamageCauser : �����ڰ� ������ ��
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
		Stat->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateStat);

	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	// �������� enum���� ���� �ٸ��� ó��
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	auto PotionItemData = Cast<UABPotionItemData>(InItemData);
	if (PotionItemData)
	{
		Stat->HealHp(PotionItemData->HealAmount);
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	UABWeaponItemData* WeaponItemData = Cast<UABWeaponItemData>(InItemData);

	// �ϵ� ���۷����϶�
	//if (WeaponItemData)
	//	Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh);

	// ����Ʈ ���۷����϶�
	if (WeaponItemData)
	{
		// �޽��� �ε��Ǿ����� �ʴٸ�
		if (WeaponItemData->WeaponMesh.IsPending())
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
		Stat->SetModifierStat(WeaponItemData->ModifierStat);
	}

}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	auto ScrollItemData = Cast<UABScrollItemData>(InItemData);
	if (ScrollItemData)
	{
		Stat->AddBaseStat(ScrollItemData->BaseStat);
	}

}

int32 AABCharacterBase::GetLevel()
{
	return Stat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}


