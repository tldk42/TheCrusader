#pragma once

#include "CoreMinimal.h"
#include "TCInterpolator.generated.h"

namespace TCInterpolatorHelpers
{
	template <class T>
	T GetZeroForType()
	{
	}

	template <>
	inline float GetZeroForType() { return 0.f; };

	template <>
	inline FVector GetZeroForType() { return FVector::ZeroVector; };

	template <>
	inline FRotator GetZeroForType() { return FRotator::ZeroRotator; };

	template <>
	inline FQuat GetZeroForType() { return FQuat::Identity; };

	template <class T>
	T NormalizeIfRotator(T Input) { return Input; }

	// 템플릿 특수화 적용
	template <>
	inline FRotator NormalizeIfRotator(FRotator Input) { return Input.GetNormalized(); };
}

template <typename T>
struct TGenericIIRInterpolator
{
public:
	TGenericIIRInterpolator() = default;

	TGenericIIRInterpolator(float InInterpSpeed)
		: InterpSpeed(InInterpSpeed)
	{
	};

	T Eval(T NewGoalValue, float DeltaTime)
	{
		if (bPendingReset)
		{
			PerformReset(NewGoalValue);
		}
		else
		{
			CurrentValue = SingleStepEval(NewGoalValue, DeltaTime);
			LastUpdateLeftoverTime = 0.f;
		}

		return CurrentValue;
	}

	T EvalSubstepped(T NewGoalValue, float DeltaTime)
	{
		if (bPendingReset)
		{
			PerformReset(NewGoalValue);
		}
		else
		{
			float RemainingTime = DeltaTime;

			if (bDoLeftoverRewind && (LastUpdateLeftoverTime > 0.f))
			{
				RemainingTime += LastUpdateLeftoverTime;
				CurrentValue = ValueAfterLastFullStep;

				LastUpdateLeftoverTime = 0.f;
			}

			const T EquilibriumStepRate = TCInterpolatorHelpers::NormalizeIfRotator<T>(NewGoalValue - LastGoalValue) * (
				1.f / RemainingTime);
			T LerpedGoalValue = LastGoalValue;

			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float StepTime = FMath::Min(MaxSubstepTime, RemainingTime);

				if (bDoLeftoverRewind && (StepTime < MaxSubstepTime))
				{
					LastUpdateLeftoverTime = StepTime;
					ValueAfterLastFullStep = CurrentValue;
				}

				LerpedGoalValue += EquilibriumStepRate * StepTime;
				RemainingTime -= StepTime;

				CurrentValue = SingleStepEval(LerpedGoalValue, StepTime);

				LastGoalValue = NewGoalValue;
			}
		}
		return CurrentValue;
	}

	void SetInterpSpeed(float NewInterpSpeed)
	{
		InterpSpeed = NewInterpSpeed;
	}

	void SetInitialValue(T InitialValue)
	{
		CurrentValue = InitialValue;
		bPendingReset = false;
	}

	T GetCurrentValue() const
	{
		return CurrentValue;
	}

	void Reset()
	{
		bPendingReset = true;
	}

protected:
	/** Maximum timeslice per substep. */
	static constexpr float MaxSubstepTime = 1.f / 120.f;

	/** 목표 값. */
	T GoalValue;

	/** 현재 값. */
	T CurrentValue;

	/** 보간 속도 */
	float InterpSpeed = 6.f;

	/** 다음 평가에서 목표 값으로 스냅. */
	bool bPendingReset = true;

	bool bDoLeftoverRewind = true;
	float LastUpdateLeftoverTime = 0.f;
	T ValueAfterLastFullStep;
	T LastGoalValue;

protected:
	T SingleStepEval(T NewGoalValue, float StepTime)
	{
		check(false);
		return T();
	}

	void PerformReset(T NewGoalValue)
	{
		CurrentValue = NewGoalValue;
		LastGoalValue = NewGoalValue;
		LastUpdateLeftoverTime = 0.f; 
		bPendingReset = false;
	}
};

template<>
inline float TGenericIIRInterpolator<float>::SingleStepEval(float StepGoalValue, float StepTime)
{
	return FMath::FInterpTo(CurrentValue, StepGoalValue, StepTime, InterpSpeed);
}

template<> 
inline FVector TGenericIIRInterpolator<FVector>::SingleStepEval(FVector StepGoalValue, float StepTime)
{
	return FMath::VInterpTo(CurrentValue, StepGoalValue, StepTime, InterpSpeed);
}

template<>
inline FRotator TGenericIIRInterpolator<FRotator>::SingleStepEval(FRotator StepGoalValue, float StepTime)
{
	return FMath::RInterpTo(CurrentValue, StepGoalValue, StepTime, InterpSpeed);
}

template<>
inline FLinearColor TGenericIIRInterpolator<FLinearColor>::SingleStepEval(FLinearColor StepGoalValue, float StepTime)
{
	return FMath::CInterpTo(CurrentValue, StepGoalValue, StepTime, InterpSpeed);
}

