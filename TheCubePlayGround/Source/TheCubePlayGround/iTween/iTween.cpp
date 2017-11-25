// Fill out your copyright notice in the Description page of Project Settings.

#include "iTween.h"
#include "TheCubePlayGround.h"
#include "iTweenPCH.h"
#include "iTweenEvent.h"


UWorld* UiTween::GetWorldLocal()
{
	for (TObjectIterator<UGameViewportClient> Itr; Itr; ++Itr)
	{
		return Itr->GetWorld();
	}

	return nullptr;
}

AiTweenEvent* UiTween::SpawnEvent(AiTAux* aux)
{
	UWorld* world = GetWorldLocal();

	if (world)
	{
		if (aux)
		{
			FActorSpawnParameters params;
			//params.bNoCollisionFail = true; //deprecated
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			params.Owner = aux;

			AiTweenEvent* ie = world->SpawnActor<AiTweenEvent>(AiTweenEvent::StaticClass(), params);

			ie->tweenIndex = (aux->currentTweens.Add(ie));

			return ie;
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "No iTAux defined");
			}
			return nullptr;
		}
	}
	else
	{
		if (aux)
		{
			Print("No world defined", "error");
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "No iTAux defined");
			}
		}
		return nullptr;
	}
}

AiTAux* UiTween::GetAux()
{
	AiTAux* aux = nullptr;

	for (TObjectIterator<AiTAux> Itr; Itr; ++Itr)
	{
		if (Itr->IsA(AiTAux::StaticClass()))
		{
			return *Itr;
		}
	}

	FActorSpawnParameters params;
	//params.bNoCollisionFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	aux = (AiTAux*)(GetWorldLocal()->SpawnActor<AiTAux>(AiTAux::StaticClass(), params));
	return aux;
}

UObject* UiTween::FindObjectByName(FString s)
{
	FName n = FName(*s);
	for (TObjectIterator<UObject> Itr; Itr; ++Itr)
	{
		FName f = Itr->GetFName();

		if (f == n)
		{
			Print("UObject found!");
			return *Itr;
		}
		else
		{
			continue;
		}
	}

	Print("No UObject with the specified name was found.", "error");
	return nullptr;
}

void UiTween::Print(FString message, FString type, float time, bool printToLog)
{
	AiTAux* aux = GetAux();

	if (aux && aux->printDebugMessages && type == "debug")
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Cyan, message);
		}

		if (printToLog)
		{
			UE_LOG(LogTemp, All, TEXT("%s"), *message);
		}
	}
	if (aux && aux->printErrorMessages && type == "error")
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, message);
		}

		if (printToLog)
		{
			UE_LOG(LogTemp, All, TEXT("%s"), *message);
		}
	}
}

//Constraints
FVector UiTween::ConstrainVector(FVector inputVector, FVector currentVector, EVectorConstraints::VectorConstraints vectorConstraints)
{
	switch (vectorConstraints)
	{
	case VectorConstraints::none:
		return inputVector;
	case VectorConstraints::xOnly:
		return FVector(inputVector.X, currentVector.Y, currentVector.Z);
	case VectorConstraints::yOnly:
		return FVector(currentVector.X, inputVector.Y, currentVector.Z);
	case VectorConstraints::zOnly:
		return FVector(currentVector.X, currentVector.Y, inputVector.Z);
	case VectorConstraints::xyOnly:
		return FVector(inputVector.X, inputVector.Y, currentVector.Z);
	case VectorConstraints::yzOnly:
		return FVector(currentVector.X, inputVector.Y, inputVector.Z);
	case VectorConstraints::xzOnly:
		return FVector(inputVector.X, currentVector.Y, inputVector.Z);
	default:
		return inputVector;
	}
}

FVector2D UiTween::ConstrainVector2D(FVector2D inputVector2D, FVector2D currentVector2D, EVector2DConstraints::Vector2DConstraints vector2DConstraints)
{
	switch (vector2DConstraints)
	{
	case Vector2DConstraints::none:
		return inputVector2D;
	case Vector2DConstraints::xOnly:
		return FVector2D(inputVector2D.X, currentVector2D.Y);
	case Vector2DConstraints::yOnly:
		return FVector2D(currentVector2D.X, inputVector2D.Y);
	default:
		return inputVector2D;
	}
}

FRotator UiTween::ConstrainRotator(FRotator inputRotator, FRotator currentRotator, ERotatorConstraints::RotatorConstraints rotatorConstraints)
{
	switch (rotatorConstraints)
	{
	case RotatorConstraints::none:
		return inputRotator;
	case RotatorConstraints::pitchOnly:
		return FRotator(inputRotator.Pitch, currentRotator.Yaw, currentRotator.Roll);
	case RotatorConstraints::yawOnly:
		return FRotator(currentRotator.Pitch, inputRotator.Yaw, currentRotator.Roll);
	case RotatorConstraints::rollOnly:
		return FRotator(currentRotator.Pitch, currentRotator.Yaw, inputRotator.Roll);
	case RotatorConstraints::pitchYawOnly:
		return FRotator(inputRotator.Pitch, inputRotator.Yaw, currentRotator.Roll);
	case RotatorConstraints::yawRollOnly:
		return FRotator(currentRotator.Pitch, inputRotator.Yaw, inputRotator.Roll);
	case RotatorConstraints::pitchRollOnly:
		return FRotator(inputRotator.Pitch, currentRotator.Yaw, inputRotator.Roll);
	default:
		return inputRotator;
	}
}

//General Use
void UiTween::GenerateSplineFromVectorArray(AiTSpline* &owningActor, USplineComponent* &splineComponent, FVector referenceVector, FRotator referenceRotator, TArray<FVector> vectorArray, bool localToReference, bool closeSpline)
{
	UWorld* world = GetWorldLocal();

	FActorSpawnParameters params;
	//params.bNoCollisionFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AiTSpline* a = world->SpawnActor<AiTSpline>(AiTSpline::StaticClass(), params);

	a->SetActorLocation(referenceVector);

	owningActor = a;
	splineComponent = a->spline;

	splineComponent->ClearSplinePoints();

	for (int i = 0; i < vectorArray.Num(); i++)
	{
		if (localToReference)
		{
			splineComponent->AddSplineWorldPoint(referenceRotator.RotateVector(vectorArray[i]) + referenceVector);
		}
		else
		{
			splineComponent->AddSplineWorldPoint(vectorArray[i]);
		}
	}

	splineComponent->SetClosedLoop(closeSpline);
}

void UiTween::GenerateSplineFromRotatorArray(AiTSpline* &owningActor, USplineComponent* &splineComponent, FVector referenceVector, FRotator referenceRotator, TArray<FRotator> rotatorArray, float generatedPointDistance, bool localToReference, bool closeSpline)
{
	UWorld* world = GetWorldLocal();

	FActorSpawnParameters params;
	//params.bNoCollisionFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AiTSpline* a = world->SpawnActor<AiTSpline>(AiTSpline::StaticClass(), params);

	a->SetActorLocation(referenceVector);

	owningActor = a;
	splineComponent = a->spline;

	splineComponent->ClearSplinePoints();

	for (int i = 0; i < rotatorArray.Num(); i++)
	{
		if (localToReference)
		{
			splineComponent->AddSplineWorldPoint(FRotator(FQuat(referenceRotator) * FQuat(rotatorArray[i])).RotateVector(FVector::ForwardVector) * generatedPointDistance + referenceVector);
		}
		else
		{
			splineComponent->AddSplineWorldPoint(rotatorArray[i].RotateVector(FVector::ForwardVector) * generatedPointDistance + referenceVector);
		}
	}

	splineComponent->SetClosedLoop(closeSpline);
}

