#include "WaterZoneVolumeActor.h"
#include "Components/BoxComponent.h"
#include "../Characters/SmallDinosaurCharacter.h"
#include "../Characters/BatCharacter.h"

AWaterZoneVolumeActor::AWaterZoneVolumeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;

    BoxComponent->SetBoxExtent(FVector(100000.0f, 100000.0f, 5000.f));
    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->SetGenerateOverlapEvents(true);

    BoxComponent->SetHiddenInGame(false);
}

void AWaterZoneVolumeActor::BeginPlay()
{
    Super::BeginPlay();

    SetActorLocation(FVector(11930.0f, -14740.0f, -27800.0f));
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWaterZoneVolumeActor::OnOverlapBegin);
    BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWaterZoneVolumeActor::OnOverlapEnd);
}

void AWaterZoneVolumeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (AMainCharacter* Character = Cast<AMainCharacter>(OtherActor))
    {
        Character->SetIsInWater(true);
    }
}

void AWaterZoneVolumeActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (AMainCharacter* Character = Cast<AMainCharacter>(OtherActor))
    {
        Character->SetIsInWater(false);
    }
}