//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Misc. Text Items
protected static const string TEXT_ADU_VERSION        = "1.21.07.20";
protected static const string TEXT_ITEM_RANDOM        = "RANDOM";
protected static const string TEXT_ITEM_AIRDROPFLARE  = "AirdropFlare";
protected static const string TEXT_ITEM_PLAYER_CALLED = "Player Called";
protected static const string TEXT_ITEM_CONTAINERS    = "|AirdropContainer|AirdropContainer_Blue|AirdropContainer_Medical|AirdropContainer_Military|AirdropContainer_Yellow|RANDOM|";
protected static const string TEXT_ITEM_SAFEZONES_MSG = "Airdrops cannot be dispatched to your current location ... Airdrop cancelled!";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Notifiction Icons
protected static const string AIRDROP_ICON_AIRDROP_ACFT  = "Airdrop-Upgraded\\data\\Airdrop-Upgraded-ACFT.paa";
protected static const string AIRDROP_ICON_AIRDROP_DROP  = "Airdrop-Upgraded\\data\\Airdrop-Upgraded-Drop.paa";
protected static const string AIRDROP_ICON_AIRDROP_RANGE = "Airdrop-Upgraded\\data\\Airdrop-Upgraded-Range.paa";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File Paths
protected static const string JSON_PATH_SETTINGS_AIRDROP = "$profile:Airdrop\\";
protected static const string TEXT_PATH_LOGS_AIRDROP     = "$profile:Airdrop\\Logs\\";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Settings & Log Files
protected static const string JSON_FILE_SETTINGS_OLD_AIRDROP = "$profile:Airdrop\\Settings.json";
protected static const string JSON_FILE_SETTINGS_AIRDROP_OLD = "$profile:Airdrop\\SettingsOld.json";
protected static const string JSON_FILE_SETTINGS_AIRDROP     = "$profile:Airdrop\\AirdropSettings.json";
protected static const string JSON_FILE_SETTINGS_BAK_AIRDROP = "$profile:Airdrop\\AirdropSettings_bakup_###.json";
protected static const string JSON_FILE_SAFEZONES            = "$profile:Airdrop\\AirdropSafezones.json";
protected static const string JSON_FILE_BAK_SAFEZONES        = "$profile:Airdrop\\AirdropSafezones_bakup.json";
protected static const string JSON_FILE_EXPORT_DROPZONES     = "$profile:VPPMapAirdrop.json";
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// >>> Airdrop.EOnCallAirdrop
// "Aircraft was dispatched to '|m_DropZone.Title|' from the |cardinal|."
// "Aircraft was dispatched to the player's location (|m_AircraftSpawn[0]|/|m_AircraftSpawn[2]|) from the |cardinal|."

// >>> AirdropPlane.InitializeAircraft
// "AirdropPlane was NOT Initialized!  You are NOT allowed to spawn in AirdropPlanes directly!"

// >>> AirdropPlane.EOnBeforeDrop 'Proximity'
// m_LogMessage = "The Aircraft is |m_Settings.Messages.Proximity/m_MetersPerkilometer| Km from |m_DropZone.Title|."

// >>> AirdropPlane.EOnDrop
// m_LogMessage = "Supplies have been dropped near |m_DropZone.Title|."
// m_LogMessage = "Supplies have been dropped near the player's location.";