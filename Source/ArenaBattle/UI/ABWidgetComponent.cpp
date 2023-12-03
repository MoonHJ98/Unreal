// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABWidgetComponent.h"
#include "ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	// �θ� �Լ����� CreateWidget�� ���� ���� �ν��Ͻ� ����
	Super::InitWidget();

	// ������ ��
	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
		ABUserWidget->SetOwningActor(GetOwner());
}
