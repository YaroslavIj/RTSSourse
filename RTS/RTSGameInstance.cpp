// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSGameInstance.h"

bool URTSGameInstance::GetAllyUnitInfo(TSubclassOf<AAllyUnit> UnitClass, FUnitInfo &OutInfo)
{
	/*if(AllyUnitsInfo)
	{		
		for(FName RowName : AllyUnitsInfo->GetRowNames())
		{
			if (FUnitInfo* Row = AllyUnitsInfo->FindRow<FUnitInfo>(RowName, FString()))
			{
				if(Row->UnitClass == UnitClass)
				{
					OutInfo = *Row;
					return true;
				}
			}
		}
	}*/
	for (FUnitInfo UnitInfo : AllyUnitsInfo)
	{
		if (UnitInfo.UnitClass == UnitClass)
		{
			OutInfo = UnitInfo;
			return true;
		}
	}
	return false;
}

bool URTSGameInstance::RemovePoints(int32 SumPoints)
{
	if (Points - SumPoints >= 0)
	{
		Points -= SumPoints;
		return true;
	}
	return false;
}

bool URTSGameInstance::UpdateAllyUnitParameter(TSubclassOf<AAllyUnit> UnitClass, FName ParameterName, float UpdateValue, float& OutValue)
{
	for (FUnitInfo& UnitInfo : AllyUnitsInfo)
	{
		if (UnitInfo.UnitClass == UnitClass)
		{
			if (ParameterName == FName("Damage"))
			{
				if (UnitInfo.Damage + UpdateValue > 0)
				{
					OutValue = UnitInfo.Damage += UpdateValue;
					return true;
				}
			}
			else if(ParameterName == FName("CoefDamage"))
			{
				if(UnitInfo.CoefDamage + UpdateValue > 0)
				{
					OutValue = UnitInfo.CoefDamage += UpdateValue;
					return true;
				}
			}
			else if (ParameterName == FName("AttackRate"))
			{
				if (UnitInfo.AttackRate + UpdateValue > 0)
				{
					OutValue = UnitInfo.AttackRate += UpdateValue;
					return true;
				}
			}
			else if (ParameterName == FName("MovementSpeed"))
			{
				if (UnitInfo.MovementSpeed + UpdateValue > 0)
				{
					OutValue = UnitInfo.MovementSpeed += UpdateValue;
					return true;
				}
			}
		}	
	}
	
	return false;
}
