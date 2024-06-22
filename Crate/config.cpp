class CfgPatches
{
	class AirdropCrate
	{
		units[]=
		{
			"AirdropFlare"
		};
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
	class Container_Base;
	class HouseNoDestruct;
	class Inventory_Base;
	class ItemBase;
	class Roadflare;
	class AirdropContainerBase: HouseNoDestruct
	{
		scope=0;
		displayName="Airdrop Container";
		model="\Airdrop-Upgraded\Crate\container.p3d";
		overrideDrawArea="8.0";
		forceFarBubble="true";
		carveNavmesh=1;
		heavyItem=1;
		physLayer="item_large";
		rotationFlags=2;
		itemSize[]={20,20};
		weight=1000000;
		class Cargo
		{
			itemsCargoSize[]={10,100};
		};
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_co.paa"
		};
	};
	class AirdropContainer: AirdropContainerBase
	{
		scope=1;
		displayName="Airdrop Container Red";
		descriptionShort="Red Airdrop container with camo netting (NOT SPAWNABLE!).";
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_co.paa"
		};
	};
	class AirdropContainer_Medical: AirdropContainerBase
	{
		scope=1;
		displayName="Airdrop Container Red Cross";
		descriptionShort="Red with white cross Airdrop container with camo netting (NOT SPAWNABLE!).";
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_med_co.paa"
		};
	};
	class AirdropContainer_Military: AirdropContainerBase
	{
		scope=1;
		displayName="Airdrop Container Military";
		descriptionShort="Camouflaged Airdrop container with camo netting (NOT SPAWNABLE!).";
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_mil_co.paa"
		};
	};
	class AirdropContainer_Blue: AirdropContainerBase
	{
		scope=1;
		displayName="Airdrop Container Blue";
		descriptionShort="Blue Airdrop container with camo netting (NOT SPAWNABLE!).";
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_blu_co.paa"
		};
	};
	class AirdropContainer_Yellow: AirdropContainerBase
	{
		scope=1;
		displayName="Airdrop Container Yellow";
		descriptionShort="Yellow Airdrop container with camo netting (NOT SPAWNABLE!).";
		hiddenSelections[]=
		{
			"body"
		};
		hiddenSelectionsTextures[]=
		{
			"Airdrop-Upgraded\Crate\data\airdrop_yel_co.paa"
		};
	};
	class AirdropFlare: Roadflare
	{
		scope=2;
		displayName="Airdrop Flare";
		descriptionShort="Activate to call an Airdrop to your current location (single use).";
	};
};
