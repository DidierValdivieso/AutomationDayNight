#include "GameStatsWidget.h"
#include "Components/ProgressBar.h"
#include "../Characters/SmallDinosaurCharacter.h"
#include "../Characters/BatCharacter.h"
#include <The_Yuvea_Project/Actors/UltraDynamicSkyActorBase.h>
#include <Kismet/GameplayStatics.h>

void UGameStatsWidget::SetOwnerDino(ASmallDinosaurCharacter* Dino)
{
    OwnerDino = Dino;
}

/*
void UGameStatsWidget::SetOwnerBat(ABatCharacter* Bat)
{
    OwnerBat = Bat;
}
*/

void UGameStatsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!SkyActor)
    {
        SkyActor = Cast<AUltraDynamicSkyActorBase>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AUltraDynamicSkyActorBase::StaticClass()));
    }

    if (SkyActor && TimeOfDayText)
    {
        float Time = SkyActor->GetTimeOfDay();
        FString TimeString = FString::Printf(TEXT("%.2f h"), Time);
        TimeOfDayText->SetText(FText::FromString(TimeString));
    }

    if (OwnerDino && HydrationBar)
    {
        HydrationBar->SetPercent(OwnerDino->GetHydrationRatio());
    }

    if (OwnerDino && EnergyBar)
    {
        EnergyBar->SetPercent(OwnerDino->GetEnergyRatio());
    }

    /*
    if (OwnerBat && HydrationBar)
    {
        HydrationBar->SetPercent(OwnerBat->GetHydrationRatio());
    }

    if (OwnerBat && EnergyBar)
    {
        EnergyBar->SetPercent(OwnerBat->GetEnergyRatio());
    }
    */
}