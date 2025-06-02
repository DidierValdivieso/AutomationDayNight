#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <The_Yuvea_Project/Actors/UltraDynamicSkyActorBase.h>
#include <Components/TextBlock.h>
#include "GameStatsWidget.generated.h"

class UProgressBar;
class ASmallDinosaurCharacter;
class ABatCharacter;

UCLASS()
class THE_YUVEA_PROJECT_API UGameStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetOwnerDino(ASmallDinosaurCharacter* Dino);

    // void SetOwnerBat(ABat* Bat);

    UPROPERTY(meta = (BindWidget))
    UProgressBar* EnergyBar;

    UPROPERTY()
    AUltraDynamicSkyActorBase* SkyActor;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimeOfDayText;

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HydrationBar;

private:
    UPROPERTY()
    ASmallDinosaurCharacter* OwnerDino;

    UPROPERTY()
    ABatCharacter* OwnerBat;
};