float UiTween::GetDistanceBetweenTwoVectors(FVector sourceVector, FVector destination)
{
	return FMath::Sqrt(FMath::Pow((destination.X - sourceVector.X), 2) + FMath::Pow((destination.Y - sourceVector.Y), 2) + FMath::Pow((destination.Z - sourceVector.Z), 2));
}

//Stopping, Pausing and Resuming Tweens
void UiTween::StopTweeningByIndex(int32 index)
{
	GetAux()->currentTweens[index]->EndPhase();
}

void UiTween::StopTweeningByTweeningObjectName(FName objectName)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening->GetFName() == objectName)
			{
				e->EndPhase();
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening->GetFName() == objectName)
			{
				e->EndPhase();
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening->GetFName() == objectName)
			{
				e->EndPhase();
			}
		}
	}
}

void UiTween::StopTweeningByTweenName(FName tweenName)
{
	TArray<FName> arr;
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		arr.Add(e->tweenName);
	}
	while (arr.Contains(tweenName))
	{
		for (AiTweenEvent* e : GetAux()->currentTweens)
		{
			if (e->tweenName == tweenName)
			{
				e->EndPhase();

				break;
			}
		}

		arr.Empty();

		for (AiTweenEvent* e : GetAux()->currentTweens)
		{
			arr.Add(e->tweenName);
		}
	}

}

void UiTween::StopTweeningByTweeningObjectReference(UObject* object)
{
	TArray<UObject*> arr;
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			arr.Add(e->actorTweening);
		}
		else if (e->componentTweening != nullptr)
		{
			arr.Add(e->componentTweening);
		}
		else if (e->widgetTweening != nullptr)
		{
			arr.Add(e->widgetTweening);
		}
	}
	while (arr.Contains(object))
	{
		for (AiTweenEvent* e : GetAux()->currentTweens)
		{
			if (e->actorTweening != nullptr)
			{
				if (e->actorTweening == object)
				{
					e->EndPhase();
				}
			}
			else if (e->componentTweening != nullptr)
			{
				if (e->componentTweening == object)
				{
					e->EndPhase();
				}
			}
			else if (e->widgetTweening != nullptr)
			{
				if (e->widgetTweening == object)
				{
					e->EndPhase();
				}
			}

			break;
		}

		arr.Empty();

		for (AiTweenEvent* e : GetAux()->currentTweens)
		{
			if (e->actorTweening != nullptr)
			{
				arr.Add(e->actorTweening);
			}
			else if (e->componentTweening != nullptr)
			{
				arr.Add(e->componentTweening);
			}
			else if (e->widgetTweening != nullptr)
			{
				arr.Add(e->widgetTweening);
			}
		}
	}
}

void UiTween::StopTweeningByEventReference(AiTweenEvent* object)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e == object)
		{
			e->EndPhase();
		}
	}
}

void UiTween::StopAllTweens()
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		e->EndPhase();
	}
}

void UiTween::PauseTweeningByIndex(int32 index)
{
	GetAux()->currentTweens[index]->shouldTick = false;
}

void UiTween::PauseTweeningByTweeningObjectName(FName objectName)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening->GetFName() == objectName)
			{
				e->shouldTick = false;
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening->GetFName() == objectName)
			{
				e->shouldTick = false;
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening->GetFName() == objectName)
			{
				e->shouldTick = false;
			}
		}
	}
}

void UiTween::PauseTweeningByTweenName(FName tweenName)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->tweenName == tweenName)
		{
			e->shouldTick = false;
		}
	}
}

void UiTween::PauseTweeningByTweeningObjectReference(UObject* object)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening == object)
			{
				e->shouldTick = false;
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening == object)
			{
				e->shouldTick = false;
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening == object)
			{
				e->shouldTick = false;
			}
		}
	}
}

void UiTween::PauseTweeningByEventReference(AiTweenEvent* object)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e == object)
		{
			e->shouldTick = false;
		}
	}
}

void UiTween::PauseAllTweens()
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		e->shouldTick = false;
	}
}

void UiTween::ResumeTweeningByIndex(int32 index)
{
	GetAux()->currentTweens[index]->shouldTick = true;
}

void UiTween::ResumeTweeningByTweeningObjectName(FName objectName)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening->GetFName() == objectName)
			{
				e->shouldTick = true;
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening->GetFName() == objectName)
			{
				e->shouldTick = true;
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening->GetFName() == objectName)
			{
				e->shouldTick = true;
			}
		}
	}
}

void UiTween::ResumeTweeningByTweenName(FName tweenName)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->tweenName == tweenName)
		{
			e->shouldTick = true;
		}
	}
}

void UiTween::ResumeTweeningByTweeningObjectReference(UObject* object)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening == object)
			{
				e->shouldTick = true;
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening == object)
			{
				e->shouldTick = true;
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening == object)
			{
				e->shouldTick = true;
			}
		}
	}
}

void UiTween::ResumeTweeningByEventReference(AiTweenEvent* object)
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e == object)
		{
			e->shouldTick = true;
		}
	}
}

void UiTween::ResumeAllTweens()
{
	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		e->shouldTick = true;
	}
}

//Get iTweenEvents
AiTweenEvent* UiTween::GetEventByIndex(int32 index)
{
	return GetAux()->currentTweens[index];
}

TArray<AiTweenEvent*> UiTween::GetEventsByTweeningObjectName(FName objectName)
{
	TArray<AiTweenEvent*> arr;

	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening->GetFName() == objectName)
			{
				arr.Add(e);
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening->GetFName() == objectName)
			{
				arr.Add(e);
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening->GetFName() == objectName)
			{
				arr.Add(e);
			}
		}
	}

	return arr;
}

TArray<AiTweenEvent*> UiTween::GetEventsByTweenName(FName tweenName)
{
	TArray<AiTweenEvent*> arr;

	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->tweenName == tweenName)
		{
			arr.Add(e);
		}
	}

	return arr;
}

TArray<AiTweenEvent*> UiTween::GetEventsByTweeningObjectReference(UObject* object)
{
	TArray<AiTweenEvent*> arr;

	for (AiTweenEvent* e : GetAux()->currentTweens)
	{
		if (e->actorTweening != nullptr)
		{
			if (e->actorTweening == object)
			{
				arr.Add(e);
			}
		}
		else if (e->componentTweening != nullptr)
		{
			if (e->componentTweening == object)
			{
				arr.Add(e);
			}
		}
		else if (e->widgetTweening != nullptr)
		{
			if (e->widgetTweening == object)
			{
				arr.Add(e);
			}
		}
	}

	return arr;
}

//FRotator UiTween::GetActorRotationByParent(AActor* actor)
//{
//	return FRotator::ZeroRotator;
//}