template<>
inline FQuat TGenericIIRInterpolator<FQuat>::SingleStepEval(FQuat StepGoalValue, float StepTime)
{
	return FMath::QInterpTo(CurrentValue, StepGoalValue, StepTime, InterpSpeed);
}

USTRUCT(BlueprintType)
struct FIIRInterpolatorFloat
{
	GENERATED_BODY()

public:
	FIIRInterpolatorFloat()
	{
		Interpolator = TGenericIIRInterpolator<float>(InterpSpeed);
	}

	FIIRInterpolatorFloat(float InInterpSpeed)
	: InterpSpeed(InInterpSpeed)
	{
		Interpolator = TGenericIIRInterpolator<float>(InterpSpeed);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InterpSpeed = 6.f;

	float Eval(float Goal, float DeltaTime)
	{
		// set every time so we can tweak values while game is live
		Interpolator.SetInterpSpeed(InterpSpeed);
		return Interpolator.Eval(Goal, DeltaTime);
	}

	void Reset() { Interpolator.Reset(); }
	void SetInitialValue(float InitialValue) { Interpolator.SetInitialValue(InitialValue); }
	float GetCurrentValue() const { return Interpolator.GetCurrentValue(); }
	
private:
	TGenericIIRInterpolator<float> Interpolator;
};

USTRUCT(BlueprintType)
struct FIIRInterpolatorVector
{
	GENERATED_BODY();

public:
	FIIRInterpolatorVector()
	{
		Interpolator = TGenericIIRInterpolator<FVector>(InterpSpeed);
	}

	FIIRInterpolatorVector(float InInterpSpeed)
		: InterpSpeed(InInterpSpeed)
		, Interpolator(InInterpSpeed)
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InterpSpeed = 6.f;

	FVector Eval(FVector GoalLocation, float DeltaTime)
	{
		// set every time so we can tweak values while game is live
		Interpolator.SetInterpSpeed(InterpSpeed);
		return Interpolator.Eval(GoalLocation, DeltaTime);
	}

