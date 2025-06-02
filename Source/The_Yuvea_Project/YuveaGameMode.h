#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Widgets/GameStatsWidget.h"
#include "YuveaGameMode.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API AYuveaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AYuveaGameMode();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameStatsWidgetClass;

protected:
	virtual void BeginPlay() override;
	void SetupHUD();

private:
	UPROPERTY()
	UGameStatsWidget* GameStatsWidgetInstance;
};
