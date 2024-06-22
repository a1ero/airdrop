//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Airdrop Settings Classes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_ControlParams
{
	string Version;			// The current version of the mod (reverse date 1.yy.mm.dd)
	int Interval;			// How often should random spawns occur (minutes)
	int Mode;				// Airdrop types (0 = none, 1 = Random, 2 = Flare Called, 3 = both)
	int AD_LogManager;		// Log airdrop manager events ...random and player called drops (0 = off, 1 = On)
	int AD_LogAircraft;		// Log aircraft events ... spawn, proximity, zombies, drop, & despawn (0 = off, 1 = On)
	int AD_LogContainer;	// Log container events ... spawn, type, loot, & despawn (0 = off, 1 = On)
	int MinimumPlayers;		// Disable airdrops if there are not enough players on the server
	int MaxBackupDays;		// The maximum number of days backup files will be maintainerd (0 = off, 1 or more = on)
	int MaxLogDays;			// The maximum number of days log files will be maintainerd (0 = off, 1 or more = on)
	int SmokeTrails;		// Shoule the containers display smoke trails (0 = NO, 1 = YES default)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_MapParams
{
	float Width;			// The width(X) of the map, e.g. Chern is 15360 (meters) 
	float Height;			// The height Z) of the map, e.g. Chern is 15360 (meters) 
	float Offset;			// Distance off the map the aircraft it to be spawned/destroyes (meters)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// >>> We need to convert this to a class array for each aircraft type/models <<<
class AD_AircraftParams
{
	int AirSpeedKIAS;		// The indicated air speed the drop aircraft is moving at (KNOTS)
	int StartAltMSL;		// The altitude above sea level the aircraft starts it run at (meters)
	int DropAGL;			// The altitude above the ground the aircraft drops supplies at (meters)
	int DropOffset;			// How far below aircraft should we spawn in the container (meters)
	int DropAccuracy;		// The how short the drop could be from the specified drop location (meters, e.g. early drop)
	float TerrainFollowing;	// How aggressively the aircraft attempts to follow the terrain if lies over (0.0 = none to 1.0 = exact)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_MessageParams
{
	int Mode;				// Message Type (0 = none, 1 = Form, 2 = Text, 3 = Both)
	int Duration;			// How long shold the FORM messages be displayed (seconds)
	int Proximity;			// Distance to drop when aircraft sends a proximity message, 0 = no message (meters)
	int ImperialUnits;		// Display messages in Imperial units (true) or Metric units (false)
	int NotificationARGB[4];// ARGB Color values for DayZ Expansion Notifiactions messages
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_ContainerParams
{
	float TriggerAGL;		// Altitdue above ground to trigger item spawn (meters)
	float FallRate;			// Fall rate of container under parachute (MPS)
	float StandUpTimer;		// Time to wait before reorienting the container (seconds)
	float SpawnMin;			// Minimum distance from container to spawn ground items (meters)
	float SpawnMax;			// Maximum distance from container to spawn ground items (meters)
	float SpawnOffset;		// How far off the ground should items be spawned (meters)
	float WindStrength;		// How much is the falling container effected by wind, 0 = no effect, 1 = full effect, 2 = double effect (percent)
	int Lifespan;			// How long should the lifespan of contaner be
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_DropZone
{
	string Title;			// Name or title of the drop location (string)
	string Location;		// Geographical classification of location (string)
	string DropType;		// Specific items to be droppd (default = 'RANDOM')
	float X, Z;				// Map coordinated (meters, integer)
	int Zombies;			// Number of zombies to be spawned at this location (integer)
	int Radius;				// Radius around drop location to spawn zombien in (meters, integer) 
	int DropAccuracy;		// Drop accuracy, random offset from drop location (meters, integer)
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AD_DropZone(string title, string location, float x, float z, int zombies, int radius, int dropaccuracy)
	{
		Title = title;
		Location = location;
		DropType = TEXT_ITEM_RANDOM;
		X = x;
		Z = z;
		Zombies = zombies;
		Radius = radius;
		DropAccuracy = dropaccuracy;
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_Location
{
	string Title;             // In game name of location
	ref TStringArray Zombies; // List of zombies for this location
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AD_Location(string title, ref TStringArray zombies)
	{
		Title = title;
		Zombies = zombies;
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_DropType
{
	string Title;			// Unique name of this drop type(string)
	string Container;		// Container type to be dropped (string)
	int Quantity;			// Number of items to be spawned (integer)
	float AddFlare;			// Chance of flare being included in the drop (0 = not included to 100 = always included)
	float SpawnMin;			// Minimum distance from container to spawn ground items (meters)
	float SpawnMax;			// Maximum distance from container to spawn ground items (meters)
	float SpawnOffset;		// How far off the ground should items be spawned (meters)
	int Lifespan;			// How long should the lifespan of spawned objects be, 0 don't set a limit (use economy) > 0 sets lifespan (miniutes)
	ref TStringArray Items;	// List of items tha tcan be spawned (string array)
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AD_DropType(string title, string container, int quantity, float addflare, float spawnmin, float spawnmax, float spawnoffset, int lifespan, ref TStringArray items)
	{
		Title = title;
		Container = container;
		Quantity = quantity;
		AddFlare = addflare;
		SpawnMin = spawnmin;
		SpawnMax = spawnmax;
		SpawnOffset = spawnoffset;
		Lifespan = lifespan;
		Items = items;
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AD_VPPMapParams
{
	int ExportMap;		// Export DropZone data in VPP Map format
	int TitleMode;		// Export DropZone data using 'Title' (0) or Autonumber (1, e.g. 'AD-##')
	int MapColor[3];	// VPP Map Marker RGB Color values
	string MapIcon;		// VPP Map Marker Icon filename (default = 'waypointeditor_CA')
	int IsActive;		// If the icon active?
	int Is3DActive;		//
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void AD_VPPMap(int exportmap, int titlemode, int red, int green, int blue, string mapicon, int isactive, int is3dactive )
	{
		ExportMap = exportmap;
		TitleMode = titlemode;
		MapColor[3] = { red, green, blue };
		MapIcon = mapicon;
		IsActive = isactive;
		Is3DActive = is3dactive;
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AirdropSettings
{
	// This is the current/latest version
	static const string m_Version = TEXT_ADU_VERSION;
	// Main settings classes 
	ref AD_ControlParams Controls;
	ref AD_MapParams Map;
	ref AD_AircraftParams Aircraft;
	ref AD_MessageParams Messages;
	ref AD_ContainerParams Container;
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Other settings
	ref array< ref AD_Location > Locations;
	ref array< ref AD_DropZone > DropZones;
	ref array< ref AD_DropType > DropTypes;
	ref AD_VPPMapParams VPP_Map;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AirdropSettings()
	{
		Controls = new ref AD_ControlParams;
		Map = new ref AD_MapParams;
		Aircraft = new ref AD_AircraftParams;
		Messages = new ref AD_MessageParams;
		Container = new ref AD_ContainerParams;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Locations = new ref array< ref AD_Location >;
		DropZones = new ref array< ref AD_DropZone >;
		DropTypes = new ref array< ref AD_DropType >;
		Print("[AirdropSettings] Settings classes created.");
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AirdropDefaults()
	{
		TStringArray TempItems;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Main Setup defaults 
		Controls.Version = m_Version;
		Controls.Interval = 60;
		Controls.Mode = 3;
		Controls.AD_LogManager = 1;
		Controls.AD_LogAircraft = 1;
		Controls.AD_LogContainer = 1;
		Controls.MinimumPlayers = 1;
		Controls.MaxBackupDays = 5;
		Controls.MaxLogDays = 2;
		Controls.SmokeTrails = 1;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Mission Map parameters
		Map.Width = 15360;
		Map.Height = 15360;
		Map.Offset = 1000;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Aircraft Setup defaults
		Aircraft.AirSpeedKIAS = 150;
		Aircraft.StartAltMSL = 2438;
		Aircraft.DropAGL = 152;
		Aircraft.DropOffset = -10;
		Aircraft.DropAccuracy = 152;
		Aircraft.TerrainFollowing = 0.5;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Message Setup defaults
		Messages.Mode = 3;
		Messages.Duration = 15;
		Messages.Proximity = 1500;
		Messages.ImperialUnits = 0;
		Messages.NotificationARGB = { 255, 0, 100, 200 };
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Container setup defaults
		Container.TriggerAGL = 3.0;
		Container.FallRate = 4.0;
		Container.StandUpTimer = 3.0;
		Container.SpawnMin = 1.0;
		Container.SpawnMax = 3.0;
		Container.SpawnOffset = 0.05;
		Container.WindStrength = 0.1;
		Container.Lifespan = 60;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Location Types
		TempItems = {
			"ZmbF_CitizenANormal_Beige","ZmbF_CitizenANormal_Blue","ZmbF_CitizenANormal_Brown","ZmbF_CitizenBSkinny","ZmbF_Clerk_Normal_Blue","ZmbF_Clerk_Normal_Green","ZmbF_Clerk_Normal_Red","ZmbF_Clerk_Normal_White","ZmbF_DoctorSkinny","ZmbF_JoggerSkinny_Blue",
			"ZmbF_JoggerSkinny_Brown","ZmbF_JoggerSkinny_Green","ZmbF_JoggerSkinny_Red","ZmbF_JournalistNormal_Blue","ZmbF_JournalistNormal_Green","ZmbF_JournalistNormal_Red","ZmbF_JournalistNormal_White","ZmbF_NurseFat","ZmbF_ParamedicNormal_Blue","ZmbF_ParamedicNormal_Green",
			"ZmbF_ParamedicNormal_Red","ZmbF_PatientOld","ZmbF_PoliceWomanNormal","ZmbF_ShortSkirt_beige","ZmbF_ShortSkirt_black","ZmbF_ShortSkirt_brown","ZmbF_ShortSkirt_checks","ZmbF_ShortSkirt_green","ZmbF_ShortSkirt_grey","ZmbF_ShortSkirt_red",
			"ZmbF_ShortSkirt_stripes","ZmbF_ShortSkirt_white","ZmbF_ShortSkirt_yellow","ZmbF_SkaterYoung_Brown","ZmbF_SkaterYoung_Striped","ZmbF_SkaterYoung_Violet","ZmbM_CitizenASkinny_Blue","ZmbM_CitizenASkinny_Brown","ZmbM_CitizenASkinny_Grey","ZmbM_CitizenASkinny_Red",
			"ZmbM_CitizenBFat_Blue","ZmbM_CitizenBFat_Green","ZmbM_CitizenBFat_Red","ZmbM_ClerkFat_Brown","ZmbM_ClerkFat_Grey","ZmbM_ClerkFat_Khaki","ZmbM_ClerkFat_White","ZmbM_CommercialPilotOld_Blue","ZmbM_CommercialPilotOld_Brown","ZmbM_CommercialPilotOld_Grey",
			"ZmbM_CommercialPilotOld_Olive","ZmbM_DoctorFat","ZmbM_JacketNormal_greenchecks","ZmbM_Jacket_beige","ZmbM_Jacket_black","ZmbM_Jacket_blue","ZmbM_Jacket_bluechecks","ZmbM_Jacket_brown","ZmbM_Jacket_greenchecks","ZmbM_Jacket_grey",
			"ZmbM_Jacket_khaki","ZmbM_Jacket_magenta","ZmbM_Jacket_stripes","ZmbM_JoggerSkinny_Blue","ZmbM_JoggerSkinny_Green","ZmbM_JoggerSkinny_Red","ZmbM_JournalistSkinny","ZmbM_ParamedicNormal_Black","ZmbM_ParamedicNormal_Blue","ZmbM_ParamedicNormal_Green",
			"ZmbM_ParamedicNormal_Red","ZmbM_PatientSkinny","ZmbM_PatrolNormal_Autumn","ZmbM_PatrolNormal_Flat","ZmbM_PatrolNormal_PautRev","ZmbM_PatrolNormal_Summer","ZmbM_PolicemanFat","ZmbM_PolicemanSpecForce","ZmbM_PrisonerSkinny","ZmbM_SkaterYoung_Blue",
			"ZmbM_SkaterYoung_Brown","ZmbM_SkaterYoung_Green","ZmbM_SkaterYoung_Grey" };
		Locations.Insert(new ref AD_Location("City", TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		TempItems = {
			"ZmbF_HikerSkinny_Blue","ZmbF_HikerSkinny_Green","ZmbF_HikerSkinny_Grey","ZmbF_HikerSkinny_Red","ZmbF_MilkMaidOld_Beige","ZmbF_MilkMaidOld_Black","ZmbF_MilkMaidOld_Green","ZmbF_MilkMaidOld_Grey","ZmbF_SurvivorNormal_Blue","ZmbF_SurvivorNormal_Orange",
			"ZmbF_SurvivorNormal_Red","ZmbF_SurvivorNormal_White","ZmbF_VillagerOld_Blue","ZmbF_VillagerOld_Green","ZmbF_VillagerOld_Red","ZmbF_VillagerOld_White","ZmbM_FarmerFat_Beige","ZmbM_FarmerFat_Blue","ZmbM_FarmerFat_Brown","ZmbM_FarmerFat_Green",
			"ZmbM_HermitSkinny_Beige","ZmbM_HermitSkinny_Black","ZmbM_HermitSkinny_Green","ZmbM_HermitSkinny_Red","ZmbM_HikerSkinny_Blue","ZmbM_HikerSkinny_Green","ZmbM_HikerSkinny_Yellow","ZmbM_HunterOld_Autumn","ZmbM_HunterOld_Spring","ZmbM_HunterOld_Summer","ZmbM_HunterOld_Winter",
			"ZmbM_MotobikerFat_Beige","ZmbM_MotobikerFat_Black","ZmbM_MotobikerFat_Blue","ZmbM_SurvivorDean_Black","ZmbM_SurvivorDean_Blue","ZmbM_SurvivorDean_Grey","ZmbM_VillagerOld_Blue","ZmbM_VillagerOld_Green","ZmbM_VillagerOld_White","ZmbM_priestPopSkinny" };
		Locations.Insert(new ref AD_Location("Country", TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		TempItems = {
			"ZmbF_BlueCollarFat_Green","ZmbF_BlueCollarFat_Red","ZmbF_BlueCollarFat_White","ZmbF_MechanicNormal_Beige","ZmbF_MechanicNormal_Green","ZmbF_MechanicNormal_Grey","ZmbF_MechanicNormal_Orange","ZmbM_ConstrWorkerNormal_Beige","ZmbM_ConstrWorkerNormal_Black","ZmbM_ConstrWorkerNormal_Green",
			"ZmbM_ConstrWorkerNormal_Grey","ZmbM_FirefighterNormal","ZmbM_FishermanOld_Blue","ZmbM_FishermanOld_Green","ZmbM_FishermanOld_Grey","ZmbM_FishermanOld_Red","ZmbM_HandymanNormal_Beige","ZmbM_HandymanNormal_Blue","ZmbM_HandymanNormal_Green","ZmbM_HandymanNormal_Grey",
			"ZmbM_HandymanNormal_White","ZmbM_HeavyIndustryWorker","ZmbM_MechanicSkinny_Blue","ZmbM_MechanicSkinny_Green","ZmbM_MechanicSkinny_Grey","ZmbM_MechanicSkinny_Red","ZmbM_OffshoreWorker_Green","ZmbM_OffshoreWorker_Orange","ZmbM_OffshoreWorker_Red","ZmbM_OffshoreWorker_Yellow" };
		Locations.Insert(new ref AD_Location("Industrial", TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		TempItems = { "ZmbM_PatrolNormal_Autumn","ZmbM_PatrolNormal_Flat","ZmbM_PatrolNormal_PautRev","ZmbM_PatrolNormal_Summer","ZmbM_SoldierNormal" };
		Locations.Insert(new ref AD_Location("Military", TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		TempItems = { "Animal_CanisLupus_White","Animal_CanisLupus_Grey" };
		Locations.Insert(new ref AD_Location("Wolfpack", TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Set drop locations
		DropZones.Insert(new ref AD_DropZone(TEXT_ITEM_PLAYER_CALLED, TEXT_ITEM_RANDOM, 0, 0, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Balota", "Military", 5043, 2505, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Berezino", "City", 11464, 8908, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Chernogorks", "City", 6128, 2497, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Elektrozavodsk", "City", 9371, 2229, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Krasnostav", "Military", 12159, 12583, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Myshkino", "Country", 2036, 7491, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Northwest Airfield", "Military", 4807, 9812, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Novodmitrovsk", "City", 11125, 14040, 10, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Novy Sobor", "Country", 7436, 7720, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Skalisty Island", "Country", 13452, 3112, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Sosnovka", "Military", 2700, 6193, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Stary Sobor", "Country", 5823, 7764, 5, 50, 100));
		DropZones.Insert(new ref AD_DropZone("Zelenogorks", "City", 2351, 5393, 10, 50, 100));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Set default loot tier #1 (name, container, loot)
		TempItems = { "Izh18", "Mossin9130", "AmmoBox_762x54_20Rnd", "AmmoBox_762x39_20Rnd", "M4A1", "BakedBeansCan", "WaterBottle", "FNX45", "Mag_STANAG_30Rnd", "AKM", "Mag_AKM_30Rnd", "Rice", "PowderedMilk", "Vodka" };
		DropTypes.Insert(new ref AD_DropType("Military Weapons", "AirdropContainer", 20, 10, Container.SpawnMin, Container.SpawnMax, Container.SpawnOffset, Container.Lifespan, TempItems));
		// Set default loot tier #2 (name, container, loot)
		TempItems = { "Canteen", "PowderedMilk", "Marmalade", "BakedBeansCan", "PeachesCan", "SpaghettiCan", "TunaCan", "Pot", "VitaminBottle", "BoxCerealCrunchin", "TacticalBaconCan" };
		DropTypes.Insert(new ref AD_DropType("Food/Medical", "AirdropContainer_Medical", 20, 10, Container.SpawnMin, Container.SpawnMax, Container.SpawnOffset, Container.Lifespan, TempItems));
		// Set default loot tier #2 (name, container, loot)
		TempItems = { "LandMineTrap","TTSKOPants","TacticalBaconCan","M4A1","PlateCarrierComplete","MilitaryBoots_Redpunk","CombatBoots_Grey","FNX45","Mag_STANAG_30Rnd","AKM","Mag_AKM_30Rnd","LargeTent","Izh18","Mosin9130","Mosin_Bayonet","Mosin_Compensator","AmmoBox_762x54_20Rnd","AmmoBox_762x39_20Rnd" };
		DropTypes.Insert(new ref AD_DropType("Military Weapons", "AirdropContainer_Military", 20, 10, Container.SpawnMin, Container.SpawnMax, Container.SpawnOffset, Container.Lifespan, TempItems));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// VPP Map Export Settings
		VPP_Map.ExportMap = 1;
		VPP_Map.TitleMode = 1;
		VPP_Map.MapColor = { 0, 200, 200 };
		VPP_Map.MapIcon = "waypointeditor_CA.paa";
		VPP_Map.IsActive = 0;
		VPP_Map.Is3DActive = 0;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static string GetVersion()
	{
		// Return Airdrop version to caller
		return m_Version;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static ref AirdropSettings Load()
	{
		bool NeedsConversion = false;
		ref AirdropSettings m_Settings = new AirdropSettings();
		ref AirdropSettingsOld m_SettingsOld = new AirdropSettingsOld();
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we don't have the settings folder
		if (!FileExist(JSON_PATH_SETTINGS_AIRDROP))
		{
			// Create settings folder
			MakeDirectory(JSON_PATH_SETTINGS_AIRDROP);
			Print("[AirdropSettings.Load] Settings folder created.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we don't have the logs folder
		if (!FileExist(TEXT_PATH_LOGS_AIRDROP))
		{
			// Create logs folder 
			MakeDirectory(TEXT_PATH_LOGS_AIRDROP);
			Print("[AirdropSettings.Load] Logs folder created.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have an OLD settings file but not a new one
		if (FileExist(JSON_FILE_SETTINGS_OLD_AIRDROP) && !FileExist(JSON_FILE_SETTINGS_AIRDROP))
		{
			// Load It
			JsonFileLoader<AirdropSettingsOld>.JsonLoadFile(JSON_FILE_SETTINGS_OLD_AIRDROP, m_SettingsOld);
			// Flag that conversion is needed
			NeedsConversion = true;
			// Log the folders & settings creation
			Print("[AirdropSettings.Load] Old settings file loaded.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have a settings file
		if (FileExist(JSON_FILE_SETTINGS_AIRDROP))
		{
			// Load it
			JsonFileLoader<AirdropSettings>.JsonLoadFile(JSON_FILE_SETTINGS_AIRDROP, m_Settings);
			// File name/path setup
			string m_Timestamp = AirdropBase.ADU_GetFileDateTimeStamp(true);
			string m_BackupFile = JSON_FILE_SETTINGS_BAK_AIRDROP;
			// Append datetime stamp to backup filename
			m_BackupFile.Replace("###", m_Timestamp);
			// Create backup copy of original file
			CopyFile(JSON_FILE_SETTINGS_AIRDROP, m_BackupFile);
			// Check for new/missing settings
			m_Settings = CheckVersion(m_Settings);
			// Override version
			m_Settings.Controls.Version = m_Version;
			// Save it (with updated & reformatted values)
			JsonFileLoader<AirdropSettings>.JsonSaveFile(JSON_FILE_SETTINGS_AIRDROP, m_Settings);
			// Log the folders & settings creation
			Print("[AirdropSettings.Load] Settings file loaded & resaved.");
		}
		else
		{
			// Create default settings
			m_Settings.AirdropDefaults();
			// Save it (with updated & reformatted values)
			JsonFileLoader<AirdropSettings>.JsonSaveFile(JSON_FILE_SETTINGS_AIRDROP, m_Settings);
			// Log the folders & settings creation
			Print("[AirdropSettings.Load] Settings created & Defaults loaded.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have an old settings file
		if (NeedsConversion) {
			// Roll old settings into new settings
			m_Settings = Update(m_Settings, m_SettingsOld);
			// Check for new/missing settings
			m_Settings = CheckVersion(m_Settings);
			// Save it (with updated & reformatted values)
			JsonFileLoader<AirdropSettings>.JsonSaveFile(JSON_FILE_SETTINGS_AIRDROP, m_Settings);
			// Kill old settings file
			DeleteFile(JSON_FILE_SETTINGS_OLD_AIRDROP);
			// Log the folders & settings creation
			Print("[AirdropSettings.Load] New Settings file updated with old settings values & resaved.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Return settings
		return m_Settings;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static private AirdropSettings CheckVersion(AirdropSettings m_Settings) {
		bool m_FixedDropTypes = false;
		bool m_FixedLocations = false;
		bool m_FixedPlayerCalled = true;
		//·····································································································································································································
		// For each DropZone... 
		for (int i3 = 0; i3 < m_Settings.DropZones.Count(); i3++)
		{
			//·····································································································································································································
			// Is the first DropZone titled 'Player Called'?
			if (i3 == 0 && m_Settings.DropZones[i3].Title == TEXT_ITEM_PLAYER_CALLED) {
				m_FixedPlayerCalled = false;
			}
			//·····································································································································································································
			// If the DropZones.DropType was not set or set incorrectly...
			if ((m_Settings.DropZones[i3].DropType == "") || (m_Settings.DropZones[i3].DropType == "random") || (m_Settings.DropZones[i3].DropType == "Random"))
			{
				// Set default DropType to 'RANDOM'...
				m_Settings.DropZones[i3].DropType = TEXT_ITEM_RANDOM;
				m_FixedDropTypes = true;
			}
			//·····································································································································································································
			// If the DropZones.Location was not set or set incorrectly...
			if ((m_Settings.DropZones[i3].Location == "") || (m_Settings.DropZones[i3].Location == "random") || (m_Settings.DropZones[i3].Location == "Random"))
			{
				// Set default Location to 'RANDOM'
				m_Settings.DropZones[i3].Location = TEXT_ITEM_RANDOM;
				m_FixedLocations = true;
			}
			//·····································································································································································································
			// Convert coordinates to Integers 
			m_Settings.DropZones[i3].X = Math.Round(m_Settings.DropZones[i3].X);
			m_Settings.DropZones[i3].Z = Math.Round(m_Settings.DropZones[i3].Z);
		}
		//·····································································································································································································
		// Do we need to add a 'Player Called' DropZone?
		if (m_FixedPlayerCalled) {
			// Insert 'Player Called' DropZone to top of List
			m_Settings.DropZones.InsertAt(new ref AD_DropZone(TEXT_ITEM_PLAYER_CALLED, TEXT_ITEM_RANDOM, 0, 0, 10, 50, 100), 0);
		}
		//·····································································································································································································
		// Do we need to log any DropZone.DropType
		if (m_FixedDropTypes) {
			// Log corrections
			Print("[AirdropSettings.CheckVersion] Corrected all DropZone.DropType values ('', 'random', or 'Random' » 'RANDOM'");
		}
		//·····································································································································································································
		// Do we need to log any DropZone.Locations
		if (m_FixedLocations) {
			// Log corrections
			Print("[AirdropSettings.CheckVersion] Corrected all DropZone.Locations values ('', 'random', or 'Random' » 'RANDOM'");
		}
		//·····································································································································································································
		// Get the integer of the current version
		int mver = ConvertVersion(m_Version);
		int lver = ConvertVersion(m_Settings.Controls.Version);
		//·····································································································································································································
		// If loaded version < m_Version
		if (lver < mver) {
			// New settings cleanup properties...
			m_Settings.Controls.MaxBackupDays = 5;
			m_Settings.Controls.MaxLogDays = 2;
			m_Settings.Controls.MinimumPlayers = 1;
			// Container setup...
			m_Settings.Container.Lifespan = 60;
			// For each DropType...
			for (int i1 = 0; i1 < m_Settings.DropTypes.Count(); i1++)
			{
				// Set new DropType field defaults
				if (m_Settings.DropTypes[i1].SpawnMin == 0)
				{
					m_Settings.DropTypes[i1].SpawnMin = m_Settings.Container.SpawnMin;
				}
				if (m_Settings.DropTypes[i1].SpawnMax == 0)
				{
					m_Settings.DropTypes[i1].SpawnMax = m_Settings.Container.SpawnMax;
				}
				if (m_Settings.DropTypes[i1].SpawnOffset == 0)
				{
					m_Settings.DropTypes[i1].SpawnOffset = m_Settings.Container.SpawnOffset;
				}
				if (m_Settings.DropTypes[i1].Lifespan == 0)
				{
					m_Settings.DropTypes[i1].Lifespan = m_Settings.Container.Lifespan;
				}
			}
			// VPP Map Settings
			m_Settings.VPP_Map.ExportMap = 1;
			m_Settings.VPP_Map.TitleMode = 1;
			m_Settings.VPP_Map.MapColor = { 0, 200, 200 };
			m_Settings.VPP_Map.MapIcon = "waypointeditor_CA.paa";
			m_Settings.VPP_Map.IsActive = 0;
			m_Settings.VPP_Map.Is3DActive = 0;
			// Log corrections
			Print("[AirdropSettings.CheckVersion] Populated all DropType Min/Max values.");
		}
		//·····································································································································································································
		bool m_FixedRandoms = false;
		bool m_FixedContainers = false;
		//·····································································································································································································
		// For each DropType...
		for (int i2 = 0; i2 < m_Settings.DropTypes.Count(); i2++)
		{
			// If the DropType.Container was not set or set incorrectly...
			if ((m_Settings.DropTypes[i2].Container == "") || (m_Settings.DropTypes[i2].Container == "random") || (m_Settings.DropTypes[i2].Container == "Random"))
			{
				// Set default DropType to 'RANDOM'
				m_Settings.DropTypes[i2].Container = TEXT_ITEM_RANDOM;
				m_FixedRandoms = true;
			}
			// If this is NOT a recognized contained
			if (!TEXT_ITEM_CONTAINERS.Contains("|" + m_Settings.DropTypes[i2].Container + "|")) {
				m_Settings.DropTypes[i2].Container = "AirdropContainer";
				m_FixedContainers = true;
			}
		}
		//·····································································································································································································
		// If we corrected any randoms
		if (m_FixedRandoms) {
			// Log corrections
			Print("[AirdropSettings.CheckVersion] Corrected all DropType.Contaner values ('', 'random', or 'Random' » 'RANDOM'");
		}
		//·····································································································································································································
		// If we corrected any randoms
		if (m_FixedContainers) {
			// Log corrections
			Print("[AirdropSettings.CheckVersion] Corrected all DropType.Contaner names (must be 'AirdropContainer', 'AirdropContainer_Blue', 'AirdropContainer_Medical', 'AirdropContainer_Military', 'AirdropContainer_Yellow', or 'RANDOM')");
		}
		//·····································································································································································································
		// Return altered settings
		return m_Settings;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static private int ConvertVersion(string version) {
		// Remove decimals
		version.Replace(".", "");
		// Return integer 
		return version.ToInt();
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static ref AirdropSettings Update(ref AirdropSettings settings, ref AirdropSettingsOld settingsold)
	{
		TStringArray TempItems;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Main Setup defaults « Interval
		settings.Controls.Interval = settingsold.Interval;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Map Setup defaults « (nothing)
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Aircraft Setup defaults « Height
		settings.Aircraft.DropAGL = settingsold.Height;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Message Setup defaults « (nothing)
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Container setup defaults « (nothing)
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Remove existing default locations
		delete settings.Locations;
		settings.Locations = new ref array< ref AD_Location >;
		// Location Types « load AirdropZombies
		settings.Locations.Insert(new ref AD_Location("Zombies", settingsold.AirdropZombies));
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Remove existing default dropzones
		delete settings.DropZones;
		settings.DropZones = new ref array< ref AD_DropZone >;
		// Add player called DropZone			
		settings.DropZones.Insert(new ref AD_DropZone("Player Caleld", TEXT_ITEM_RANDOM, 0, 0, 10, 50, 100));
		// Set drop location vars
		string title;
		string location;
		float x, z;
		int zombies;
		int radius;
		int dropaccuracy;
		// For each old drop location
		for (int dz = 0; dz < settingsold.DropLocations.Count(); dz++) {
			title = settingsold.DropLocations[dz].Name;
			x = settingsold.DropLocations[dz].x;
			z = settingsold.DropLocations[dz].y;
			radius = settingsold.DropLocations[dz].Radius;
			// Insert dropzone
			settings.DropZones.Insert(new ref AD_DropZone(title, "Zombies", x, z, settingsold.Infected, 50, radius));
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Remove existing loot types
		delete settings.DropTypes;
		settings.DropTypes = new ref array< ref AD_DropType >;
		// Set default loot tier #1 (name, container, loot)
		string container;
		int quantity;
		float addflare;
		ref TStringArray items;
		// For each old loot tier
		for (int lt = 0; lt < settingsold.LootTiers.Count(); lt++) {
			container = settingsold.LootTiers[lt].Name;
			items = settingsold.LootTiers[lt].Loot;
			// Do we have an airdrop flare in the loot list?
			if (settingsold.LootTiers[lt].Loot.Find(TEXT_ITEM_AIRDROPFLARE) >= 0) {
				// Count the number of occurances in the loot list
				int adfs = ADU_CountItems(TEXT_ITEM_AIRDROPFLARE, items, true);
				addflare = adfs / items.Count();
			}
			// Delete AirdropFlares from List
			//items.RemoveItem(TEXT_ITEM_AIRDROPFLARE);
			// Set title based on container
			switch (container) {
			case "AirdropContainer": {
				title = "Standard Loot";
				break;
			}
			case "AirdropContainer_Medical": {
				title = "Medical Loot";
				break;
			}
			case "AirdropContainer_Military": {
				title = "Military Loot";
				break;
			}
			}
			// Insert Drop type
			settings.DropTypes.Insert(new ref AD_DropType(title, container, settingsold.Items, 10, settings.Container.SpawnMin, settings.Container.SpawnMax, settings.Container.SpawnOffset, settings.Container.Lifespan, items));
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Send back the updated settings
		return settings;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Counts the number of occurrances of an item in a text array of items
	static int ADU_CountItems(string item, TStringArray items, bool removeitem = false) {
		int ct = 0;
		// For each item in teh items list
		for (int ix = items.Count() - 1; ix >= items.Count(); ix--) {
			// Is this the item we are looking for?
			if (items[ix] == item) {
				// Do we want to remove items found?
				if (removeitem) {
					// Remove airdrop flares
					items.RemoveItem(item);
				}
				// Increment counter
				ct++;
			}
		}
		// Return count of items
		return ct;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
