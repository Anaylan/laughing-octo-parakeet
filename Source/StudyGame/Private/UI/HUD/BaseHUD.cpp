// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BaseHUD.h"


#include "UI/Widgets/BaseWidget.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!AllWidgets.IsEmpty())
	{
		for (const TSubclassOf<UBaseWidget> Widget: AllWidgets)
		{
			if (!Widget) continue;

			UBaseWidget* CreatedWidget = CreateWidget<UBaseWidget>(GetWorld(), Widget);
			CreatedWidget->AddToViewport();
			
			CreatedWidgets.Add(CreatedWidget);
		}
	}
}

void ABaseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	CreatedWidgets.Empty();
}

void ABaseHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

