// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABPlayerController, Log, All);
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

	// 블루프린트의 이벤트를 연결해주는 함수
	// K2 : 블루프린트의 전신인 kismet이란 기능을 나타내는 접두사

	// BlueprintImplementableEvent : 블루프린트에서 어떤 이벤트를 발동시키는 것처럼 기능을 수행하기 위한 키워드
	// Meta = (DisplayName = "") : 블루프린트에서 보이는 함수명

	// 블루프린트에서 함수를 제작할 것이기 때문에 cpp에서 몸체를 만들지 않아도 됨
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnScoreChangedCpp"))
	void K2_OnScoreChanged(int32 NewScore);

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameClearCpp"))
	void K2_OnGameClear();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameOverCpp"))
	void K2_OnGameOver();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameRetryCountCpp"))
	void K2_OnGameRetryCount(int32 NewRetryCount);

	void GameScoreChaged(int32 NewScore);
	void GameClear();
	void GameOver();


protected:
	virtual void BeginPlay() override;
	
// HUD Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UABHUDWidget> ABHUDWidget;

// Save Game Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Game)
	TObjectPtr<class UABSaveGame> SaveGameInstance;

};
