// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.f;
}

void UABHpBarWidget::NativeConstruct()
{
	// 이 함수가 호출될 때는 UI관련 거의 모든 기능이 초기화 완료
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	HpStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensure(HpStat);

	// UABUserWidget 상속받고 난 후 OwingActor 사용가능
	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
		CharacterWidget->SetupCharacterWidget(this);
}

void UABHpBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxHp = (BaseStat + ModifierStat).MaxHp;
	if (HpProgressBar)
		HpProgressBar->SetPercent(CurrentHp / MaxHp);

	if (HpStat)
		HpStat->SetText(FText::FromString(GetHpStatText()));
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	CurrentHp = NewCurrentHp;

	ensure(MaxHp > 0.f);
	if (HpProgressBar)
	{
		// HpProgressBar는 UMG모듈에서 제공하므로 Build.cs에서 umg 모듈 추가해야함
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
	if (HpStat)
		HpStat->SetText(FText::FromString(GetHpStatText()));
}

FString UABHpBarWidget::GetHpStatText()
{
	return FString::Printf(TEXT("%.0f/%0.f"), CurrentHp, MaxHp);
}