	void Reset() { Interpolator.Reset(); }
	void SetInitialValue(FVector InitialValue) { Interpolator.SetInitialValue(InitialValue); }
	FVector GetCurrentValue() const { return Interpolator.GetCurrentValue(); }

private:
	TGenericIIRInterpolator<FVector> Interpolator;
};

template<class T>
struct TGenericDoubleIIRInterpolator
{
public:
	TGenericDoubleIIRInterpolator()
	{
		SetInterpSpeeds(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	TGenericDoubleIIRInterpolator(float InPrimaryInterpSpeed, float InIntermediateInterpSpeed)
	{
		SetInterpSpeeds(InPrimaryInterpSpeed, InIntermediateInterpSpeed);
	}

	void SetInterpSpeeds(float NewPrimaryInterpSpeed, float NewIntermediateInterpSpeed)
	{
		PrimaryInterpSpeed = NewPrimaryInterpSpeed;
		PrimaryInterpolator.SetInterpSpeed(PrimaryInterpSpeed);
		IntermediateInterpSpeed = NewIntermediateInterpSpeed;
		IntermediateInterpolator.SetInterpSpeed(IntermediateInterpSpeed);
	}

	void SetInitialValue(T InitialValue)
	{
		IntermediateInterpolator.SetInitialValue(InitialValue);
		PrimaryInterpolator.SetInitialValue(InitialValue);
	}

	/**
	 * Updates the interpolator for the given new goal value and time slice.
	 * Does a full eval in a single timeslice.
	 */
	T Eval(T NewGoalValue, float DeltaTime)
	{
		// underlying interpolators will handle resets
		return SingleStepEval(NewGoalValue, DeltaTime);
	}

	/** Does sub-stepping, with partial-interval rewinding */
	T EvalSubstepped(T NewGoalValue, float DeltaTime)
	{
		// underlying interpolators will handle resets
		{
			float RemainingTime = DeltaTime;

			// move the goal linearly toward goal while we substep
			const T EquilibriumStepRate = TCInterpolatorHelpers::NormalizeIfRotator<T>(NewGoalValue - LastGoalValue) * (1.f / RemainingTime);
			T LerpedGoalValue = LastGoalValue;

			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float StepTime = FMath::Min(MaxSubstepTime, RemainingTime);

				LerpedGoalValue += EquilibriumStepRate * StepTime;
				RemainingTime -= StepTime;

				//UE_LOG(LogTemp, Log, TEXT("...   internal eval starting at %s, towards %s, steptime = %f"), *CurrentPos.ToString(), *LerpedEquilibriumPos.ToString(), StepTime);
				SingleStepEval(LerpedGoalValue, StepTime);
				//UE_LOG(LogTemp, Log, TEXT("...      resultant pos = %s"), *CurrentPos.ToString());

				LastGoalValue = NewGoalValue;
			}
		}

		return PrimaryInterpolator.GetCurrentValue();
	}

	void Reset()
	{
		IntermediateInterpolator.Reset();
		PrimaryInterpolator.Reset();
	}

	T GetCurrentValue() const
	{
		return PrimaryInterpolator.GetCurrentValue();
	};

protected:

	T SingleStepEval(T StepGoalValue, float StepTime)
	{
		// make sure step time of the double is same as step time of the underlying singles.
		// that ensures the partial step rewind works and isn't running too often
		T IntermedValue = IntermediateInterpolator.EvalSubstepped(StepGoalValue, StepTime);
		return PrimaryInterpolator.EvalSubstepped(IntermedValue, StepTime);
	}

protected:
	// do we need to store this? it's also in the interpolators
	float PrimaryInterpSpeed = 4.f;
	float IntermediateInterpSpeed = 12.f;

	T LastGoalValue;

	/** Maximum timeslice per substep. */
	static constexpr float MaxSubstepTime = 1.f / 120.f;

	TGenericIIRInterpolator<T> IntermediateInterpolator;
	TGenericIIRInterpolator<T> PrimaryInterpolator;
};

USTRUCT(BlueprintType)
struct FDoubleIIRInterpolatorVector
{
	GENERATED_BODY();

public:
	FDoubleIIRInterpolatorVector()
	{
		Interpolator = TGenericDoubleIIRInterpolator<FVector>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	FDoubleIIRInterpolatorVector(float InPrimaryInterpSpeed, float InIntermediateInterpSpeed)
		: PrimaryInterpSpeed(InPrimaryInterpSpeed)
		, IntermediateInterpSpeed(InIntermediateInterpSpeed)
	{
		Interpolator = TGenericDoubleIIRInterpolator<FVector>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PrimaryInterpSpeed = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IntermediateInterpSpeed = 12.f;

	FVector Eval(FVector GoalLocation, float DeltaTime)
	{
		// set every time so we can tweak values while game is live
		Interpolator.SetInterpSpeeds(PrimaryInterpSpeed, IntermediateInterpSpeed);
		return Interpolator.Eval(GoalLocation, DeltaTime);
	}

	void Reset() { Interpolator.Reset(); }
	void SetInitialValue(FVector InitialValue) { Interpolator.SetInitialValue(InitialValue); }
	FVector GetCurrentValue() const { return Interpolator.GetCurrentValue(); }

private:
	TGenericDoubleIIRInterpolator<FVector> Interpolator;
};

USTRUCT(BlueprintType)
struct FDoubleIIRInterpolatorRotator
{
	GENERATED_BODY();

public:
	FDoubleIIRInterpolatorRotator()
	{
		Interpolator = TGenericDoubleIIRInterpolator<FRotator>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	FDoubleIIRInterpolatorRotator(float InPrimaryInterpSpeed, float InIntermediateInterpSpeed)
		: PrimaryInterpSpeed(InPrimaryInterpSpeed)
		, IntermediateInterpSpeed(InIntermediateInterpSpeed)
	{
		Interpolator = TGenericDoubleIIRInterpolator<FRotator>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PrimaryInterpSpeed = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IntermediateInterpSpeed = 12.f;

	FRotator Eval(FRotator GoalRotation, float DeltaTime)
	{
		// set every time so we can tweak values while game is live
		Interpolator.SetInterpSpeeds(PrimaryInterpSpeed, IntermediateInterpSpeed);
		return Interpolator.Eval(GoalRotation, DeltaTime);
	}

	void Reset() { Interpolator.Reset(); }
	void SetInitialValue(FRotator InitialValue) { Interpolator.SetInitialValue(InitialValue); }
	FRotator GetCurrentValue() const { return Interpolator.GetCurrentValue(); }

private:
	TGenericDoubleIIRInterpolator<FRotator> Interpolator;
};

USTRUCT(BlueprintType)
struct FDoubleIIRInterpolatorFloat
{
	GENERATED_BODY();

public:
	FDoubleIIRInterpolatorFloat()
	{
		Interpolator = TGenericDoubleIIRInterpolator<float>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	FDoubleIIRInterpolatorFloat(float InPrimaryInterpSpeed, float InIntermediateInterpSpeed)
		: PrimaryInterpSpeed(InPrimaryInterpSpeed)
		, IntermediateInterpSpeed(InIntermediateInterpSpeed)
	{
		Interpolator = TGenericDoubleIIRInterpolator<float>(PrimaryInterpSpeed, IntermediateInterpSpeed);
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PrimaryInterpSpeed = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IntermediateInterpSpeed = 12.f;

	float Eval(float Goal, float DeltaTime)
	{
		// set every time so we can tweak values while game is live
		Interpolator.SetInterpSpeeds(PrimaryInterpSpeed, IntermediateInterpSpeed);
		return Interpolator.Eval(Goal, DeltaTime);
	}

	void Reset() { Interpolator.Reset(); }
	void SetInitialValue(float InitialValue) { Interpolator.SetInitialValue(InitialValue); }
	float GetCurrentValue() const { return Interpolator.GetCurrentValue(); }

private:
	TGenericDoubleIIRInterpolator<float> Interpolator;
};