//Actor
//Actor Move From/To
AiTweenEvent* UiTween::ActorMoveFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, AActor* actorToMove /*= nullptr*/, FVector locationFrom /*= FVector::ZeroVector*/, FVector locationTo /*= FVector::ZeroVector*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, ECoordinateSpace::CoordinateSpace coordinateSpace, bool sweep /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, ELookType::LookType orientation /*= noOrientationChange*/, UObject* orientationTarget /*= nullptr*/, float orientationSpeed /*= 5.0f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToMove;
		ie->vectorFrom = locationFrom;
		ie->vectorTo = locationTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		ie->coordinateSpace = coordinateSpace;
		ie->sweep = sweep;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->orientation = orientation;
		ie->orientationTarget = orientationTarget;
		ie->orientationSpeed = orientationSpeed;
		ie->rotatorConstraints = rotatorConstraints;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveFromToSimple(FName tweenName /*= "No Name"*/, AActor* actorToMove /*= nullptr*/, FVector locationFrom /*= FVector::ZeroVector*/, FVector locationTo /*= FVector::ZeroVector*/, ECoordinateSpace::CoordinateSpace coordinateSpace, bool sweep, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveFromTo;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToMove;
		ie->vectorFrom = locationFrom;
		ie->vectorTo = locationTo;
		ie->coordinateSpace = coordinateSpace;
		ie->sweep = sweep;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveFromToExpert(AActor* actorToMove, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve, UObject* orientationTarget, UObject* onTweenStartTarget, UObject* onTweenUpdateTarget, UObject* onTweenLoopTarget, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveFromTo;
		ie->actorTweening = actorToMove;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->orientationTarget = orientationTarget;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveFromToMin(AActor* actorToMove, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveFromTo;
		ie->actorTweening = actorToMove;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Actor Move To Spline Point
AiTweenEvent* UiTween::ActorMoveToSplinePointFull(float timerInterval, FName tweenName /*= "No Name"*/, AActor* actorToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool moveToPath /*= false*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, bool sweep /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, bool switchPathOrientationDirection, ELookType::LookType orientation /*= noOrientationChange*/, UObject* orientationTarget /*= nullptr*/, float orientationSpeed /*= 5.0f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*) = false*/, bool destroySplineComponent, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveToSplinePoint;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToMove;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = moveToPath;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		ie->sweep = sweep;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->switchPathOrientationDirection = switchPathOrientationDirection;
		ie->orientation = orientation;
		ie->orientationTarget = orientationTarget;
		ie->orientationSpeed = orientationSpeed;
		ie->rotatorConstraints = rotatorConstraints;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->destroySplineObject = destroySplineComponent;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		if (!actorToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveToSplinePointSimple(FName tweenName /*= "No Name"*/, AActor* actorToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool moveToPath /*= false*/, bool sweep /*= false*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool destroySplineComponent)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveToSplinePoint;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToMove;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = moveToPath;
		ie->sweep = sweep;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->destroySplineObject = destroySplineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		if (!actorToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveToSplinePointExpert(AActor* actorToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* orientationTarget /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveToSplinePoint;
		ie->actorTweening = actorToMove;
		ie->splineComponent = splineComponent;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->orientationTarget = orientationTarget;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!actorToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorMoveToSplinePointMin(AActor* actorToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorMoveToSplinePoint;
		ie->actorTweening = actorToMove;
		ie->splineComponent = splineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!actorToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

//Actor Move Update
void UiTween::ActorMoveUpdate(AActor* actorToMove, FVector locationTo, float delta, float speed, bool isLocal, bool sweep, EVectorConstraints::VectorConstraints vectorConstraints, ELookType::LookType orientation, UObject* orientationTarget, float orientationSpeed, ERotatorConstraints::RotatorConstraints rotatorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToMove->IsValidLowLevel())
	{
		if (isLocal && actorToMove->GetRootComponent()->GetAttachParent()->IsValidLowLevel())
		{
			locationTo = (actorToMove->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorRotation().RotateVector(locationTo) + actorToMove->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorLocation());
		}

		actorToMove->SetActorLocation(ConstrainVector(FMath::VInterpTo(actorToMove->GetActorLocation(), locationTo, delta, speed), actorToMove->GetActorLocation(), vectorConstraints), sweep);

		if (orientation == orientToTarget)
		{
			if (orientationTarget->IsA(AActor::StaticClass()))
			{
				ActorRotateUpdate(actorToMove, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - actorToMove->GetActorLocation()).Rotator(), delta, orientationSpeed, /*false, */false, rotatorConstraints);
			}
			else if (orientationTarget->IsA(USceneComponent::StaticClass()))
			{
				ActorRotateUpdate(actorToMove, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - actorToMove->GetActorLocation()).Rotator(), delta, orientationSpeed, /*false, */false, rotatorConstraints);
			}
			else if (orientationTarget->IsA(UWidget::StaticClass()))
			{
				//Feature is still to come. Waiting on getting the ability to read widget position in screen space
			}
		}
		else if (orientation == orientToPath)
		{
			actorToMove->SetActorRotation(ConstrainRotator(FMath::RInterpTo(actorToMove->GetActorRotation(), FRotationMatrix::MakeFromX((ConstrainVector(locationTo, actorToMove->GetActorLocation(), vectorConstraints)) - actorToMove->GetActorLocation()).Rotator(), delta, orientationSpeed), actorToMove->GetActorRotation(), rotatorConstraints));
		}
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Actor Rotate From/To
AiTweenEvent* UiTween::ActorRotateFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, AActor* actorToRotate /*= nullptr*/, FRotator rotationFrom /*= FRotator::ZeroRotator*/, FRotator rotationTo /*= FRotator::ZeroRotator*/, bool enforceValueTo, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= VectorConstraints::none*/, ECoordinateSpace::CoordinateSpace coordinateSpace, bool shortestPath /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToRotate;
		ie->rotatorFrom = rotationFrom;
		ie->rotatorTo = rotationTo;
		ie->enforceValueTo = enforceValueTo;
		ie->rotatorConstraints = rotatorConstraints;
		ie->coordinateSpace = coordinateSpace;
		ie->shortestPath = shortestPath;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateFromToSimple(FName tweenName /*= "No Name"*/, AActor* actorToRotate /*= nullptr*/, FRotator rotationFrom /*= FRotator::ZeroRotator*/, FRotator rotationTo /*= FRotator::ZeroRotator*/, ECoordinateSpace::CoordinateSpace coordinateSpace, bool shortestPath /*= false*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateFromTo;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToRotate;
		ie->rotatorFrom = rotationFrom;
		ie->rotatorTo = rotationTo;
		ie->coordinateSpace = coordinateSpace;
		ie->shortestPath = shortestPath;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateFromToExpert(AActor* actorToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateFromTo;
		ie->actorTweening = actorToRotate;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateFromToMin(AActor* actorToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateFromTo;
		ie->actorTweening = actorToRotate;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Actor Rotate To Spline Point
AiTweenEvent* UiTween::ActorRotateToSplinePointFull(float timerInterval, FName tweenName /*= "No Name"*/, AActor* actorToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool rotateToPath /*= false*/, bool enforceValueTo, float generatedPointDistance, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool destroySplineComponent, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateToSplinePoint;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToRotate;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = rotateToPath;
		ie->enforceValueTo = enforceValueTo;
		ie->generatedPointDistance = generatedPointDistance;
		ie->rotatorConstraints = rotatorConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->destroySplineObject = destroySplineComponent;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		if (!actorToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateToSplinePointSimple(FName tweenName /*= "No Name"*/, AActor* actorToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool rotateToPath /*= false*/, float generatedPointDistance, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool destroySplineComponent)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateToSplinePoint;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToRotate;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = rotateToPath;
		ie->generatedPointDistance = generatedPointDistance;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->destroySplineObject = destroySplineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		if (!actorToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateToSplinePointExpert(AActor* actorToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* orientationTarget /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateToSplinePoint;
		ie->actorTweening = actorToRotate;
		ie->splineComponent = splineComponent;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!actorToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorRotateToSplinePointMin(AActor* actorToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (actorToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorRotateToSplinePoint;
		ie->actorTweening = actorToRotate;
		ie->splineComponent = splineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!actorToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

//Actor Rotate Update
void UiTween::ActorRotateUpdate(AActor* actorToRotate, FRotator rotationTo, float delta, float speed, bool isLocal, ERotatorConstraints::RotatorConstraints rotatorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToRotate->IsValidLowLevel())
	{
		if (isLocal && actorToRotate->GetRootComponent()->GetAttachParent()->IsValidLowLevel())
		{
			rotationTo = FRotator(FQuat(actorToRotate->GetRootComponent()->GetAttachParent()->GetComponentRotation()) * FQuat(rotationTo));
		}

		actorToRotate->SetActorRotation(ConstrainRotator(FMath::RInterpTo(actorToRotate->GetActorRotation(), rotationTo, delta, speed), actorToRotate->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Actor Scale From/To
AiTweenEvent* UiTween::ActorScaleFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, AActor* actorToScale /*= nullptr*/, FVector scaleFrom /*= FVector::ZeroVector*/, FVector scaleTo /*= FVector(1,1,1)*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, bool isLocal, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorScaleFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToScale;
		ie->vectorFrom = scaleFrom;
		ie->vectorTo = scaleTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		if (isLocal)
		{
			ie->coordinateSpace = CoordinateSpace::parent;
		}
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorScaleFromToSimple(FName tweenName /*= "No Name"*/, AActor* actorToScale /*= nullptr*/, FVector scaleFrom /*= FVector::ZeroVector*/, FVector scaleTo /*= FVector(1,1,1)*/, bool isLocal, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorScaleFromTo;
		ie->tweenName = tweenName;
		ie->actorTweening = actorToScale;
		ie->vectorFrom = scaleFrom;
		ie->vectorTo = scaleTo;
		if (isLocal)
		{
			ie->coordinateSpace = CoordinateSpace::parent;
		}
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorScaleFromToExpert(AActor* actorToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorScaleFromTo;
		ie->actorTweening = actorToScale;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ActorScaleFromToMin(AActor* actorToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::actorScaleFromTo;
		ie->actorTweening = actorToScale;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Actor Scale Update
void UiTween::ActorScaleUpdate(AActor* actorToScale, FVector scaleTo, float delta, float speed, bool isLocal, EVectorConstraints::VectorConstraints vectorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (actorToScale->IsValidLowLevel())
	{
		if (isLocal && actorToScale->GetRootComponent()->GetAttachParent()->IsValidLowLevel())
		{
			scaleTo = actorToScale->GetRootComponent()->GetAttachParent()->GetComponentScale() * scaleTo;
		}

		actorToScale->SetActorScale3D(ConstrainVector(FMath::VInterpTo(actorToScale->GetActorScale3D(), scaleTo, delta, speed), actorToScale->GetActorScale3D(), vectorConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Component
//Component Move From/To
AiTweenEvent* UiTween::ComponentMoveFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, USceneComponent* componentToMove /*= nullptr*/, FVector locationFrom /*= FVector::ZeroVector*/, FVector locationTo /*= FVector::ZeroVector*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, ECoordinateSpace::CoordinateSpace coordinateSpace /*= CoordinateSpace::world*/, bool sweep /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, ELookType::LookType orientation /*= noOrientationChange*/, UObject* orientationTarget /*= nullptr*/, float orientationSpeed /*= 5.0f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nu) llptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToMove;
		ie->vectorFrom = locationFrom;
		ie->vectorTo = locationTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		ie->coordinateSpace = coordinateSpace;
		ie->sweep = sweep;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->orientation = orientation;
		ie->orientationTarget = orientationTarget;
		ie->orientationSpeed = orientationSpeed;
		ie->rotatorConstraints = rotatorConstraints;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveFromToSimple(FName tweenName /*= "No Name"*/, USceneComponent* componentToMove /*= nullptr*/, FVector locationFrom /*= FVector::ZeroVector*/, FVector locationTo /*= FVector::ZeroVector*/, ECoordinateSpace::CoordinateSpace coordinateSpace /*= CoordinateSpace::world*/, bool sweep /*= false*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveFromTo;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToMove;
		ie->vectorFrom = locationFrom;
		ie->vectorTo = locationTo;
		ie->coordinateSpace = coordinateSpace;
		ie->sweep = sweep;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveFromToExpert(USceneComponent* componentToMove /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* orientationTarget /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveFromTo;
		ie->componentTweening = componentToMove;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->orientationTarget = orientationTarget;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveFromToMin(USceneComponent* componentToMove /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveFromTo;
		ie->componentTweening = componentToMove;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Component Move To Spline Point
AiTweenEvent* UiTween::ComponentMoveToSplinePointFull(float timerInterval, FName tweenName /*= "No Name"*/, USceneComponent* componentToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool moveToPath /*= false*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, bool sweep /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, bool switchPathOrientationDirection /*= true*/, ELookType::LookType orientation /*= noOrientationChange*/, UObject* orientationTarget /*= nullptr*/, float orientationSpeed /*= 5.0f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget, bool tickWhenPaused, bool ignoreTimeDilation, bool destroySplineComponent, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveToSplinePoint;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToMove;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = moveToPath;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		ie->sweep = sweep;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->switchPathOrientationDirection = switchPathOrientationDirection;
		ie->orientation = orientation;
		ie->orientationTarget = orientationTarget;
		ie->orientationSpeed = orientationSpeed;
		ie->rotatorConstraints = rotatorConstraints;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->destroySplineObject = destroySplineComponent;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		if (!componentToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveToSplinePointSimple(FName tweenName /*= "No Name"*/, USceneComponent* componentToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool moveToPath /*= false*/, bool sweep /*= false*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool destroySplineComponent)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveToSplinePoint;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToMove;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = moveToPath;
		ie->sweep = sweep;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->destroySplineObject = destroySplineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		if (!componentToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveToSplinePointExpert(USceneComponent* componentToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* orientationTarget /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveToSplinePoint;
		ie->componentTweening = componentToMove;
		ie->splineComponent = splineComponent;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->orientationTarget = orientationTarget;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!componentToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentMoveToSplinePointMin(USceneComponent* componentToMove /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToMove->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compMoveToSplinePoint;
		ie->componentTweening = componentToMove;
		ie->splineComponent = splineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!componentToMove->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

//Component Move Update
void UiTween::ComponentMoveUpdate(USceneComponent* componentToMove, FVector locationTo, float delta, float speed, bool isLocal, bool sweep, EVectorConstraints::VectorConstraints vectorConstraints, ELookType::LookType orientation, UObject* orientationTarget, float orientationSpeed, ERotatorConstraints::RotatorConstraints rotatorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToMove->IsValidLowLevel())
	{
		if (isLocal && componentToMove->GetOwner()->IsValidLowLevel())
		{
			locationTo = (componentToMove->GetOwner()->GetActorRotation().RotateVector(locationTo) + componentToMove->GetOwner()->GetActorLocation());
		}

		componentToMove->SetWorldLocation(ConstrainVector(FMath::VInterpTo(componentToMove->GetComponentLocation(), locationTo, delta, speed), componentToMove->GetComponentLocation(), vectorConstraints), sweep);

		if (orientation == orientToTarget)
		{
			if (orientationTarget->IsA(AActor::StaticClass()))
			{
				ComponentRotateUpdate(componentToMove, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - componentToMove->GetComponentLocation()).Rotator(), delta, orientationSpeed, /*false, */false, rotatorConstraints);
			}
			else if (orientationTarget->IsA(USceneComponent::StaticClass()))
			{
				ComponentRotateUpdate(componentToMove, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - componentToMove->GetComponentLocation()).Rotator(), delta, orientationSpeed, /*false, */false, rotatorConstraints);
			}
			else if (orientationTarget->IsA(UWidget::StaticClass()))
			{
				//Feature is still to come. Waiting on getting the ability to read widget position in screen space
			}
		}
		else if (orientation == orientToPath)
		{
			componentToMove->SetWorldRotation(ConstrainRotator(FMath::RInterpTo(componentToMove->GetComponentRotation(), FRotationMatrix::MakeFromX((ConstrainVector(locationTo, componentToMove->GetComponentLocation(), vectorConstraints)) - componentToMove->GetComponentLocation()).Rotator(), delta, orientationSpeed), componentToMove->GetComponentRotation(), rotatorConstraints));
		}
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Component Rotate From/To
AiTweenEvent* UiTween::ComponentRotateFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, USceneComponent* componentToRotate /*= nullptr*/, FRotator rotationFrom /*= FRotator::ZeroRotator*/, FRotator rotationTo /*= FRotator::ZeroRotator*/, bool enforceValueTo, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, ECoordinateSpace::CoordinateSpace coordinateSpace /*= CoordinateSpace::world*/, bool shortestPath /*= false*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToRotate;
		ie->rotatorFrom = rotationFrom;
		ie->rotatorTo = rotationTo;
		ie->enforceValueTo = enforceValueTo;
		ie->rotatorConstraints = rotatorConstraints;
		ie->coordinateSpace = coordinateSpace;
		ie->shortestPath = shortestPath;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateFromToSimple(FName tweenName /*= "No Name"*/, USceneComponent* componentToRotate /*= nullptr*/, FRotator rotationFrom /*= FRotator::ZeroRotator*/, FRotator rotationTo /*= FRotator::ZeroRotator*/, ECoordinateSpace::CoordinateSpace coordinateSpace /*= CoordinateSpace::world*/, bool shortestPath /*= false*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateFromTo;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToRotate;
		ie->rotatorFrom = rotationFrom;
		ie->rotatorTo = rotationTo;
		ie->coordinateSpace = coordinateSpace;
		ie->shortestPath = shortestPath;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateFromToExpert(USceneComponent* componentToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateFromTo;
		ie->componentTweening = componentToRotate;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateFromToMin(USceneComponent* componentToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateFromTo;
		ie->componentTweening = componentToRotate;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Component Rotate To Spline Point
AiTweenEvent* UiTween::ComponentRotateToSplinePointFull(float timerInterval, FName tweenName /*= "No Name"*/, USceneComponent* componentToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool rotateToPath /*= false*/, bool enforceValueTo, float generatedPointDistance /*= 100.f*/, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool destroySplineComponent, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateToSplinePoint;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToRotate;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = rotateToPath;
		ie->enforceValueTo = enforceValueTo;
		ie->generatedPointDistance = generatedPointDistance;
		ie->rotatorConstraints = rotatorConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->destroySplineObject = destroySplineComponent;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		if (!componentToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateToSplinePointSimple(FName tweenName /*= "No Name"*/, USceneComponent* componentToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, bool rotateToPath /*= false*/, float generatedPointDistance /*= 100.f*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool destroySplineComponent)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateToSplinePoint;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToRotate;
		ie->splineComponent = splineComponent;
		ie->interpolateToSpline = rotateToPath;
		ie->generatedPointDistance = generatedPointDistance;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->destroySplineObject = destroySplineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		if (!componentToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateToSplinePointExpert(USceneComponent* componentToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* orientationTarget /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateToSplinePoint;
		ie->componentTweening = componentToRotate;
		ie->splineComponent = splineComponent;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!componentToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentRotateToSplinePointMin(USceneComponent* componentToRotate /*= nullptr*/, USplineComponent* splineComponent /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween and a valid spline before proceeding
	if (componentToRotate->IsValidLowLevel() && splineComponent->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compRotateToSplinePoint;
		ie->componentTweening = componentToRotate;
		ie->splineComponent = splineComponent;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		if (!componentToRotate->IsValidLowLevel())
		{
			Print("No tweenable object defined. No tweening operation will occur.", "error");
		}
		else if (!splineComponent->IsValidLowLevel())
		{
			Print("No spline defined. No tweening operation will occur.", "error");
		}

		return nullptr;
	}
}

//Component Rotate Update
void UiTween::ComponentRotateUpdate(USceneComponent* componentToRotate, FRotator rotationTo, float delta, float speed, bool isLocal, ERotatorConstraints::RotatorConstraints rotatorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToRotate->IsValidLowLevel())
	{
		if (isLocal && componentToRotate->GetOwner()->IsValidLowLevel())
		{
			rotationTo = FRotator(FQuat(componentToRotate->GetOwner()->GetActorRotation()) * FQuat(rotationTo));
		}

		componentToRotate->SetWorldRotation(ConstrainRotator(FMath::RInterpTo(componentToRotate->GetComponentRotation(), rotationTo, delta, speed), componentToRotate->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Component Scale From/To
AiTweenEvent* UiTween::ComponentScaleFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, USceneComponent* componentToScale /*= nullptr*/, FVector scaleFrom /*= FVector::ZeroVector*/, FVector scaleTo /*= FVector(1, 1, 1)*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints /*= VectorConstraints::none*/, bool isLocal, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compScaleFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToScale;
		ie->vectorFrom = scaleFrom;
		ie->vectorTo = scaleTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vectorConstraints = vectorConstraints;
		if (isLocal)
		{
			ie->coordinateSpace = CoordinateSpace::parent;
		}
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentScaleFromToSimple(FName tweenName /*= "No Name"*/, USceneComponent* componentToScale /*= nullptr*/, FVector scaleFrom /*= FVector::ZeroVector*/, FVector scaleTo /*= FVector(1, 1, 1)*/, bool isLocal, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compScaleFromTo;
		ie->tweenName = tweenName;
		ie->componentTweening = componentToScale;
		ie->vectorFrom = scaleFrom;
		ie->vectorTo = scaleTo;
		if (isLocal)
		{
			ie->coordinateSpace = CoordinateSpace::parent;
		}
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentScaleFromToExpert(USceneComponent* componentToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compScaleFromTo;
		ie->componentTweening = componentToScale;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::ComponentScaleFromToMin(USceneComponent* componentToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::compScaleFromTo;
		ie->componentTweening = componentToScale;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//Component Scale Update
void UiTween::ComponentScaleUpdate(USceneComponent* componentToScale, FVector scaleTo, float delta, float speed, bool isLocal, EVectorConstraints::VectorConstraints vectorConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (componentToScale->IsValidLowLevel())
	{
		if (isLocal && componentToScale->GetOwner()->IsValidLowLevel())
		{
			scaleTo = componentToScale->GetOwner()->GetActorScale3D() * scaleTo;
		}

		componentToScale->SetWorldScale3D(ConstrainVector(FMath::VInterpTo(componentToScale->GetComponentScale(), scaleTo, delta, speed), componentToScale->GetComponentScale(), vectorConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//UMG RT
//UMG RT Move From/To
AiTweenEvent* UiTween::UMGRTMoveFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, UWidget* widgetToMove /*= nullptr*/, FVector2D locationFrom /*= FVector2D::ZeroVector*/, FVector2D locationTo /*= FVector2D::ZeroVector*/, bool enforceValueTo, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, /*ELookType::LookType orientation, UObject* orientationTarget, float orientationSpeed, */UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTMoveFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToMove;
		ie->vector2DFrom = locationFrom;
		ie->vector2DTo = locationTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vector2DConstraints = vector2DConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		//ie->orientation = orientation;
		//ie->orientationTarget = orientationTarget;
		//ie->orientationSpeed = orientationSpeed;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTMoveFromToSimple(FName tweenName /*= "No Name"*/, UWidget* widgetToMove /*= nullptr*/, FVector2D locationFrom /*= FVector2D::ZeroVector*/, FVector2D locationTo /*= FVector2D::ZeroVector*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTMoveFromTo;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToMove;
		ie->vector2DFrom = locationFrom;
		ie->vector2DTo = locationTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTMoveFromToExpert(UWidget* widgetToMove /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, /*UObject* orientationTarget = nullptr, */UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTMoveFromTo;
		ie->widgetTweening = widgetToMove;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTMoveFromToMin(UWidget* widgetToMove /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTMoveFromTo;
		ie->widgetTweening = widgetToMove;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//UMG RT Move Update
void UiTween::UMGRTMoveUpdate(UWidget* widgetToMove, FVector2D locationTo, float delta, float speed, EVector2DConstraints::Vector2DConstraints vector2DConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove->IsValidLowLevel())
	{
		FVector v = FMath::VInterpTo(FVector(widgetToMove->RenderTransform.Translation.X, widgetToMove->RenderTransform.Translation.Y, 0), FVector(locationTo.X, locationTo.Y, 0), delta, speed);

		widgetToMove->SetRenderTranslation(ConstrainVector2D(FVector2D(v.X, v.Y), widgetToMove->RenderTransform.Translation, vector2DConstraints));

		//DevNote: Orientation logic when the time comes
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//UMG RT Rotate From/To
AiTweenEvent* UiTween::UMGRTRotateFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, UWidget* widgetToRotate /*= nullptr*/, float angleFrom /*= 0*/, float angleTo /*= 90*/, bool enforceValueTo, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTRotateFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToRotate;
		ie->floatFrom = angleFrom;
		ie->floatTo = angleTo;
		ie->enforceValueTo = enforceValueTo;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTRotateFromToSimple(FName tweenName /*= "No Name"*/, UWidget* widgetToRotate /*= nullptr*/, float angleFrom /*= 0*/, float angleTo /*= 90*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTRotateFromTo;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToRotate;
		ie->floatFrom = angleFrom;
		ie->floatTo = angleTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTRotateFromToExpert(UWidget* widgetToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTRotateFromTo;
		ie->widgetTweening = widgetToRotate;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTRotateFromToMin(UWidget* widgetToRotate /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToRotate->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTRotateFromTo;
		ie->widgetTweening = widgetToRotate;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//UMG RT Rotate Update
void UiTween::UMGRTRotateUpdate(UWidget* widgetToRotate, float angleTo, float delta, float speed)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToRotate->IsValidLowLevel())
	{
		widgetToRotate->SetRenderAngle(FMath::FInterpTo(widgetToRotate->RenderTransform.Angle, angleTo, delta, speed));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//UMG RT Scale From/To
AiTweenEvent* UiTween::UMGRTScaleFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, UWidget* widgetToScale /*= nullptr*/, FVector2D scaleFrom /*= FVector2D(1,1)*/, FVector2D scaleTo /*= FVector2D(2,2)*/, bool enforceValueTo, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTScaleFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToScale;
		ie->vector2DFrom = scaleFrom;
		ie->vector2DTo = scaleTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vector2DConstraints = vector2DConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTScaleFromToSimple(FName tweenName /*= "No Name"*/, UWidget* widgetToScale /*= nullptr*/, FVector2D scaleFrom /*= FVector2D(1, 1)*/, FVector2D scaleTo /*= FVector2D(2, 2)*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTScaleFromTo;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToScale;
		ie->vector2DFrom = scaleFrom;
		ie->vector2DTo = scaleTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTScaleFromToExpert(UWidget* widgetToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTScaleFromTo;
		ie->widgetTweening = widgetToScale;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTScaleFromToMin(UWidget* widgetToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTScaleFromTo;
		ie->widgetTweening = widgetToScale;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//UMG RT Scale Update
void UiTween::UMGRTScaleUpdate(UWidget* widgetToScale, FVector2D scaleTo, float delta, float speed, EVector2DConstraints::Vector2DConstraints vector2DConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale->IsValidLowLevel())
	{
		FVector v = FMath::VInterpTo(FVector(widgetToScale->RenderTransform.Scale.X, widgetToScale->RenderTransform.Scale.Y, 0), FVector(scaleTo.X, scaleTo.Y, 0), delta, speed);

		widgetToScale->SetRenderScale(ConstrainVector2D(FVector2D(v.X, v.Y), widgetToScale->RenderTransform.Scale, vector2DConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//UMG RT Shear From/To
AiTweenEvent* UiTween::UMGRTShearFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, UWidget* widgetToShear /*= nullptr*/, FVector2D shearFrom /*= FVector2D(1, 1)*/, FVector2D shearTo /*= FVector2D(2, 2)*/, bool enforceValueTo, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/, bool tieToObjectValidity)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToShear->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTShearFromTo;
		ie->timerInterval = timerInterval;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToShear;
		ie->vector2DFrom = shearFrom;
		ie->vector2DTo = shearTo;
		ie->enforceValueTo = enforceValueTo;
		ie->vector2DConstraints = vector2DConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->maximumLoopSections = maximumLoopSections;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;
		ie->tieToObjectValidity = tieToObjectValidity;

		//Initialize Event
		ie->InitEvent();
		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTShearFromToSimple(FName tweenName /*= "No Name"*/, UWidget* widgetToShear /*= nullptr*/, FVector2D shearFrom /*= FVector2D(1, 1)*/, FVector2D shearTo /*= FVector2D(2, 2)*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToShear->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTShearFromTo;
		ie->tweenName = tweenName;
		ie->widgetTweening = widgetToShear;
		ie->vector2DFrom = shearFrom;
		ie->vector2DTo = shearTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTShearFromToExpert(UWidget* widgetToShear /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToShear->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTShearFromTo;
		ie->widgetTweening = widgetToShear;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartTarget = onTweenStartTarget;
		ie->onTweenUpdateTarget = onTweenUpdateTarget;
		ie->onTweenLoopTarget = onTweenLoopTarget;
		ie->onTweenCompleteTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::UMGRTShearFromToMin(UWidget* widgetToShear /*= nullptr*/, FString parameters, bool initializeOnSpawn)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToShear->IsValidLowLevel())
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::umgRTShearFromTo;
		ie->widgetTweening = widgetToShear;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//UMG RT Shear Update
void UiTween::UMGRTShearUpdate(UWidget* widgetToShear, FVector2D shearTo, float delta, float speed, EVector2DConstraints::Vector2DConstraints vector2DConstraints)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToShear->IsValidLowLevel())
	{
		FVector v = FMath::VInterpTo(FVector(widgetToShear->RenderTransform.Shear.X, widgetToShear->RenderTransform.Shear.Y, 0), FVector(shearTo.X, shearTo.Y, 0), delta, speed);

		widgetToShear->SetRenderShear(ConstrainVector2D(FVector2D(v.X, v.Y), widgetToShear->RenderTransform.Shear, vector2DConstraints));
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
	}
}

//Data Type
//Float From/To
AiTweenEvent* UiTween::FloatFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, float startValue /*= 0.f*/, float endValue /*= 1.f*/, bool enforceValueTo, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->timerInterval = timerInterval;
	ie->tweenName = tweenName;
	ie->floatFrom = startValue;
	ie->floatTo = endValue;
	ie->enforceValueTo = enforceValueTo;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->maximumLoopSections = maximumLoopSections;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::FloatFromToSimple(FName tweenName /*= "No Name"*/, float startValue /*= 0.f*/, float endValue /*= 1.f*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->tweenName = tweenName;
	ie->floatFrom = startValue;
	ie->floatTo = endValue;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();

	return ie;
}

AiTweenEvent* UiTween::FloatFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

AiTweenEvent* UiTween::FloatFromToMin(FString parameters, bool initializeOnSpawn)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}


//Vector From/To
AiTweenEvent* UiTween::VectorFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, FVector startValue /*= FVector::ZeroVector*/, FVector endValue /*= FVector::ZeroVector*/, bool enforceValueTo, EVectorConstraints::VectorConstraints vectorConstraints/*= VectorConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vectorFromTo;
	ie->timerInterval = timerInterval;
	ie->tweenName = tweenName;
	ie->vectorFrom = startValue;
	ie->vectorTo = endValue;
	ie->enforceValueTo = enforceValueTo;
	ie->vectorConstraints = vectorConstraints;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->maximumLoopSections = maximumLoopSections;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::VectorFromToSimple(FName tweenName /*= "No Name"*/, FVector startValue /*= FVector::ZeroVector*/, FVector endValue /*= FVector::ZeroVector*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vectorFromTo;
	ie->tweenName = tweenName;
	ie->vectorFrom = startValue;
	ie->vectorTo = endValue;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();

	return ie;
}

AiTweenEvent* UiTween::VectorFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vectorFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

AiTweenEvent* UiTween::VectorFromToMin(FString parameters, bool initializeOnSpawn)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vectorFromTo;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

//Vector2D From/To 
AiTweenEvent* UiTween::Vector2DFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, FVector2D startValue /*= FVector2D::ZeroVector*/, FVector2D endValue /*= FVector2D::ZeroVector*/, bool enforceValueTo, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vector2DFromTo;
	ie->timerInterval = timerInterval;
	ie->tweenName = tweenName;
	ie->vector2DFrom = startValue;
	ie->vector2DTo = endValue;
	ie->enforceValueTo = enforceValueTo;
	ie->vector2DConstraints = vector2DConstraints;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->maximumLoopSections = maximumLoopSections;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::Vector2DFromToSimple(FName tweenName /*= "No Name"*/, FVector2D startValue /*= FVector2D::ZeroVector*/, FVector2D endValue /*= FVector2D::ZeroVector*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vector2DFromTo;
	ie->tweenName = tweenName;
	ie->vector2DFrom = startValue;
	ie->vector2DTo = endValue;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();

	return ie;
}

AiTweenEvent* UiTween::Vector2DFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vector2DFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

AiTweenEvent* UiTween::Vector2DFromToMin(FString parameters, bool initializeOnSpawn)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::vector2DFromTo;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

//Rotator From/To
AiTweenEvent* UiTween::RotatorFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, FRotator startValue /*= FRotator::ZeroRotator*/, FRotator endValue /*= FRotator::ZeroRotator*/, bool enforceValueTo, ERotatorConstraints::RotatorConstraints rotatorConstraints /*= RotatorConstraints::none*/, bool shortestPath, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::rotatorFromTo;
	ie->timerInterval = timerInterval;
	ie->tweenName = tweenName;
	ie->rotatorFrom = startValue;
	ie->rotatorTo = endValue;
	ie->enforceValueTo = enforceValueTo;
	ie->rotatorConstraints = rotatorConstraints;
	ie->shortestPath = shortestPath;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->maximumLoopSections = maximumLoopSections;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::RotatorFromToSimple(FName tweenName /*= "No Name"*/, FRotator startValue /*= FRotator::ZeroRotator*/, FRotator endValue /*= FRotator::ZeroRotator*/, bool shortestPath, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::rotatorFromTo;
	ie->tweenName = tweenName;
	ie->rotatorFrom = startValue;
	ie->rotatorTo = endValue;
	ie->shortestPath = shortestPath;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();

	return ie;
}

AiTweenEvent* UiTween::RotatorFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::rotatorFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

AiTweenEvent* UiTween::RotatorFromToMin(FString parameters, bool initializeOnSpawn)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::rotatorFromTo;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

//Linear Color From/To
AiTweenEvent* UiTween::LinearColorFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, FLinearColor startValue /*= FLinearColor::White*/, FLinearColor endValue /*= FLinearColor::Black*/, bool enforceValueTo, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, int32 maximumLoopSections, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::linearColorFromTo;
	ie->timerInterval = timerInterval;
	ie->tweenName = tweenName;
	ie->linearColorFrom = startValue;
	ie->linearColorTo = endValue;
	ie->enforceValueTo = enforceValueTo;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->maximumLoopSections = maximumLoopSections;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::LinearColorFromToSimple(FName tweenName /*= "No Name"*/, FLinearColor startValue /*= FLinearColor::White*/, FLinearColor endValue /*= FLinearColor::Black*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::linearColorFromTo;
	ie->tweenName = tweenName;
	ie->linearColorFrom = startValue;
	ie->linearColorTo = endValue;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();
	return ie;
}

AiTweenEvent* UiTween::LinearColorFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, UObject* onTweenStartTarget /*= nullptr*/, UObject* onTweenUpdateTarget /*= nullptr*/, UObject* onTweenLoopTarget /*= nullptr*/, UObject* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::linearColorFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartTarget = onTweenStartTarget;
	ie->onTweenUpdateTarget = onTweenUpdateTarget;
	ie->onTweenLoopTarget = onTweenLoopTarget;
	ie->onTweenCompleteTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

AiTweenEvent* UiTween::LinearColorFromToMin(FString parameters /*= ""*/, bool initializeOnSpawn /*= true*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::linearColorFromTo;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

//Data Type
//Float From/To
AiTweenEvent* UiTween::SlateFloatFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, float startValue /*= 0.f*/, float endValue /*= 1.f*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, STweenableWidget* onTweenStartTarget /*= nullptr*/, STweenableWidget* onTweenTickTarget /*= nullptr*/, STweenableWidget* onTweenLoopTarget /*= nullptr*/, STweenableWidget* onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->tweenName = tweenName;
	ie->floatFrom = startValue;
	ie->floatTo = endValue;
	ie->delay = delay;
	ie->delayType = delayType;
	ie->tickType = tickType;
	ie->tickTypeValue = tickTypeValue;
	ie->easeType = easeType;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->punchAmplitude = punchAmplitude;
	ie->loopType = loopType;
	ie->onTweenStartSlateTarget = onTweenStartTarget;
	ie->onTweenTickSlateTarget = onTweenTickTarget;
	ie->onTweenLoopSlateTarget = onTweenLoopTarget;
	ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;
	ie->tickWhenPaused = tickWhenPaused;
	ie->ignoreTimeDilation = ignoreTimeDilation;

	//Initialize Event
	ie->InitEvent();

	return ie;
}

AiTweenEvent* UiTween::SlateFloatFromToSimple(FName tweenName /*= "No Name"*/, float startValue /*= 0.f*/, float endValue /*= 1.f*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, STweenableWidget* onTweenStartTarget /*= nullptr*/, STweenableWidget* onTweenTickTarget /*= nullptr*/, STweenableWidget* onTweenCompleteTarget /*= nullptr*/)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->tweenName = tweenName;
	ie->floatFrom = startValue;
	ie->floatTo = endValue;
	ie->tickTypeValue = timeInSeconds;
	ie->easeType = easeType;
	ie->onTweenStartSlateTarget = onTweenStartTarget;
	ie->onTweenTickSlateTarget = onTweenTickTarget;
	ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event
	ie->InitEvent();

	return ie;
}


AiTweenEvent* UiTween::SlateFloatFromToExpert(FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, STweenableWidget* onTweenStartTarget /*= nullptr*/, STweenableWidget* onTweenTickTarget /*= nullptr*/, STweenableWidget* onTweenLoopTarget /*= nullptr*/, STweenableWidget* onTweenCompleteTarget)
{
	//Create Actor Instance
	AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
	//Set Event Type
	ie->eventType = EEventType::EventType::floatFromTo;
	ie->customEaseTypeCurve = customEaseTypeCurve;
	ie->onTweenStartSlateTarget = onTweenStartTarget;
	ie->onTweenTickSlateTarget = onTweenTickTarget;
	ie->onTweenLoopSlateTarget = onTweenLoopTarget;
	ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

	//Set tweening variables based on string parameters
	ie->ParseParameters(parameters);

	//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
	//AiTweenEvent AActor*
	if (initializeOnSpawn)
	{
		ie->InitEvent();
	}

	return ie;
}

//UMG RT
//UMG RT Move From/To
AiTweenEvent* UiTween::SlateMoveFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, STweenableWidget*  widgetToMove /*= nullptr*/, FVector2D locationFrom /*= FVector2D::ZeroVector*/, FVector2D locationTo /*= FVector2D::ZeroVector*/, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, /*ELookType::LookType orientation, STweenableWidget*  orientationTarget, float orientationSpeed, */STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenLoopTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateMoveFromTo;
		ie->tweenName = tweenName;
		ie->slateTweening = widgetToMove;
		ie->vector2DFrom = locationFrom;
		ie->vector2DTo = locationTo;
		ie->vector2DConstraints = vector2DConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		//ie->orientation = orientation;
		//ie->orientationTarget = orientationTarget;
		//ie->orientationSpeed = orientationSpeed;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenLoopSlateTarget = onTweenLoopTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::SlateMoveFromToSimple(FName tweenName /*= "No Name"*/, STweenableWidget*  widgetToMove /*= nullptr*/, FVector2D locationFrom /*= FVector2D::ZeroVector*/, FVector2D locationTo /*= FVector2D::ZeroVector*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateMoveFromTo;
		ie->tweenName = tweenName;
		ie->slateTweening = widgetToMove;
		ie->vector2DFrom = locationFrom;
		ie->vector2DTo = locationTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::SlateMoveFromToExpert(STweenableWidget*  widgetToMove /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, /*STweenableWidget*  orientationTarget = nullptr, */STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenLoopTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToMove)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateMoveFromTo;
		ie->slateTweening = widgetToMove;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenLoopSlateTarget = onTweenLoopTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

//UMG RT Scale From/To
AiTweenEvent* UiTween::SlateScaleFromToFull(float timerInterval, FName tweenName /*= "No Name"*/, STweenableWidget*  widgetToScale /*= nullptr*/, FVector2D scaleFrom /*= FVector2D(1,1)*/, FVector2D scaleTo /*= FVector2D(2,2)*/, EVector2DConstraints::Vector2DConstraints vector2DConstraints /*= Vector2DConstraints::none*/, float delay /*= 0.0f*/, EDelayType::DelayType delayType /*= first*/, ETickType::TickType tickType /*= seconds*/, float tickTypeValue /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, UCurveFloat* customEaseTypeCurve /*= nullptr*/, float punchAmplitude /*= 1.0f*/, ELoopType::LoopType loopType /*= once*/, STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenLoopTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget /*= nullptr*/, bool tickWhenPaused /*= false*/, bool ignoreTimeDilation /*= false*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateScaleFromTo;
		ie->tweenName = tweenName;
		ie->slateTweening = widgetToScale;
		ie->vector2DFrom = scaleFrom;
		ie->vector2DTo = scaleTo;
		ie->vector2DConstraints = vector2DConstraints;
		ie->delay = delay;
		ie->delayType = delayType;
		ie->tickType = tickType;
		ie->tickTypeValue = tickTypeValue;
		ie->easeType = easeType;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->punchAmplitude = punchAmplitude;
		ie->loopType = loopType;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenLoopSlateTarget = onTweenLoopTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;
		ie->tickWhenPaused = tickWhenPaused;
		ie->ignoreTimeDilation = ignoreTimeDilation;

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::SlateScaleFromToSimple(FName tweenName /*= "No Name"*/, STweenableWidget*  widgetToScale /*= nullptr*/, FVector2D scaleFrom /*= FVector2D(1, 1)*/, FVector2D scaleTo /*= FVector2D(2, 2)*/, float timeInSeconds /*= 5.0f*/, EEaseType::EaseType easeType /*= linear*/, FString parameters, STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget /*= nullptr*/)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateScaleFromTo;
		ie->tweenName = tweenName;
		ie->slateTweening = widgetToScale;
		ie->vector2DFrom = scaleFrom;
		ie->vector2DTo = scaleTo;
		ie->tickTypeValue = timeInSeconds;
		ie->easeType = easeType;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event
		ie->InitEvent();

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}

AiTweenEvent* UiTween::SlateScaleFromToExpert(STweenableWidget*  widgetToScale /*= nullptr*/, FString parameters, bool initializeOnSpawn, UCurveFloat* customEaseTypeCurve /*= nullptr*/, STweenableWidget*  onTweenStartTarget /*= nullptr*/, STweenableWidget*  onTweenTickTarget /*= nullptr*/, STweenableWidget*  onTweenLoopTarget /*= nullptr*/, STweenableWidget*  onTweenCompleteTarget)
{
	//Make sure we have a valid object to tween before proceeding
	if (widgetToScale)
	{
		//Create Actor Instance
		AiTweenEvent* ie = (AiTweenEvent*)SpawnEvent(GetAux());
		//Set Event Type
		ie->eventType = EEventType::EventType::slateScaleFromTo;
		ie->slateTweening = widgetToScale;
		ie->customEaseTypeCurve = customEaseTypeCurve;
		ie->onTweenStartSlateTarget = onTweenStartTarget;
		ie->onTweenTickSlateTarget = onTweenTickTarget;
		ie->onTweenLoopSlateTarget = onTweenLoopTarget;
		ie->onTweenCompleteSlateTarget = onTweenCompleteTarget;

		//Set tweening variables based on string parameters
		ie->ParseParameters(parameters);

		//Initialize Event if initializeOnSpawn is true, otherwise user must manually initialize by calling InitEvent() on 
		//AiTweenEvent AActor*
		if (initializeOnSpawn)
		{
			ie->InitEvent();
		}

		return ie;
	}
	else
	{
		Print("No tweenable object defined. No tweening operation will occur.", "error");
		return nullptr;
	}
}
