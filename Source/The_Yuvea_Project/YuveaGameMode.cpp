#include "YuveaGameMode.h"
#include "Widgets/GameStatsWidget.h"
#include "Characters/SmallDinosaurCharacter.h"
#include "Characters/BatCharacter.h"
#include <Kismet/GameplayStatics.h>

AYuveaGameMode::AYuveaGameMode()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(TEXT("/Game/Blueprints/Widgets/WBP_GameStatsWidget"));
    if (WidgetFinder.Succeeded())
    {
        GameStatsWidgetClass = WidgetFinder.Class;
    }
}

void AYuveaGameMode::BeginPlay()
{
    Super::BeginPlay();

    SetupHUD();
}

void AYuveaGameMode::SetupHUD()
{
    if (!GameStatsWidgetClass) return;

    GameStatsWidgetInstance = CreateWidget<UGameStatsWidget>(GetWorld(), GameStatsWidgetClass);
    if (!GameStatsWidgetInstance) return;

    GameStatsWidgetInstance->AddToViewport();

    APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (ASmallDinosaurCharacter* Dino = Cast<ASmallDinosaurCharacter>(Pawn))
    {
        GameStatsWidgetInstance->SetOwnerDino(Dino);
    }
    /*
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (ABatCharacter* Bat = Cast<ABatCharacter>(Pawn))
    {
        GameStatsWidgetInstance->SetOwnerBat(Bat);
    }
    */
}