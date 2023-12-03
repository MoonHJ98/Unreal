// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	// 부모 함수에서 CreateWidget를 통해 위젯 인스턴스 생성
	Super::InitWidget();

	// 생성된 후
	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
		ABUserWidget->SetOwningActor(GetOwner());
}
