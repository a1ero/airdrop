class CfgPatches
{
	class AirdropParachute
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"AirdropUpgraded"
		};
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class ItemBase;
	class AirdropParachuteBase: HouseNoDestruct
	{
		scope=0;
		displayName="Airdrop Parachute";
		model="\Airdrop-Upgraded\Parachute\parachute.p3d";
		overrideDrawArea="8.0";
		forceFarBubble="true";
		carveNavmesh=0;
		rotationFlags=2;
		hiddenSelections[]=
		{
			"parachute"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Parachute\data\parachute_co.paa"
		};
	};
	class AirdropParachute: AirdropParachuteBase
	{
		scope=1;
		displayName="Airdrop Parachute";
		descriptionShort="The parachute that is attached to Airdrop container (NOT SPAWNABLE!).";
	};
};
