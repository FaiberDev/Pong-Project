// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmStoreSubsystem.h"

void URhythmStoreSubsystem::SetBPM(float NewBPM){
	CurrentBPM = NewBPM;
	OnBPMSet.Broadcast(NewBPM);
}