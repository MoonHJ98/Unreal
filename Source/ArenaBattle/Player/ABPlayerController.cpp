// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"
#include "UI/ABHUDWidget.h"
#include "Kismet/GamePlayStatics.h"
#include "ABSaveGame.h"

DEFINE_LOG_CATEGORY(LogABPlayerController);

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> ABHUDWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_ABHUD.WBP_ABHUD_C"));
	if (ABHUDWidgetRef.Class)
		ABHUDWidgetClass = ABHUDWidgetRef.Class;

}

void AABPlayerController::GameScoreChaged(int32 NewScore)
{
	K2_OnScoreChanged(NewScore);
}

void AABPlayerController::GameClear()
{
	K2_OnGameClear();
}

void AABPlayerController::GameOver()
{
	K2_OnGameOver();

	// Saved -> SaveGames에 파일로 저장됨
	if (!UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Player0"), 0))
	{
		UE_LOG(LogABPlayerController, Error, TEXT("Save Game Error"));
	}
	K2_OnGameRetryCount(SaveGameInstance->RetryCount);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	// GameMode 블루프린트에서 호출하도록 변경
	// 위젯 생성
	// CreateWidget : 위젯의 NativeOnInitialized() 호출
	//ABHUDWidget = CreateWidget<UABHUDWidget>(this, ABHUDWidgetClass);
	//if (ABHUDWidget)
	//{
	//	//AddToViewport : 위젯의 NativeConstruct() 호출
	//	ABHUDWidget->AddToViewport();
	//}

	SaveGameInstance = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Player0"), 0));
	if (SaveGameInstance)
		SaveGameInstance->RetryCount++;
	else
		SaveGameInstance = NewObject<UABSaveGame>();

	K2_OnGameRetryCount(SaveGameInstance->RetryCount);

}
